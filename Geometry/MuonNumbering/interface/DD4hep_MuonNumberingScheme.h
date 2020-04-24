#ifndef GEOMETRY_MUONNUMBERING_MUONNUMBERINGSCHEME_H
#define GEOMETRY_MUONNUMBERING_MUONNUMBERINGSCHEME_H

/** \class MuonNumberingScheme for DD4HEP
 *
 * Author: Sergio Lo Meo (sergio.lo.meo@cern.ch)
 * 
 * Date: 24 April 2020
 *  
 * Original author for DD version: Arno Straessner, CERN <arno.straessner@cern.ch>
 *
 */

#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"

class MuonBaseNumber;
class MuonConstants;

namespace cms {
  class MuonNumberingScheme {
  public:
    MuonNumberingScheme(){};
    virtual ~MuonNumberingScheme(){};
    
    virtual int baseNumberToUnitNumber(const MuonBaseNumber&);
    
  private:
  };
}  // namespace cms
#endif
