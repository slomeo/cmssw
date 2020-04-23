#ifndef SIMG4CMS_MUON_MUONG4NUMBERING_H
#define SIMG4CMS_MUON_MUONG4NUMBERING_H

/** \class MuonG4Numbering for DD4HEP migration
 *
 * \author Sergio Lo Meo (sergio.lo.meo@cern.ch)
 * \date 23 April 2020
 * Old DD version: original author Arno Straessner, CERN <arno.straessner@cern.ch>
 *
 * 
 */

#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"

class G4Step;
class MuonBaseNumber;
class MuonConstants;

namespace cms {
 
  class MuonG4Numbering {
  public:
   
    MuonG4Numbering(const MuonConstants& muonConstants);
    ~MuonG4Numbering(){};
    
    MuonBaseNumber PhysicalVolumeToBaseNumber(const G4Step* aStep);
    
  private:
    const int get(const char*, const MuonConstants&) const;
    const int getCopyNoLevel(const int);
    const int getCopyNoSuperNo(const int);
    const int getCopyNoBaseNo(const int);
    const bool copyNoRelevant(const int);
    
    int theLevelPart;
    int theSuperPart;
    int theBasePart;
    int theStartCopyNo;
  };
  
}  // namespace cms
#endif
