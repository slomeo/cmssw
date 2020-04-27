#ifndef SIMG4CMS_MUON_MUONSENSITIVEDETECTOR_H
#define SIMG4CMS_MUON_MUONSENSITIVEDETECTOR_H

/** \class MuonSensitiveDetector for DD4hep
 *
 * \author Sergio Lo Meo (sergio.lo.meo@cern.ch)
 *
 * Date: 27 April 2020
 *
 * 
 * Original author of the DD version Arno Straessner, CERN <arno.straessner@cern.ch>
 *
 * Modification of the DD version:
 * 19/05/03. P.Arce
 * Add SimTracks selection
 */

#include "SimG4Core/Notification/interface/Observer.h"
#include "SimG4Core/Notification/interface/BeginOfEvent.h"
#include "SimG4Core/SensitiveDetector/interface/SensitiveTkDetector.h"
#include "DataFormats/GeometryVector/interface/LocalPoint.h"

#include <string>

#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"
#include "Geometry/MuonNumbering/interface/DD4hep_MuonSimHitNumberingScheme.h"

class MuonSlaveSD;
class MuonFrameRotation;
class UpdatablePSimHit;
class MuonSubDetector;
class SimHitPrinter;
class G4Step;
class G4ProcessTypeEnumerator;
class SimTrackManager;
class MuonConstants;

namespace cms {

  class MuonSimHitNumberingScheme;
  class MuonG4Numbering;

  class MuonSensitiveDetector : public SensitiveTkDetector, public Observer<const BeginOfEvent*> {
  public:
    explicit MuonSensitiveDetector(const std::string&,
                                   const edm::EventSetup&,
                                   const SensitiveDetectorCatalog&,
                                   edm::ParameterSet const&,
                                   const SimTrackManager*,
                                   const MuonConstants& muonConstants);
    ~MuonSensitiveDetector() override;
    G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;
    uint32_t setDetUnitId(const G4Step*) override;
    void EndOfEvent(G4HCofThisEvent*) override;

    void fillHits(edm::PSimHitContainer&, const std::string&) override;
    void clearHits() override;

    const MuonSlaveSD* GetSlaveMuon() const { return slaveMuon; }

  protected:
    void update(const BeginOfEvent*) override;

  private:
    inline Local3DPoint cmsUnits(const Local3DPoint& v) { return Local3DPoint(v.x() * 0.1, v.y() * 0.1, v.z() * 0.1); }

    MuonSlaveSD* slaveMuon;
    MuonSimHitNumberingScheme* numbering;
    MuonSubDetector* detector;
    MuonFrameRotation* theRotation;
    MuonG4Numbering* g4numbering;

    bool newHit(const G4Step*);
    void createHit(const G4Step*);
    void updateHit(const G4Step*);
    void saveHit();

    /**
   * Transform from local coordinates of a volume to local coordinates of a parent volume
   * one or more levels up the volume hierarchy: e.g. levelsUp = 1 for immediate parent.
   * This is done by moving from local_1 -> global -> local_2.
   */
    Local3DPoint InitialStepPositionVsParent(const G4Step* currentStep, G4int levelsUp);
    Local3DPoint FinalStepPositionVsParent(const G4Step* currentStep, G4int levelsUp);

    const G4VPhysicalVolume* thePV;
    UpdatablePSimHit* theHit;
    uint32_t theDetUnitId;
    uint32_t newDetUnitId;
    int theTrackID;

    bool printHits;
    SimHitPrinter* thePrinter;

    //--- SimTracks cuts
    float ePersistentCutGeV;
    bool allMuonsPersistent;

    G4ProcessTypeEnumerator* theG4ProcessTypeEnumerator;

    const SimTrackManager* theManager;
  };

}  // namespace cms

#endif  // SIMG4CMS_MUON_MUONSENSITIVEDETECTOR_H
