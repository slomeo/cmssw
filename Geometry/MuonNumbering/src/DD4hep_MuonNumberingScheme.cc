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
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"

using namespace cms;

int MuonNumberingScheme::baseNumberToUnitNumber(const MuonBaseNumber&) { return 0; }
