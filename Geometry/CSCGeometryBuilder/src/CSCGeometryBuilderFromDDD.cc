/*
// \class CSCGeometryBuilderFromDDD
//
//  Description: CSC Geometry Builder for DD4hep
//              
//
// \author Sergio Lo Meo (sergio.lo.meo@cern.ch) following what Ianna Osburne made for DTs (DD4HEP migration)
//         Created:  Fri, 27 Sep 2019 
//   
//         Old DD version author: Tim Cox
*/
// 
#include "CSCGeometryBuilderFromDDD.h"
#include "CSCGeometryBuilder.h"
#include "CSCGeometryParsFromDD.h"

#include <CondFormats/GeometryObjects/interface/RecoIdealGeometry.h>
#include <CondFormats/GeometryObjects/interface/CSCRecoDigiParameters.h>

#include <DetectorDescription/DDCMS/interface/DDFilteredView.h>
#include <DetectorDescription/DDCMS/interface/DDCompactView.h>
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"

#include <FWCore/Utilities/interface/Exception.h>

#include <utility>

// to debug
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <math.h>

using namespace std;// to debug

CSCGeometryBuilderFromDDD::CSCGeometryBuilderFromDDD() : myName("CSCGeometryBuilderFromDDD") {}

CSCGeometryBuilderFromDDD::~CSCGeometryBuilderFromDDD() {}
// DDD
void CSCGeometryBuilderFromDDD::build(CSCGeometry& geom,
                                      const DDCompactView* cview,
                                      const MuonDDDConstants& muonConstants) {
  RecoIdealGeometry rig;
  CSCRecoDigiParameters rdp;

  // simple class just really a method to get the parameters... but I want this method
  // available to classes other than CSCGeometryBuilderFromDDD so... simple class...
  CSCGeometryParsFromDD cscp;
  if (!cscp.build(cview, muonConstants, rig, rdp)) {
    throw cms::Exception("CSCGeometryBuilderFromDDD", "Failed to build the necessary objects from the DDD");
    cout<<"MYDEBUG, CSCGeometryBuilderFromDDD "<< "Failed to build the necessary objects from the DDD" <<endl;
  }
  CSCGeometryBuilder realbuilder;
  realbuilder.build(geom, rig, rdp);
}

// for DD4hep

void CSCGeometryBuilderFromDDD::build(CSCGeometry& geom,
				      const cms::DDCompactView* cview,
				      const cms::MuonNumbering& muonConstants)  {

  RecoIdealGeometry rig;
  CSCRecoDigiParameters rdp;
  
   CSCGeometryParsFromDD cscp;
  if (!cscp.build(cview, muonConstants, rig, rdp)) {
    throw cms::Exception("CSCGeometryBuilderFromDDD", "Failed to build the necessary objects from the DD4HEP");
    cout<<"MYDEBUG, CSCGeometryBuilderFromDDD "<< "Failed to build the necessary objects from the DD4HEP" <<endl;
  }
 
  CSCGeometryBuilder realbuilder;
  realbuilder.build(geom, rig, rdp);
 
}
