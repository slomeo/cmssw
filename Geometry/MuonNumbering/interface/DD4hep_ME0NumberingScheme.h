#ifndef Geometry_MuonNumbering_ME0NumberingScheme_h
#define Geometry_MuonNumbering_ME0NumberingScheme_h

/*
//\class ME0NumberingScheme

Description: ME0 Numbering Scheme for DD4hep

//
// Author:  Sergio Lo Meo (sergio.lo.meo@cern.ch) following what Ianna Osburne made for DTs (DD4HEP migration)
//          Created:  02 Dec 2019 
*/

#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"

class MuonBaseNumber;
class MuonConstants;

namespace cms {
  class ME0NumberingScheme {
  public:
    
    ME0NumberingScheme(const MuonConstants& muonConstants);
    void baseNumberToUnitNumber(const MuonBaseNumber&);
    int getDetId() const { return detId; }
    
  private:
    
    const int get(const char*, const MuonConstants&) const;
    void initMe(const MuonConstants& muonConstants);
    void setDetId(int idnew) { detId = idnew; }   
    
    int theRegionLevel;
    int theSectorLevel;
    int theLayerLevel;
    int theRollLevel;
    int theNEtaPart;
    
    int detId;//it was int
  };
}  // namespace cms
#endif
