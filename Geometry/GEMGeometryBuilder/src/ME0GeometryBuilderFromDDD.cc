/*
//\class ME0GeometryBuilder

 Description: ME0 Geometry builder from DD & DD4hep
              DD4hep part added to the original old file (DD version) made by M. Maggi (INFN Bari)
//
// Author:  Sergio Lo Meo (sergio.lo.meo@cern.ch) following what Ianna Osburne made for DTs (DD4HEP migration)
//          Created:  06 Dec 2019 
*/
#include "Geometry/GEMGeometryBuilder/src/ME0GeometryBuilderFromDDD.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "Geometry/GEMGeometry/interface/ME0EtaPartitionSpecs.h"

#include "DetectorDescription/Core/interface/DDFilter.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/Core/interface/DDSolid.h"

#include "Geometry/MuonNumbering/interface/MuonDDDNumbering.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"
#include "Geometry/MuonNumbering/interface/ME0NumberingScheme.h"

#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include "DataFormats/GeometryVector/interface/Basic3DVector.h"

#include "CLHEP/Units/GlobalSystemOfUnits.h"

#include <algorithm>
#include <iostream>
#include <string>

#include <DetectorDescription/DDCMS/interface/DDFilteredView.h>
#include <DetectorDescription/DDCMS/interface/DDCompactView.h>
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"
#include "DetectorDescription/DDCMS/interface/DDSpecParRegistry.h"
#include "Geometry/MuonNumbering/interface/DD4hep_ME0NumberingScheme.h"

// to debug
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

using namespace std;// to debug

ME0GeometryBuilderFromDDD::ME0GeometryBuilderFromDDD() {}

ME0GeometryBuilderFromDDD::~ME0GeometryBuilderFromDDD() {}

ME0Geometry* ME0GeometryBuilderFromDDD::build(const DDCompactView* cview, const MuonDDDConstants& muonConstants) {
  std::string attribute = "MuStructure";
  std::string value = "MuonEndCapME0";
  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside build"<<endl;
  // Asking only for the MuonME0's
  DDSpecificsMatchesValueFilter filter{DDValue(attribute, value, 0.0)};
  DDFilteredView fview(*cview, filter);

  return this->buildGeometry(fview, muonConstants);
}


// for DD4hep
ME0Geometry* ME0GeometryBuilderFromDDD::build(const cms::DDCompactView* cview,
                                              const cms::MuonNumbering& muonConstants) {
  std::string attribute = "MuStructure";
  std::string value = "MuonEndCapME0";
  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside build"<<endl;
  cms::DDFilteredView fview(cview->detector(), cview->detector()->worldVolume());
  cms::DDSpecParRefs refs;
  const cms::DDSpecParRegistry& mypar = cview->specpars();
  mypar.filter(refs, attribute, value);
  fview.mergedSpecifics(refs);
  return this->buildGeometry(fview, muonConstants);
}

