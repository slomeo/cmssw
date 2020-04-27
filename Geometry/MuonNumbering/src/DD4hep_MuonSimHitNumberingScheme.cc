#include "Geometry/MuonNumbering/interface/DD4hep_MuonSimHitNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_DTNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_CSCNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_RPCNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_GEMNumberingScheme.h"
//#include "Geometry/MuonNumbering/interface/DD4hep_ME0NumberingScheme.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"
#include "Geometry/MuonNumbering/interface/MuonSubDetector.h"
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"

using namespace cms;

MuonSimHitNumberingScheme::MuonSimHitNumberingScheme(MuonSubDetector* d, const MuonConstants& muonConstants) {
  theDetector = d;
  if (theDetector->isEndcap()) {
    theNumberingCSC = new CSCNumberingScheme(muonConstants);
  } else if (theDetector->isBarrel()) {
    theNumberingDT = new DTNumberingScheme(muonConstants);
  } else if (theDetector->isRPC()) {
    theNumberingRPC = new RPCNumberingScheme(muonConstants);
  } else if (theDetector->isGEM()) {
    theNumberingGEM = new GEMNumberingScheme(muonConstants);
  }  //else if (theDetector->isME0()) {
     // theNumbering = new ME0NumberingScheme(muonConstants);
     // }
}

//MuonSimHitNumberingScheme::~MuonSimHitNumberingScheme() { delete theNumberingCSC; }

int MuonSimHitNumberingScheme::CSCbaseNumberToUnitNumber(const MuonBaseNumber& num) {
  if (theNumberingCSC) {
    theNumberingCSC->baseNumberToUnitNumber(num);
    return theNumberingCSC->getDetId();
  } else {
    return 0;
  }
}

int MuonSimHitNumberingScheme::RPCbaseNumberToUnitNumber(const MuonBaseNumber& num) {
  if (theNumberingRPC) {
    theNumberingRPC->baseNumberToUnitNumber(num);
    return theNumberingRPC->getDetId();
  } else {
    return 0;
  }
}

int MuonSimHitNumberingScheme::GEMbaseNumberToUnitNumber(const MuonBaseNumber& num) {
  if (theNumberingGEM) {
    theNumberingGEM->baseNumberToUnitNumber(num);
    return theNumberingGEM->getDetId();
  } else {
    return 0;
  }
}

int MuonSimHitNumberingScheme::DTbaseNumberToUnitNumber(const MuonBaseNumber& num) {
  if (theNumberingDT) {
    return theNumberingDT->getDetId(num);
  } else {
    return 0;
  }
}

/*
int MuonSimHitNumberingScheme::GEMbaseNumberToUnitNumber(const MuonBaseNumber& num) {
  if (theNumberingME0) {
    theNumberingME0->baseNumberToUnitNumber(num);
    return theNumberingME0->getDetId();
  } else {
    return 0;
  }
}
*/
