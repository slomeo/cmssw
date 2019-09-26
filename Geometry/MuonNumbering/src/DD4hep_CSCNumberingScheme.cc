/*
// \class CSCNumberingScheme
//
//  Description: CSC Numbering Scheme for DD4hep
//              
//
// \author Sergio Lo Meo (sergio.lo.meo@cern.ch) following what Ianna Osburne made for DTs (DD4HEP migration)
//         Created:  Thu, 26 Sep 2019 
//   
//         Old DD version authors:  Arno Straessner & Tim Cox
*/
// 
#include "DataFormats/MuonDetId/interface/CSCDetId.h"
#include "Geometry/MuonNumbering/interface/DD4hep_CSCNumberingScheme.h"
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"
#include <cassert>

// to debug
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <math.h>

using namespace cms;
using namespace std;// to debug

CSCNumberingScheme::CSCNumberingScheme(const MuonConstants& muonConstants) { initMe(muonConstants); }

void CSCNumberingScheme::initMe(const MuonConstants& muonConstants) {
int theLevelPart = get("level", muonConstants); 
assert(theLevelPart != 0);
theRegionLevel = get("me_region", muonConstants) / theLevelPart;
theStationLevel = get("me_station", muonConstants) / theLevelPart;
theSubringLevel = get("me_subring", muonConstants) / theLevelPart;
theSectorLevel = get("me_sector", muonConstants) / theLevelPart;
theRingLevel = get("me_ring", muonConstants) / theLevelPart;
theLayerLevel = get("me_layer", muonConstants) / theLevelPart;
cout<<"MYDEBUG, CSCNumbering "<< "theLevelPart= " << theLevelPart<<endl;
cout<<"MYDEBUG, CSCNumbering "<< "theRegionLevel= " << theRegionLevel<<endl;
cout<<"MYDEBUG, CSCNumbering "<< "theStationLevel= " << theStationLevel<<endl;
cout<<"MYDEBUG, CSCNumbering "<< "theSubringLevel= " << theSubringLevel<<endl;
cout<<"MYDEBUG, CSCNumbering "<< "theSectorLevel= " << theSectorLevel<<endl;
cout<<"MYDEBUG, CSCNumbering "<< "theRingLevel= " << theRingLevel<<endl;
cout<<"MYDEBUG, CSCNumbering "<< "theLayerLevel= " << theLayerLevel<<endl;
}

void CSCNumberingScheme::baseNumberToUnitNumber(const MuonBaseNumber& num) {

  cout<<"MYDEBUG, CSCNumbering " << "num.getLevels() " << num.getLevels()<<endl;
  for (int level = 1; level <= num.getLevels(); level++) {
    cout<<"MYDEBUG, CSCNumbering level, num.getSuperNo, num.getBaseNo" << level << " " << num.getSuperNo(level) << " " << num.getBaseNo(level)<<endl;
  }

  int fwbw_id = 0;
  int station_id = 0;
  int ring_id = 0;
  int subring_id = 0;
  int sector_id = 0;
  int layer_id = 0;

  // Decode endcap levels
  // We should be able to work with 6 (layer-level) or 5 (chamber-level)

  for (int level = 1; level <= num.getLevels(); level++) {
    if (level == theRegionLevel) {
      const int copyno = num.getBaseNo(level);
      fwbw_id = copyno + 1;
      cout<<"MYDEBUG, CSCNumbering "<< "endcap= " << fwbw_id<<endl;

    } else if (level == theStationLevel) {
      const int station_tag = num.getSuperNo(level);
      station_id = station_tag;
      cout<<"MYDEBUG, CSCNumbering " << "station= " << station_id<<endl;

    } else if (level == theSubringLevel) {
      const int copyno = num.getBaseNo(level);
      subring_id = copyno + 1;
      cout<<"MYDEBUG, CSCNumbering " << "subring= " << subring_id<<endl;
 
    } else if (level == theSectorLevel) {
      const int copyno = num.getBaseNo(level);
      sector_id = copyno + 1;
      cout<<"MYDEBUG, CSCNumbering " << "sector= " << sector_id<<endl;

    } else if (level == theLayerLevel) {
      const int copyno = num.getBaseNo(level);
      layer_id = copyno + 1;
      cout<<"MYDEBUG, CSCNumbering " << "layer= " << layer_id<<endl;

    } else if (level == theRingLevel) {
      const int ring_tag = num.getSuperNo(level);
      ring_id = ring_tag;
      cout<<"MYDEBUG, CSCNumbering" << "ring= " << ring_id<<endl;
    }
  }

  // check validity

  if ((fwbw_id < 1) || (fwbw_id > 2)) {
    cout<<"MYDEBUG, CSCNumbering " << "@SUB=CSCNumberingScheme::baseNumberToUnitNumber"
	<< "forward/backward id out of range: " << fwbw_id<<endl;
  }
  
  if ((station_id < 1) || (station_id > 4)) {
    cout<<"MYDEBUG, CSCNumbering " << "@SUB=CSCNumberingScheme::baseNumberToUnitNumber"
	<< "station id out of range: " << station_id<<endl;
  }
  
  if ((ring_id < 1) || (ring_id > 4)) {
    cout<<"MYDEBUG, CSCNumbering " << "@SUB=CSCNumberingScheme::baseNumberToUnitNumber"
	<< "ring id out of range: " << ring_id<<endl;
  }
  
  if ((subring_id < 1) || (subring_id > 2)) {
    cout<<"MYDEBUG, CSCNumbering " << "@SUB=CSCNumberingScheme::baseNumberToUnitNumber"
	<< "subring id out of range: " << subring_id<<endl;
  }
  
  if ((sector_id < 1) || (sector_id > 36)) {
    cout<<"MYDEBUG, CSCNumbering " << "@SUB=CSCNumberingScheme::baseNumberToUnitNumber"
	<< "sector id out of range:" << sector_id<<endl;
  }
  
  // Allow id=0 since that means a chamber
  if ((layer_id < 0) || (layer_id > 6)) {
    cout<<"MYDEBUG, CSCNumbering " << "@SUB=CSCNumberingScheme::baseNumberToUnitNumber"
	<< "layer id out of range " << layer_id<<endl;
  }
  
  // find appropriate chamber label

  int chamber_id = chamberIndex(station_id, ring_id, subring_id, sector_id);
  
  // convert into raw id of appropriate DetId
  
  int intIndex = CSCDetId::rawIdMaker(fwbw_id, station_id, ring_id, chamber_id, layer_id);
  
  
cout<<"MYDEBUG, CSCNumbering " << "CSCNumberingScheme :  fw/bw " << fwbw_id << " station " << station_id << " ring "
    << ring_id << " subring " << subring_id << " chamber " << chamber_id << " sector "
    << sector_id << " layer " << layer_id<<endl;
 
 setDetId(intIndex);
 
}

int CSCNumberingScheme::chamberIndex(int station_id, int ring_id, int subring_id, int sector_id) const {
  int chamber_id = 0;

  // chamber label is related to sector_id but we need to
  // adjust to real hardware labelling
  // Tim confirms this works properly according to CMS IN 2000/004 Version 2.5 March 2007.

  if (ring_id == 3) {
    chamber_id = sector_id;
  } else {
    if (subring_id == 1) {
      chamber_id = 2 * sector_id - 1;
    } else {
      chamber_id = 2 * sector_id;
    }
  }

  return chamber_id;
}

const int CSCNumberingScheme::get(const char* key, const MuonConstants& muonConstants) const {
  int result(0);
  auto const& it = (muonConstants.find(key));
  if (it != end(muonConstants))
    result = it->second;
  return result;
}