ME0Geometry* ME0GeometryBuilderFromDDD::buildGeometry(DDFilteredView& fv, const MuonDDDConstants& muonConstants) {

  static int countLayer = 0;   
  static int countChamber = 0;
  static int countEta = 0;      

  ME0Geometry* geometry = new ME0Geometry();
  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside buildGeometry " << "Top level logical part: " << fv.logicalPart().name().name()<<endl;
  LogTrace("ME0GeometryBuilderFromDDD") << "Building the geometry service";
  LogTrace("ME0GeometryBuilderFromDDD") << "About to run through the ME0 structure\n"
                                        << "Top level logical part: " << fv.logicalPart().name().name();

// ==========================================
// ===  Test to understand the structure  ===
// ==========================================
#ifdef EDM_ML_DEBUG
  bool testChambers = fv.firstChild();
  LogTrace("ME0GeometryBuilderFromDDD") << "doChamber = fv.firstChild() = " << testChambers;
  // ----------------------------------------------------------------------------------------------------------------------------------------------
  while (testChambers) {
    // to etapartitions
    LogTrace("ME0GeometryBuilderFromDDD")
        << "to layer " << fv.firstChild();  // commented out in case only looping over sensitive volumes
    LogTrace("ME0GeometryBuilderFromDDD")
        << "to etapt " << fv.firstChild();  // commented out in case only looping over sensitive volumes
    MuonDDDNumbering mdddnum(muonConstants);
    ME0NumberingScheme me0Num(muonConstants);
    int rawId = me0Num.baseNumberToUnitNumber(mdddnum.geoHistoryToBaseNumber(fv.geoHistory()));
    ME0DetId detId = ME0DetId(rawId);
    ME0DetId detIdCh = detId.chamberId();
    // back to chambers
    LogTrace("ME0GeometryBuilderFromDDD")
        << "back to layer " << fv.parent();  // commented out in case only looping over sensitive volumes
    LogTrace("ME0GeometryBuilderFromDDD")
        << "back to chamb " << fv.parent();  // commented out in case only looping over sensitive volumes
    // ok lets get started ...
    LogTrace("ME0GeometryBuilderFromDDD")
        << "In DoChambers Loop :: ME0DetId " << detId << " = " << detId.rawId() << " (which belongs to ME0Chamber "
        << detIdCh << " = " << detIdCh.rawId() << ")";
    LogTrace("ME0GeometryBuilderFromDDD") << "Second level logical part: " << fv.logicalPart().name().name();
    DDBooleanSolid solid2 = (DDBooleanSolid)(fv.logicalPart().solid());
    std::vector<double> dpar2 = solid2.parameters();
    std::stringstream parameters2;
    for (unsigned int i = 0; i < dpar2.size(); ++i) {
      parameters2 << " dpar[" << i << "]=" << dpar2[i] / 10 << "cm ";
    }
    LogTrace("ME0GeometryBuilderFromDDD")
        << "Second level parameters: vector with size = " << dpar2.size() << " and elements " << parameters2.str();

    bool doLayers = fv.firstChild();
    // --------------------------------------------------------------------------------------------------------------------------------------------
    LogTrace("ME0GeometryBuilderFromDDD") << "doLayer = fv.firstChild() = " << doLayers;
    while (doLayers) {
      // to etapartitions
      LogTrace("ME0GeometryBuilderFromDDD")
          << "to etapt " << fv.firstChild();  // commented out in case only looping over sensitive volumes
      MuonDDDNumbering mdddnum(muonConstants);
      ME0NumberingScheme me0Num(muonConstants);
      int rawId = me0Num.baseNumberToUnitNumber(mdddnum.geoHistoryToBaseNumber(fv.geoHistory()));
      ME0DetId detId = ME0DetId(rawId);
      ME0DetId detIdLa = detId.layerId();
      // back to layers
      LogTrace("ME0GeometryBuilderFromDDD")
          << "back to layer " << fv.parent();  // commented out in case only looping over sensitive volumes
      LogTrace("ME0GeometryBuilderFromDDD")
          << "In DoLayers Loop :: ME0DetId " << detId << " = " << detId.rawId() << " (which belongs to ME0Layer "
          << detIdLa << " = " << detIdLa.rawId() << ")";
      LogTrace("ME0GeometryBuilderFromDDD") << "Third level logical part: " << fv.logicalPart().name().name();
      DDBooleanSolid solid3 = (DDBooleanSolid)(fv.logicalPart().solid());
      std::vector<double> dpar3 = solid3.parameters();
      std::stringstream parameters3;
      for (unsigned int i = 0; i < dpar3.size(); ++i) {
        parameters3 << " dpar[" << i << "]=" << dpar3[i] / 10 << "cm ";
      }
      LogTrace("ME0GeometryBuilderFromDDD")
          << "Third level parameters: vector with size = " << dpar3.size() << " and elements " << parameters3.str();
      bool doEtaParts = fv.firstChild();
      // --------------------------------------------------------------------------------------------------------------------------------------------
      LogTrace("ME0GeometryBuilderFromDDD") << "doEtaPart = fv.firstChild() = " << doEtaParts;
      while (doEtaParts) {
        LogTrace("ME0GeometryBuilderFromDDD") << "In DoEtaParts Loop :: ME0DetId " << detId << " = " << detId.rawId();
        LogTrace("ME0GeometryBuilderFromDDD") << "Fourth level logical part: " << fv.logicalPart().name().name();
        DDBooleanSolid solid4 = (DDBooleanSolid)(fv.logicalPart().solid());
        std::vector<double> dpar4 = solid4.parameters();
        std::stringstream parameters4;
        for (unsigned int i = 0; i < dpar4.size(); ++i) {
          parameters4 << " dpar[" << i << "]=" << dpar4[i] / 10 << "cm ";
        }
        LogTrace("ME0GeometryBuilderFromDDD")
            << "Fourth level parameters: vector with size = " << dpar4.size() << " and elements " << parameters4.str();
        // --------------------------------------------------------------------------------------------------------------------------------------------
        doEtaParts = fv.nextSibling();
        LogTrace("ME0GeometryBuilderFromDDD") << "doEtaPart = fv.nextSibling() = " << doEtaParts;
      }
      fv.parent();  // commented out in case only looping over sensitive volumes
      LogTrace("ME0GeometryBuilderFromDDD")
          << "went back to parent :: name = " << fv.logicalPart().name().name() << " will now ask for nextSibling";
      doLayers = fv.nextSibling();
      LogTrace("ME0GeometryBuilderFromDDD") << "doLayer = fv.nextSibling() = " << doLayers;
    }
    fv.parent();  // commented out in case only looping over sensitive volumes
    LogTrace("ME0GeometryBuilderFromDDD")
        << "went back to parent :: name = " << fv.logicalPart().name().name() << " will now ask for nextSibling";
    testChambers = fv.nextSibling();
    LogTrace("ME0GeometryBuilderFromDDD") << "doChamber = fv.nextSibling() = " << testChambers;
  }
  fv.parent();
#endif

  // ==========================================
  // === Here the Real ME0 Geometry Builder ===
  // ==========================================
  bool doChambers = fv.firstChild();

  

  //  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD after bool doChambers = fv.firstChild(); bool value:  "<<doChambers<<endl;
  while (doChambers) {

    cout<<" MYDEBUG, number 1: ME0GeometryBuilder inside while(doChambers), name volume: "<<fv.logicalPart().name().name()<<endl;
    //    cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside while(doChambers)"<<endl;
    // to etapartitions and back again to pick up DetId
    fv.firstChild();
    cout<<" MYDEBUG, number 2: ME0GeometryBuilder, inside while(doChambers), after first fv.firstChild() name volume: "<<fv.logicalPart().name().name()<<endl;
    fv.firstChild();
    cout<<" MYDEBUG, number 2 bis: ME0GeometryBuilder, inside while(doChambers), after second fv.firstChild() name volume: "<<fv.logicalPart().name().name()<<endl;

    MuonDDDNumbering mdddnum(muonConstants);
    ME0NumberingScheme me0Num(muonConstants);
    int rawId = me0Num.baseNumberToUnitNumber(mdddnum.geoHistoryToBaseNumber(fv.geoHistory()));
    ME0DetId detId = ME0DetId(rawId);
    ME0DetId detIdCh = detId.chamberId();

    fv.parent();
    cout<<" MYDEBUG, number 3: ME0GeometryBuilder, inside while(doChambers), after first fv.parent() name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
    fv.parent();
    cout<<" MYDEBUG, number 4: ME0GeometryBuilder, inside while(doChambers), after seconf fv.parent() name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;

    // build chamber
    ME0Chamber* me0Chamber = buildChamber(fv, detIdCh);
    geometry->add(me0Chamber);

    // loop over layers of the chamber
    bool doLayers = fv.firstChild();
   
    while (doLayers) {
      cout<<" MYDEBUG, number 5: ME0GeometryBuilder, inside while(doLayers) name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
      //cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside while(doLayers)"<<endl;
      // to etapartitions and back again to pick up DetId
      fv.firstChild();
      cout<<" MYDEBUG, number 6: ME0GeometryBuilder, inside while(doLayers) after fv.firstChild() name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
      MuonDDDNumbering mdddnum(muonConstants);
      ME0NumberingScheme me0Num(muonConstants);
      int rawId = me0Num.baseNumberToUnitNumber(mdddnum.geoHistoryToBaseNumber(fv.geoHistory()));
      ME0DetId detId = ME0DetId(rawId);
      ME0DetId detIdLa = detId.layerId();
      fv.parent();
      cout<<" MYDEBUG, number 7: ME0GeometryBuilder, inside while(doLayers) after first fv.parent() name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
      // build layer
      ME0Layer* me0Layer = buildLayer(fv, detIdLa);
      me0Chamber->add(me0Layer);
      geometry->add(me0Layer);

      // loop over etapartitions of the layer
      bool doEtaParts = fv.firstChild();
      
      while (doEtaParts) {
	cout<<" MYDEBUG, number 8: ME0GeometryBuilder, inside while (doEtaParts) name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
	//cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside while(doEtaParts)"<<endl;
        // pick up DetId
        MuonDDDNumbering mdddnum(muonConstants);
        ME0NumberingScheme me0Num(muonConstants);
        int rawId = me0Num.baseNumberToUnitNumber(mdddnum.geoHistoryToBaseNumber(fv.geoHistory()));
        ME0DetId detId = ME0DetId(rawId);

        // build etapartition
        ME0EtaPartition* etaPart = buildEtaPartition(fv, detId);
        me0Layer->add(etaPart);
        geometry->add(etaPart);

        doEtaParts = fv.nextSibling();
	cout<<" MYDEBUG, number 9: ME0GeometryBuilder, inside while (doEtaParts) after fv.nextSibling() name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
	countEta = countEta + 1;
      }
      fv.parent();
      cout<<" MYDEBUG, number 10: ME0GeometryBuilder, inside while (doLayers) after fv.parent() name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
      doLayers = fv.nextSibling();
      cout<<" MYDEBUG, number 11: ME0GeometryBuilder, inside while (doLayers) after fv.nextSibling() name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
      countLayer = countLayer + 1;   
    }
    fv.parent();
    cout<<" MYDEBUG, number 12: ME0GeometryBuilder, inside while (doChamber) after fv.parent() name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
    doChambers = fv.nextSibling();
    cout<<" MYDEBUG, number 13: ME0GeometryBuilder, inside while (doChamber) after fv.nextSibling() name volume: "<<fv.logicalPart().name().name()<<" detId: "<<rawId<<endl;
    countChamber = countChamber + 1;
  }

  cout<<"-------------------------------------------------------------------------------------------------------"<<endl; 
  cout<<"COUNTS, Chamber: "<<countChamber<<" Layers: "<<countLayer<<" Eta: "<<countEta<<endl;
  cout<<"-------------------------------------------------------------------------------------------------------"<<endl; 

  return geometry;

}

