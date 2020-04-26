#ifndef SIMG4CMS_MUON_MUONGEMFRAMEROTATION_H
#define SIMG4CMS_MUON_MUONGEMFRAMEROTATION_H

/** \class MuonGEMFrameRotation for DD4HEP
 *
 * \author Sergio Lo Meo (sergio.lo.meo@cern.ch)
 * Date: 26 April 2020
 * Original author of the DD version: Arno Straessner, CERN <arno.straessner@cern.ch>
 * 
 *
 */

#include "SimG4CMS/Muon/interface/MuonFrameRotation.h"
#include "SimG4CMS/Muon/interface/DD4hep_MuonG4Numbering.h"

class G4Step;
class MuonConstants;

namespace cms {

  class MuonGEMFrameRotation : public MuonFrameRotation {
  public:
    MuonGEMFrameRotation(const MuonConstants& muonConstants);
    ~MuonGEMFrameRotation() override;
    Local3DPoint transformPoint(const Local3DPoint&, const G4Step*) const override;
  };
}  // namespace cms
#endif
