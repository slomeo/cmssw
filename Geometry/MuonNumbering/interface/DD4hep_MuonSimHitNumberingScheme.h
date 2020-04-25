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
#include "Geometry/MuonNumbering/interface/DD4hep_DTNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_RPCNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_CSCNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_GEMNumberingScheme.h"
//#include "Geometry/MuonNumbering/interface/DD4hep_ME0NumberingScheme.h"

class MuonBaseNumber;
class MuonSubDetector;
class MuonConstants;

namespace cms {
  class MuonSimHitNumberingScheme : public MuonNumberingScheme {
  public:

    MuonSimHitNumberingScheme(MuonSubDetector*, const MuonConstants& muonConstants);
    //   ~MuonSimHitNumberingScheme() override;
    
    int CSCbaseNumberToUnitNumber(const MuonBaseNumber&); // override;
    int RPCbaseNumberToUnitNumber(const MuonBaseNumber&); // override;
    int GEMbaseNumberToUnitNumber(const MuonBaseNumber&); // override;
    int DTbaseNumberToUnitNumber(const MuonBaseNumber&); // override;
    //int ME0baseNumberToUnitNumber(const MuonBaseNumber&); // override;
    
  private:
    MuonSubDetector* theDetector;
    CSCNumberingScheme* theNumberingCSC;
    DTNumberingScheme* theNumberingDT;
    RPCNumberingScheme* theNumberingRPC;
    GEMNumberingScheme* theNumberingGEM;
    //GEMNumberingScheme* theNumberingME0;
  };
}  // namespace cms
#endif