ME0Chamber* ME0GeometryBuilderFromDDD::buildChamber(DDFilteredView& fv, ME0DetId detId) const {
  LogTrace("ME0GeometryBuilderFromDDD") << "buildChamber " << fv.logicalPart().name().name() << " " << detId
                                        << std::endl;
  //  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD buildChambers"<<endl;
  DDBooleanSolid solid = (DDBooleanSolid)(fv.logicalPart().solid());
  // std::vector<double> dpar = solid.solidA().parameters();
  std::vector<double> dpar = solid.parameters();
  double L = dpar[0] / cm;  // length is along local Y
  double T = dpar[3] / cm;  // thickness is long local Z
  double b = dpar[4] / cm;  // bottom width is along local X
  double B = dpar[8] / cm;  // top width is along local X
  // hardcoded :: double b = 21.9859, B = 52.7261, L = 87.1678, T = 12.9;

cout<<"MYDEBUG, ME0GeometryBuilderFromDDD, buildChamber, dpar: 1,2,3,4: "<<dpar[1]<<" "<<dpar[2]<<" "<<dpar[3]<<" "<<dpar[4]<<endl;
cout<<"MYDEBUG, ME0GeometryBuilderFromDDD, buildChamber, dpar 5,6,7,8,0: "<<dpar[5]<<" "<<dpar[6]<<" "<<dpar[7]<<" "<<dpar[8]<<" "<<dpar[0]<<endl;

#ifdef EDM_ML_DEBUG
  LogTrace("ME0GeometryBuilderFromDDD") << " name of logical part = " << fv.logicalPart().name().name() << std::endl;
  LogTrace("ME0GeometryBuilderFromDDD") << " dpar is vector with size = " << dpar.size() << std::endl;
  for (unsigned int i = 0; i < dpar.size(); ++i) {
    LogTrace("ME0GeometryBuilderFromDDD") << " dpar [" << i << "] = " << dpar[i] / 10 << " cm " << std::endl;
  }
  LogTrace("ME0GeometryBuilderFromDDD") << "size  b: " << b << "cm, B: " << B << "cm,  L: " << L << "cm, T: " << T
                                        << "cm " << std::endl;
#endif

  bool isOdd = false;  // detId.chamber()%2;
  ME0BoundPlane surf(boundPlane(fv, new TrapezoidalPlaneBounds(b, B, L, T), isOdd));
  ME0Chamber* chamber = new ME0Chamber(detId.chamberId(), surf);
  return chamber;
}

