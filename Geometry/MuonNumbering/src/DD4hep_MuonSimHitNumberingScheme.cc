#include "Geometry/MuonNumbering/interface/DD4hep_MuonSimHitNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_DTNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_CSCNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_RPCNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_GEMNumberingScheme.h"
//#include "Geometry/MuonNumbering/interface/ME0NumberingScheme.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"
#include "Geometry/MuonNumbering/interface/MuonSubDetector.h"
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"

using namespace cms;

MuonSimHitNumberingScheme::MuonSimHitNumberingScheme(MuonSubDetector* d, const MuonConstants& muonConstants) {
  theDetector = d;
  if (theDetector->isEndcap()) {
    theNumberingCSC = new CSCNumberingScheme(muonConstants);
  } //else if (theDetector->isBarrel()) {
  // theNumbering = new DTNumberingScheme(muonConstants);
  // } else if (theDetector->isRPC()) {
  //  theNumbering = new RPCNumberingScheme(muonConstants);
  // } else if (theDetector->isGEM()) {
  //  theNumbering = new GEMNumberingScheme(muonConstants);
  // } //else if (theDetector->isME0()) {
    // theNumbering = new ME0NumberingScheme(muonConstants);
    // }
}

//MuonSimHitNumberingScheme::~MuonSimHitNumberingScheme() { delete theNumberingCSC; }

int MuonSimHitNumberingScheme::baseNumberToUnitNumber(const MuonBaseNumber& num) {
  if (theNumberingCSC) {
    theNumberingCSC->baseNumberToUnitNumber(num);
    return theNumberingCSC->getDetId();
  } else {
    return 0;
  }
}
