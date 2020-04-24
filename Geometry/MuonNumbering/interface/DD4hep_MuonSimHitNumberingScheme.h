#ifndef GEOMETRY_MUONNUMBERING_MUONSIMHITNUMBERINGSCHEME_H
#define GEOMETRY_MUONNUMBERING_MUONSIMHITNUMBERINGSCHEME_H

/** \class MuonSimHitNumberingScheme for DD4HEP
 * 
 * Author: Sergio Lo Meo (sergio.lo.meo@cern.ch)
 * Date: 24 April 2020 
 * Original author for DD version: Arno Straessner, CERN <arno.straessner@cern.ch>
 *
 */

#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumberingScheme.h"
//#include "Geometry/MuonNumbering/interface/DD4hep_DTNumberingScheme.h"
//#include "Geometry/MuonNumbering/interface/DD4hep_RPCNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_CSCNumberingScheme.h"
//#include "Geometry/MuonNumbering/interface/DD4hep_GEMNumberingScheme.h"

class MuonBaseNumber;
class MuonSubDetector;
class MuonConstants;

namespace cms {
  class MuonSimHitNumberingScheme : public MuonNumberingScheme {
  public:

    MuonSimHitNumberingScheme(MuonSubDetector*, const MuonConstants& muonConstants);
    //   ~MuonSimHitNumberingScheme() override;
    
int baseNumberToUnitNumber(const MuonBaseNumber&) override;
    
  private:
    MuonSubDetector* theDetector;
    CSCNumberingScheme* theNumberingCSC;
//  DTNumberingScheme* theDTNumbering;
//RPCNumberingScheme* theRPCNumbering;
//  CSCNumberingScheme* theCSCNumbering;
//  GEMNumberingScheme* theGEMNumbering;
  };
}  // namespace cms
#endif