ME0Layer* ME0GeometryBuilderFromDDD::buildLayer(DDFilteredView& fv, ME0DetId detId) const {
  LogTrace("ME0GeometryBuilderFromDDD") << "buildLayer " << fv.logicalPart().name().name() << " " << detId << std::endl;
  // cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside buildLayer"<<endl;
  DDBooleanSolid solid = (DDBooleanSolid)(fv.logicalPart().solid());
  // std::vector<double> dpar = solid.solidA().parameters();
  std::vector<double> dpar = solid.parameters();
  double L = dpar[0] / cm;  // length is along local Y
  double t = dpar[3] / cm;  // thickness is long local Z
  double b = dpar[4] / cm;  // bottom width is along local X
  double B = dpar[8] / cm;  // top width is along local X
  // dpar = solid.solidB().parameters();
  // dz += dpar[3]/cm;     // layer thickness --- to be checked !!! layer thickness should be same as eta part thickness
  // hardcoded :: double b = 21.9859, B = 52.7261, L = 87.1678, t = 0.4;

cout<<"MYDEBUG, ME0GeometryBuilderFromDDD, buildLayer, dpar: 1,2,3,4: "<<dpar[1]<<" "<<dpar[2]<<" "<<dpar[3]<<" "<<dpar[4]<<endl;
cout<<"MYDEBUG, ME0GeometryBuilderFromDDD, buildLayer, dpar 5,6,7,8,0: "<<dpar[5]<<" "<<dpar[6]<<" "<<dpar[7]<<" "<<dpar[8]<<" "<<dpar[0]<<endl;


#ifdef EDM_ML_DEBUG
  LogTrace("ME0GeometryBuilderFromDDD") << " name of logical part = " << fv.logicalPart().name().name() << std::endl;
  LogTrace("ME0GeometryBuilderFromDDD") << " dpar is vector with size = " << dpar.size() << std::endl;
  for (unsigned int i = 0; i < dpar.size(); ++i) {
    LogTrace("ME0GeometryBuilderFromDDD") << " dpar [" << i << "] = " << dpar[i] / 10 << " cm " << std::endl;
  }
  LogTrace("ME0GeometryBuilderFromDDD") << "size  b: " << b << "cm, B: " << B << "cm,  L: " << L << "cm, t: " << t
                                        << "cm " << std::endl;
#endif

  bool isOdd = false;  // detId.chamber()%2;
  ME0BoundPlane surf(boundPlane(fv, new TrapezoidalPlaneBounds(b, B, L, t), isOdd));
  ME0Layer* layer = new ME0Layer(detId.layerId(), surf);
  return layer;
}

