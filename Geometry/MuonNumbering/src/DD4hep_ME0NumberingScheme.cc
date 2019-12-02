/*
//\class ME0NumberingScheme

Description: ME0 Numbering Scheme for DD4hep

//
// Author:  Sergio Lo Meo (sergio.lo.meo@cern.ch) following what Ianna Osburne made for DTs (DD4HEP migration)
//          Created:  02 Dec 2019 
*/
#include "Geometry/MuonNumbering/interface/DD4hep_ME0NumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"
#include "DataFormats/MuonDetId/interface/ME0DetId.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"
#include <cassert>

// to debug
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <math.h>

using namespace cms;
using namespace std;// to debug

ME0NumberingScheme::ME0NumberingScheme(const MuonConstants& muonConstants) { initMe(muonConstants); }

void ME0NumberingScheme::initMe(const MuonConstants& muonConstants) {

 int theLevelPart = get("level", muonConstants);

  assert(theLevelPart != 0);

  theRegionLevel = get("m0_region", muonConstants) / theLevelPart;
  theLayerLevel = get("m0_layer", muonConstants) / theLevelPart;
  theSectorLevel = get("m0_sector", muonConstants) / theLevelPart;
  theRollLevel = get("m0_roll", muonConstants) / theLevelPart;
  theNEtaPart = get("m0_nroll", muonConstants);

 cout << "MYDEBUG, ME0NumberingScheme: "
       << " \ntheRegionLevel " << theRegionLevel << " \ntheLayerLevel "
       << theLayerLevel << " \ntheSectorLevel "
      << theSectorLevel << " \ntheRollLevel " << theRollLevel<< " \ntheNEtaPart "<<theNEtaPart<<endl;
}

void ME0NumberingScheme::baseNumberToUnitNumber(const MuonBaseNumber& num) {
 
  for (int level = 1; level <= num.getLevels(); level++) {
 
    cout<<"MYDEBUG, ME0NumberingScheme: "<< "level " << level << " " << num.getSuperNo(level) << " " << num.getBaseNo(level);
  }

  int maxLevel = theRollLevel;
  if (num.getLevels() != maxLevel) {
    cout<<"MYDEBUG, ME0NumberingScheme: " << "MuonME0NS::BNToUN "
                                          << "BaseNumber has " << num.getLevels() << " levels,"
                                          << "need " << maxLevel <<endl;
  }

  int region = 0;
  int layer = 0;
  int chamber = 0;
  int  roll = 0;

  //decode significant ME0 levels

  if (num.getBaseNo(theRegionLevel) == 0)
    region = 1;
  else
    region = -1;
  layer = num.getBaseNo(theLayerLevel) + 1;
  chamber = num.getBaseNo(theSectorLevel) + 1;
  roll = num.getBaseNo(theRollLevel) + 1;

  // collect all info

  cout << "MYDEBUG, ME0NumberingScheme: Region " << region << " Layer " << layer << " Chamber "
       << chamber << " Roll " << roll<<endl;

  // Build the actual numbering
  ME0DetId id(region, layer, chamber, roll);

  cout<<"MYDEBUG, ME0NumberingScheme " << id.rawId() << " DetId " << id<<endl;
 
  setDetId(id.rawId());
}

const int ME0NumberingScheme::get(const char* key, const MuonConstants& muonConstants) const {
  int result(0);
  auto const& it = (muonConstants.find(key));
  if (it != end(muonConstants))
    result = it->second;
  return result;
}