ME0EtaPartition* ME0GeometryBuilderFromDDD::buildEtaPartition(DDFilteredView& fv, ME0DetId detId) const {
  LogTrace("ME0GeometryBuilderFromDDD") << "buildEtaPartition " << fv.logicalPart().name().name() << " " << detId
                                        << std::endl;
  // cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside buildEtaPartition"<<endl;
  // EtaPartition specific parameter (nstrips and npads)
  DDValue numbOfStrips("nStrips");
  DDValue numbOfPads("nPads");
  std::vector<const DDsvalues_type*> specs(fv.specifics());
  std::vector<const DDsvalues_type*>::iterator is = specs.begin();
  double nStrips = 0., nPads = 0.;
  for (; is != specs.end(); is++) {
    if (DDfetch(*is, numbOfStrips))
      nStrips = numbOfStrips.doubles()[0];
    if (DDfetch(*is, numbOfPads))
      nPads = numbOfPads.doubles()[0];
  }
  LogTrace("ME0GeometryBuilderFromDDD") << ((nStrips == 0.) ? ("No nStrips found!!")
                                                            : ("Number of strips: " + std::to_string(nStrips)));
  LogTrace("ME0GeometryBuilderFromDDD") << ((nPads == 0.) ? ("No nPads found!!")
                                                          : ("Number of pads: " + std::to_string(nPads)));

  // EtaPartition specific parameter (size)
  std::vector<double> dpar = fv.logicalPart().solid().parameters();
  double b = dpar[4] / cm;  // half bottom edge
  double B = dpar[8] / cm;  // half top edge
  double L = dpar[0] / cm;  // half apothem
  double t = dpar[3] / cm;  // half thickness

cout<<"MYDEBUG, ME0GeometryBuilderFromDDD, buildEta, dpar: 1,2,3,4: "<<dpar[1]<<" "<<dpar[2]<<" "<<dpar[3]<<" "<<dpar[4]<<endl;
cout<<"MYDEBUG, ME0GeometryBuilderFromDDD, buildEta, dpar 5,6,7,8,0: "<<dpar[5]<<" "<<dpar[6]<<" "<<dpar[7]<<" "<<dpar[8]<<" "<<dpar[0]<<endl;


#ifdef EDM_ML_DEBUG
  LogTrace("ME0GeometryBuilderFromDDD") << " name of logical part = " << fv.logicalPart().name().name() << std::endl;
  LogTrace("ME0GeometryBuilderFromDDD") << " dpar is vector with size = " << dpar.size() << std::endl;
  for (unsigned int i = 0; i < dpar.size(); ++i) {
    LogTrace("ME0GeometryBuilderFromDDD") << " dpar [" << i << "] = " << dpar[i] / 10 << " cm " << std::endl;
  }
  LogTrace("ME0GeometryBuilderFromDDD") << "size  b: " << b << "cm, B: " << B << "cm,  L: " << L << "cm, t: " << t
                                        << "cm " << std::endl;
#endif

  std::vector<float> pars;
  pars.emplace_back(b);
  pars.emplace_back(B);
  pars.emplace_back(L);
  pars.emplace_back(nStrips);
  pars.emplace_back(nPads);

  bool isOdd =
      false;  // detId.chamber()%2; // this gives the opportunity (in future) to change the face of the chamber (electronics facing IP or electronics away from IP)
  ME0BoundPlane surf(boundPlane(fv, new TrapezoidalPlaneBounds(b, B, L, t), isOdd));
  std::string name = fv.logicalPart().name().name();
  ME0EtaPartitionSpecs* e_p_specs = new ME0EtaPartitionSpecs(GeomDetEnumerators::ME0, name, pars);

  ME0EtaPartition* etaPartition = new ME0EtaPartition(detId, surf, e_p_specs);
  return etaPartition;
}

ME0GeometryBuilderFromDDD::ME0BoundPlane ME0GeometryBuilderFromDDD::boundPlane(const DDFilteredView& fv,
                                                                               Bounds* bounds,
                                                                               bool isOddChamber) const {
  // extract the position
  const DDTranslation& trans(fv.translation());
  const Surface::PositionType posResult(float(trans.x() / cm), float(trans.y() / cm), float(trans.z() / cm));

  // now the rotation
  //  DDRotationMatrix tmp = fv.rotation();
  // === DDD uses 'active' rotations - see CLHEP user guide ===
  //     ORCA uses 'passive' rotation.
  //     'active' and 'passive' rotations are inverse to each other
  //  DDRotationMatrix tmp = fv.rotation();
  const DDRotationMatrix& rotation = fv.rotation();  //REMOVED .Inverse();
  DD3Vector x, y, z;
  rotation.GetComponents(x, y, z);
  // LogTrace("GEMGeometryBuilderFromDDD") << "translation: "<< fv.translation() << std::endl;
  // LogTrace("GEMGeometryBuilderFromDDD") << "rotation   : "<< fv.rotation() << std::endl;
  // LogTrace("GEMGeometryBuilderFromDDD") << "INVERSE rotation manually: \n"
  //        << x.X() << ", " << x.Y() << ", " << x.Z() << std::endl
  //        << y.X() << ", " << y.Y() << ", " << y.Z() << std::endl
  //        << z.X() << ", " << z.Y() << ", " << z.Z() << std::endl;

  Surface::RotationType rotResult(float(x.X()),
                                  float(x.Y()),
                                  float(x.Z()),
                                  float(y.X()),
                                  float(y.Y()),
                                  float(y.Z()),
                                  float(z.X()),
                                  float(z.Y()),
                                  float(z.Z()));

  //Change of axes for the forward
  Basic3DVector<float> newX(1., 0., 0.);
  Basic3DVector<float> newY(0., 0., 1.);
  Basic3DVector<float> newZ(0., 1., 0.);
  newY *= -1;

  rotResult.rotateAxes(newX, newY, newZ);

  return ME0BoundPlane(new BoundPlane(posResult, rotResult, bounds));
}

// dd4hep

ME0Geometry* ME0GeometryBuilderFromDDD::buildGeometry(cms::DDFilteredView& fv, const cms::MuonNumbering& muonConstants) {
  ME0Geometry* geometry = new ME0Geometry();
  static int countLayer = 0;   
  static int countChamber = 0;
  static int countEta = 0;      
  static int countEtaBis = 0;      
  
  // -----------------------------------------  start chambers -----------------------------------------

  bool doChambers = fv.firstChild();
  while (doChambers) {

    cout<<" MYDEBUG, number 1: ME0GeometryBuilder inside while(doChambers), name volume: "<<fv.name()<<endl;
    cout<<" MYDEBUG, count Chamber "<<countChamber<<endl;
    /*
    // to etapartitions and back again to pick up DetId
    fv.down();
    cout<<" MYDEBUG, number 2: ME0GeometryBuilder, inside while(doChambers), after first fv.down() name volume: "<<fv.name()<<endl; 
    fv.down();
    cout<<" MYDEBUG, number 3: ME0GeometryBuilder, inside while(doChambers), after second fv.down() name volume: "<<fv.name()<<endl;
    */
    MuonBaseNumber mbn = muonConstants.geoHistoryToBaseNumber(fv.history());
    cms::ME0NumberingScheme me0Num(muonConstants.values());
    me0Num.baseNumberToUnitNumber(mbn);
    int id = me0Num.getDetId();
    ME0DetId detId = ME0DetId(me0Num.getDetId());
    ME0DetId detIdCh = detId.chamberId();

    // build chamber
    ME0Chamber* me0Chamber = buildChamber(fv, detIdCh);
    geometry->add(me0Chamber);
   
    //    fv.up();
    // fv.up();

    // -------------------------------------------- start layers -------------------------------------------------------------    
    // loop over layers of the chamber
    bool doLayers = fv.nextSibling();//era fv.nextSibling or fv.firstChild()
      cout<<" MYDEBUG, number 4: ME0GeometryBuilder, outside while(doLayers) after fv.nextSibling, name volume: "<<fv.name()<<" detId: "<<id<<endl;
     while (doLayers) {
       cout<<" MYDEBUG, count Layer "<<countLayer<<endl;
       MuonBaseNumber mbn = muonConstants.geoHistoryToBaseNumber(fv.history());
       cms::ME0NumberingScheme me0Num(muonConstants.values());
       me0Num.baseNumberToUnitNumber(mbn);
       int id = me0Num.getDetId();
       ME0DetId detId = ME0DetId(me0Num.getDetId());//era detIdCh
       ME0DetId detIdLa = detId.layerId();//ci vuole ???
       
       // build layer
       ME0Layer* me0Layer = buildLayer(fv, detIdLa);
       me0Chamber->add(me0Layer);
       geometry->add(me0Layer);
       
       fv.down();
       cout<<" MYDEBUG, number 5: ME0GeometryBuilder, inside while(doLayers) after fv.down() name volume: "<<fv.name()<<" detId: "<<id<<endl;
       //----- build first eta partition       
       MuonBaseNumber mbnbis = muonConstants.geoHistoryToBaseNumber(fv.history());
       cms::ME0NumberingScheme me0Numbis(muonConstants.values());
       me0Numbis.baseNumberToUnitNumber(mbnbis);
       int idbis = me0Numbis.getDetId();
       ME0DetId detIdbis = ME0DetId(me0Numbis.getDetId());//era detIdCh
       // build first etapartition
       ME0EtaPartition* etaPart = buildEtaPartition(fv, detIdbis);
       me0Layer->add(etaPart);
       geometry->add(etaPart);
       countEtaBis = countEtaBis + 1;
       //-----end build first eta partition ----------
       // ----------------------------------------------------- start eta partition ---------------------------------------
       bool doEtaParts = fv.sibling();// era sibling 
       cout<<" MYDEBUG, number 6: ME0GeometryBuilder, inside while(doLayers) after fv.sibling name volume: "<<fv.name()<<" detId: "<<id<<endl;
       cout<<" MYDEBUG, number 7: ME0GeometryBuilder, doEtaParts: "<<doEtaParts<<endl;     
       while (doEtaParts) {
	 cout<<" MYDEBUG, count Eta "<<countEta<<endl;
	 cout<<" MYDEBUG, number 8: ME0GeometryBuilder, inside while (doEtaParts) name volume: "<<fv.name()<<" detId: "<<id<<endl;
	 MuonBaseNumber mbn = muonConstants.geoHistoryToBaseNumber(fv.history());
	 cms::ME0NumberingScheme me0Num(muonConstants.values());
	 me0Num.baseNumberToUnitNumber(mbn);
	 int id = me0Num.getDetId();
	 ME0DetId detId = ME0DetId(me0Num.getDetId());//era detIdCh
	 // build other etapartition
	 ME0EtaPartition* etaPart = buildEtaPartition(fv, detId);
	 me0Layer->add(etaPart);
	 geometry->add(etaPart);
	 //
	 doEtaParts = fv.sibling();
	 cout<<" MYDEBUG, number 9: ME0GeometryBuilder, inside while (doEtaParts) after fv.sibling() name volume: "<<fv.name()<<" detId: "<<id<<endl;
	 cout<<" MYDEBUG, number 10: ME0GeometryBuilder, doEtaParts: "<<doEtaParts<<endl;
	 countEta = countEta + 1;
       }// ---------------------------------------------------- end eta partitions ---------------------------------------
      
            
       cout<<" MYDEBUG, number 11: ME0GeometryBuilder, inside while (doLayers) after  fv.up() name volume: "<<fv.name()<<" detId: "<<id<<endl;
       cout<<" MYDEBUG, number 12: ME0GeometryBuilder, doLayers: "<<doLayers<<endl;      
       doLayers = fv.nextSibling();//era fv.sibling()
       cout<<" MYDEBUG, number 13: ME0GeometryBuilder, doLayers: "<<doLayers<<endl;
       cout<<" MYDEBUG, number 14: ME0GeometryBuilder, inside while (doLayers) after fv.sibling() name volume: "<<fv.name()<<" detId: "<<id<<endl;
       countLayer = countLayer + 1;    
     } // ----------------------------------------------- end layers ---------------------------------------------------
     fv.parent();
     doChambers = fv.firstChild();//era fv.sibling()
     cout<<" MYDEBUG, number 15: ME0GeometryBuilder, inside while (doChamber) after fv.nextSibling() name volume: "<<fv.name()<<" detId: "<<id<<endl;
     cout<<" MYDEBUG, number 16: ME0GeometryBuilder, doChambers: "<<doChambers<<endl;
     countChamber = countChamber + 1;
  }
 
  cout<<"-------------------------------------------------------------------------------------------------------"<<endl; 
  cout<<"COUNTS,  Chamber: "<<countChamber<<" Layers: "<<countLayer<<" Eta: "<<countEta<<" EtaBis: "<<countEtaBis<<endl;
  cout<<"-------------------------------------------------------------------------------------------------------"<<endl; 

  return geometry;
}

ME0Chamber* ME0GeometryBuilderFromDDD::buildChamber(cms::DDFilteredView& fv, ME0DetId detId) const {

  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD buildChambers"<<endl;
  //  /*

    std::string my_title(fv.solid()->GetTitle());   
    if(my_title == "Trd1") cout<<"TRAPEZOID: "<<endl;
    if(my_title == "Subtraction") cout<<"SUBSTRACTIONSOLID "<<endl;

//  DDBooleanSolid solid = (DDBooleanSolid)(fv.logicalPart().solid()); // in DD
//  std::vector<double> dpar = solid.parameters(); // in DD

    std::vector<double> dpar = fv.parameters();  // funziona se il solido non e' subtraction (vedi CSC)

    double L = dpar[3];  //era /cm // length is along local Y
    double T = dpar[2];  // era /cm // thickness is long local Z
    double b = dpar[0];  //era /cm // bottom width is along local X
    double B = dpar[1];  //era  /cm // top width is along local X
  // hardcoded :: double b = 21.9859, B = 52.7261, L = 87.1678, T = 12.9;
cout<<"MYDEBUG, buildChamber, dpar: 1,2,3,4: "<<dpar[1]<<" "<<dpar[2]<<" "<<dpar[3]<<" "<<dpar[4]<<endl;
cout<<"MYDEBUG, buildChamber, dpar 5,6,7,8,0: "<<dpar[5]<<" "<<dpar[6]<<" "<<dpar[7]<<" "<<dpar[8]<<" "<<dpar[0]<<endl;

  bool isOdd = false;  // detId.chamber()%2;
   ME0BoundPlane surf(boundPlane(fv, new TrapezoidalPlaneBounds(b, B, L, T), isOdd));
  ME0Chamber* chamber = new ME0Chamber(detId.chamberId(), surf);
  // */
  //ME0Chamber* chamber;// to be deleted
  return chamber;
}

ME0Layer* ME0GeometryBuilderFromDDD::buildLayer(cms::DDFilteredView& fv, ME0DetId detId) const {
  //    /*
  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside buildLayer"<<endl;

  std::string my_title(fv.solid()->GetTitle());   
    if(my_title == "Trd1") cout<<"TRAPEZOID: "<<endl;
    if(my_title == "Subtraction") cout<<"SUBSTRACTIONSOLID "<<endl;

//  DDBooleanSolid solid = (DDBooleanSolid)(fv.logicalPart().solid());
  // std::vector<double> dpar = solid.solidA().parameters();
//  std::vector<double> dpar = solid.parameters();

 std::vector<double> dpar = fv.parameters();  // funziona se il solido non e' subtraction (vedi CSC)

 double L = dpar[3]; //era /cm // length is along local Y
 double t = dpar[2]; //era /cm  // thickness is long local Z
 double b = dpar[0]; //era cm  // bottom width is along local X
 double B = dpar[1]; //era/cm  // top width is along local X

cout<<"MYDEBUG, buildLayer, dpar: 1,2,3,4: "<<dpar[1]<<" "<<dpar[2]<<" "<<dpar[3]<<" "<<dpar[4]<<endl;
cout<<"MYDEBUG, buildLayer, dpar 5,6,7,8,0: "<<dpar[5]<<" "<<dpar[6]<<" "<<dpar[7]<<" "<<dpar[8]<<" "<<dpar[0]<<endl;

  bool isOdd = false;  // detId.chamber()%2;
  ME0BoundPlane surf(boundPlane(fv, new TrapezoidalPlaneBounds(b, B, L, t), isOdd));
  ME0Layer* layer = new ME0Layer(detId.layerId(), surf);
  //  */ 
  //ME0Layer* layer;// to be deleted
 return layer;
}

ME0EtaPartition* ME0GeometryBuilderFromDDD::buildEtaPartition(cms::DDFilteredView& fv, ME0DetId detId) const {

  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside buildEtaPartition"<<endl;
  // /*


  //  auto nStrips = fv.get<double>("nStrips");
  // auto nPads = fv.get<double>("nPads");

  auto nStrips = 384; // ok, but why doesn't the code read Geometry/GEMGeometryBuilder/data/v7/GEMSpecs.xml? 
  auto nPads = 192;  // ok, but why doesn't the code read Geometry/GEMGeometryBuilder/data/v7/GEMSpecs.xml? 

  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside buildEtaPartition, nStrips: "<<nStrips<<endl;
  cout<<" MYDEBUG, ME0GeometryBuilderFromDDD inside buildEtaPartition, nPads: "<<nPads<<endl;
  /*
  // EtaPartition specific parameter (nstrips and npads)
  DDValue numbOfStrips("nStrips");
  DDValue numbOfPads("nPads");
  std::vector<const DDsvalues_type*> specs(fv.specifics());
  std::vector<const DDsvalues_type*>::iterator is = specs.begin();
  double nStrips = 0., nPads = 0.;
  for (; is != specs.end(); is++) {
    if (DDfetch(*is, numbOfStrips))
      nStrips = numbOfStrips.doubles()[0];
    if (DDfetch(*is, numbOfPads))
      nPads = numbOfPads.doubles()[0];
  }
  */
  // EtaPartition specific parameter (size)
  //std::vector<double> dpar = fv.logicalPart().solid().parameters();
  
 std::vector<double> dpar = fv.parameters();

 double b = dpar[0];// era /cm era 4  // half bottom edge
 double B = dpar[1];//era /cm era 8  // half top edge
 double L = dpar[3];//era /cm era 0  // half apothem
 double t = dpar[2];//era /cm era 3  // half thickness

cout<<"MYDEBUG, buildEta, dpar: 1,2,3,4: "<<dpar[1]<<" "<<dpar[2]<<" "<<dpar[3]<<" "<<dpar[4]<<endl;
cout<<"MYDEBUG, buildEta, dpar 5,6,7,8,0: "<<dpar[5]<<" "<<dpar[6]<<" "<<dpar[7]<<" "<<dpar[8]<<" "<<dpar[0]<<endl;

  const std::vector<float> pars{float(dpar[0]), float(dpar[1]), float(dpar[3]), float(nStrips), float(nPads)};
  /*
  std::vector<float> pars;
  pars.emplace_back(b);
  pars.emplace_back(B);
  pars.emplace_back(L);
  pars.emplace_back(nStrips);
  pars.emplace_back(nPads);
  */
  bool isOdd = false;  // detId.chamber()%2; // this gives the opportunity (in future) to change the face of the chamber (electronics facing IP or electronics away from IP)
  ME0BoundPlane surf(boundPlane(fv, new TrapezoidalPlaneBounds(b, B, L, t), isOdd));
 
  std::string_view name = fv.name();
 
 ME0EtaPartitionSpecs* e_p_specs = new ME0EtaPartitionSpecs(GeomDetEnumerators::ME0, std::string(name), pars);

  ME0EtaPartition* etaPartition = new ME0EtaPartition(detId, surf, e_p_specs);
  // */
  // ME0EtaPartition* etaPartition;// to be deleted
  cout<<"MYDEBUG, buildEta, ADDED EtaId = "<<detId<<" name: "<<std::string(name)<<endl;
  return etaPartition;
}
 
ME0GeometryBuilderFromDDD::ME0BoundPlane ME0GeometryBuilderFromDDD::boundPlane(const cms::DDFilteredView& fv,
                                                                               Bounds* bounds,
                                                                               bool isOddChamber) const {

  // extract the position
  const Double_t* trans = fv.trans();
  Surface::PositionType posResult(trans[0], trans[1], trans[2]);

  // now the rotation
  DDRotationMatrix rotation;
  fv.rot(rotation);
  DD3Vector x, y, z;
  rotation.GetComponents(x, y, z);
  Surface::RotationType rotResult(float(x.X()),
                                  float(x.Y()),
                                  float(x.Z()),
                                  float(y.X()),
                                  float(y.Y()),
                                  float(y.Z()),
                                  float(z.X()),
                                  float(z.Y()),
                                  float(z.Z()));

  //Change of axes for the forward
  Basic3DVector<float> newX(1., 0., 0.);
  Basic3DVector<float> newY(0., 0., 1.);
  Basic3DVector<float> newZ(0., 1., 0.);
  newY *= -1;

  rotResult.rotateAxes(newX, newY, newZ);

  return ME0BoundPlane(new BoundPlane(posResult, rotResult, bounds));

}
 
