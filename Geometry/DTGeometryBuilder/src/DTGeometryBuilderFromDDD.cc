/** \file
 *
 *  \author N. Amapane - CERN. 
 */

#include <Geometry/DTGeometryBuilder/src/DTGeometryBuilderFromDDD.h>
#include <Geometry/DTGeometry/interface/DTGeometry.h>
#include <Geometry/DTGeometry/interface/DTChamber.h>
#include <Geometry/DTGeometry/interface/DTLayer.h>
#include <DetectorDescription/Core/interface/DDFilter.h>
#include <DetectorDescription/Core/interface/DDFilteredView.h>
#include <DetectorDescription/Core/interface/DDSolid.h>
#include "DataFormats/Math/interface/GeantUnits.h"
#include "Geometry/MuonNumbering/interface/MuonGeometryNumbering.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"
#include "Geometry/MuonNumbering/interface/DTNumberingScheme.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "CondFormats/GeometryObjects/interface/RecoIdealGeometry.h"
#include "CondFormats/Alignment/interface/DetectorGlobalPosition.h"
#include "CondFormats/Alignment/interface/AlignmentErrorsExtended.h"
#include "CondFormats/AlignmentRecord/interface/GlobalPositionRcd.h"
#include "CondFormats/AlignmentRecord/interface/DTAlignmentRcd.h"
#include "CondFormats/AlignmentRecord/interface/DTAlignmentErrorRcd.h"
#include "CondFormats/AlignmentRecord/interface/DTAlignmentErrorExtendedRcd.h"
#include "Geometry/CommonTopologies/interface/GeometryAligner.h"
#include "Geometry/Records/interface/DTRecoGeometryRcd.h"
#include "DataFormats/GeometrySurface/interface/Plane.h"
#include "DataFormats/GeometrySurface/interface/Bounds.h"
#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "Geometry/Records/interface/MuonNumberingRecord.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/Records/interface/GeometryFileRcd.h"
#include "DetectorDescription/DDCMS/interface/DDDetector.h"
#include "DetectorDescription/DDCMS/interface/DDFilteredView.h"
#include "DetectorDescription/DDCMS/interface/DDCompactView.h"
#include "DD4hep/Detector.h"
#include <utility>
#include <memory>
#include <string_view>
#include <string>
#include "DetectorDescription/DDCMS/interface/DDSpecParRegistry.h"
#include "DataFormats/Math/interface/CMSUnits.h"
#include "DataFormats/Math/interface/GeantUnits.h"

using namespace std;
using namespace geant_units;
using namespace geant_units::operators;


DTGeometryBuilderFromDDD::DTGeometryBuilderFromDDD() {}

DTGeometryBuilderFromDDD::~DTGeometryBuilderFromDDD() {}

void DTGeometryBuilderFromDDD::build(DTGeometry& theGeometry,
                                     const DDCompactView* cview,
                                     const MuonGeometryConstants& muonConstants) {
  //  cout << "DTGeometryBuilderFromDDD::build" << endl;
  //   static const string t0 = "DTGeometryBuilderFromDDD::build";
  //   TimeMe timer(t0,true);

  std::string attribute = "MuStructure";
  std::string value = "MuonBarrelDT";

  // Asking only for the Muon DTs
  DDSpecificsMatchesValueFilter filter{DDValue(attribute, value, 0.0)};

  DDFilteredView fview(*cview, filter);
  buildGeometry(theGeometry, fview, muonConstants);
}
//-------------start dd4hep------------
void DTGeometryBuilderFromDDD::build(DTGeometry& theGeometry,
                                     const cms::DDCompactView* cview,
                                     const MuonGeometryConstants& muonConstants) {
  const std::string attribute = "MuStructure";
  const std::string value = "MuonBarrelDT";
  cms::DDFilteredView fview(cview->detector(), cview->detector()->worldVolume());
  cms::DDSpecParRefs refs;
  const cms::DDSpecParRegistry& mypar = cview->specpars();
  mypar.filter(refs, attribute, value);
  fview.mergedSpecifics(refs);
  buildGeometry(theGeometry, fview, muonConstants);
}
//-------------fine dd4hep------------
void DTGeometryBuilderFromDDD::buildGeometry(DTGeometry& theGeometry,
                                             DDFilteredView& fv,
                                             const MuonGeometryConstants& muonConstants) const {
  bool doChamber = fv.firstChild();

  // Loop on chambers
  int ChamCounter = 0;
  while (doChamber) {
    ChamCounter++;
    DDValue val("Type");
    const DDsvalues_type params(fv.mergedSpecifics());
    string type;
    if (DDfetch(&params, val))
      type = val.strings()[0];
    // FIXME
    val = DDValue("FEPos");
    string FEPos;
    if (DDfetch(&params, val))
      FEPos = val.strings()[0];
    DTChamber* chamber = buildChamber(fv, type, muonConstants);

    // Loop on SLs
    bool doSL = fv.firstChild();
    int SLCounter = 0;
    while (doSL) {
      SLCounter++;
      DTSuperLayer* sl = buildSuperLayer(fv, chamber, type, muonConstants);
      theGeometry.add(sl);

      bool doL = fv.firstChild();
      int LCounter = 0;
      // Loop on SLs
      while (doL) {
        LCounter++;
        DTLayer* layer = buildLayer(fv, sl, type, muonConstants);
        theGeometry.add(layer);

        fv.parent();
        doL = fv.nextSibling();  // go to next layer
      }                          // layers

      fv.parent();
      doSL = fv.nextSibling();  // go to next SL
    }                           // sls
    theGeometry.add(chamber);

    fv.parent();
    doChamber = fv.nextSibling();  // go to next chamber
  }                                // chambers
}
//----inizio dd4hep---
void DTGeometryBuilderFromDDD::buildGeometry(DTGeometry& geom,
                                             cms::DDFilteredView& fview,
                                             const MuonGeometryConstants& num) const {

  bool doChamber = fview.firstChild();

  while (doChamber) {
    DTChamber* chamber = buildChamber(fview, num);

    // Loop on SLs
    bool doSL = fview.nextSibling();
    while (doSL) {
      DTSuperLayer* sl = buildSuperLayer(fview, chamber, num);

      // Loop on Layers
      fview.down();
      bool doLayers = fview.sibling();
      while (doLayers) {
        DTLayer* l = buildLayer(fview, sl, num);
        geom.add(l);

        doLayers = fview.sibling();  // go to next Layer
      }
      // Done with layers

      geom.add(sl);
      doSL = fview.nextSibling();  // go to next SL
    }
    geom.add(chamber);

    fview.parent();                  // stop iterating current branch
    doChamber = fview.firstChild();  // go to next chamber
  }

}
//------fine dd4hep---
DTChamber* DTGeometryBuilderFromDDD::buildChamber(DDFilteredView& fv,
                                                  const string& type,
                                                  const MuonGeometryConstants& muonConstants) const {
  MuonGeometryNumbering mdddnum(muonConstants);
  DTNumberingScheme dtnum(muonConstants);
  int rawid = dtnum.getDetId(mdddnum.geoHistoryToBaseNumber(fv.geoHistory()));
  DTChamberId detId(rawid);

  // Chamber specific parameter (size)
  // FIXME: some trouble for boolean solids?
  vector<double> par = extractParameters(fv);

  ///SL the definition of length, width, thickness depends on the local reference frame of the Det
  // width is along local X. r-phi  dimension - different in different chambers
  // length is along local Y. z      dimension - constant 125.55 cm
  // thickness is long local Z. radial thickness - almost constant about 18 cm

  RCPPlane surf(plane(fv, dtGeometryBuilder::getRecPlaneBounds(par.begin())));

  DTChamber* chamber = new DTChamber(detId, surf);

  return chamber;
}
//----------inizio dd4hep
DTChamber* DTGeometryBuilderFromDDD::buildChamber(cms::DDFilteredView& fview,
                                                  const MuonGeometryConstants& muonConstants) const {

  MuonGeometryNumbering mdddnum(muonConstants);
  DTNumberingScheme dtnum(muonConstants);
  int rawid = dtnum.baseNumberToUnitNumber(mdddnum.geoHistoryToBaseNumber(fview.history()));

  DTChamberId detId(rawid);
  auto const& par = fview.parameters();

  RCPPlane surf(plane(fview, new RectangularPlaneBounds(par[0], par[1], par[2])));

  DTChamber* chamber = new DTChamber(detId, surf);

  return chamber;

}
//-----------fine dd4hep

DTSuperLayer* DTGeometryBuilderFromDDD::buildSuperLayer(DDFilteredView& fv,
                                                        DTChamber* chamber,
                                                        const std::string& type,
                                                        const MuonGeometryConstants& muonConstants) const {
  MuonGeometryNumbering mdddnum(muonConstants);
  DTNumberingScheme dtnum(muonConstants);
  int rawid = dtnum.getDetId(mdddnum.geoHistoryToBaseNumber(fv.geoHistory()));
  DTSuperLayerId slId(rawid);

  // Slayer specific parameter (size)
  vector<double> par = extractParameters(fv);

  // r-phi  dimension - different in different chambers
  // z      dimension - constant 126.8 cm
  // radial thickness - almost constant about 20 cm

  // Ok this is the s-layer position...
  RCPPlane surf(plane(fv, dtGeometryBuilder::getRecPlaneBounds(par.begin())));

  DTSuperLayer* slayer = new DTSuperLayer(slId, surf, chamber);

  //LocalPoint lpos(10,20,30);
  //GlobalPoint gpos=slayer->toGlobal(lpos);

  // add to the chamber
  chamber->add(slayer);
  return slayer;
}
//------inizio dd4hep
DTSuperLayer* DTGeometryBuilderFromDDD::buildSuperLayer(cms::DDFilteredView& fview,
                                                        DTChamber* chamber,
                                                        const MuonGeometryConstants& muonConstants) const {
 
  MuonGeometryNumbering mdddnum(muonConstants);
    DTNumberingScheme dtnum(muonConstants);
    int rawid = dtnum.baseNumberToUnitNumber(mdddnum.geoHistoryToBaseNumber(fview.history()));

  DTSuperLayerId slId(rawid);

  auto const& par = fview.parameters();

  RCPPlane surf(plane(fview, new RectangularPlaneBounds(par[0], par[1], par[2])));

  DTSuperLayer* slayer = new DTSuperLayer(slId, surf, chamber);

  // add to the chamber
  chamber->add(slayer);

  return slayer;

}
//----fine dd4hep

DTLayer* DTGeometryBuilderFromDDD::buildLayer(DDFilteredView& fv,
                                              DTSuperLayer* sl,
                                              const std::string& type,
                                              const MuonGeometryConstants& muonConstants) const {
  MuonGeometryNumbering mdddnum(muonConstants);
  DTNumberingScheme dtnum(muonConstants);
  int rawid = dtnum.getDetId(mdddnum.geoHistoryToBaseNumber(fv.geoHistory()));
  DTLayerId layId(rawid);

  // Layer specific parameter (size)
  vector<double> par = extractParameters(fv);
  // width -- r-phi  dimension - different in different chambers
  // length -- z      dimension - constant 126.8 cm
  // thickness -- radial thickness - almost constant about 20 cm

  RCPPlane surf(plane(fv, dtGeometryBuilder::getRecPlaneBounds(par.begin())));

  // Loop on wires
  bool doWire = fv.firstChild();
  int WCounter = 0;
  int firstWire = fv.copyno();
  par = extractParameters(fv);
  float wireLength = convertMmToCm(par[1]);
  while (doWire) {
    WCounter++;
    doWire = fv.nextSibling();  // next wire
  }
  //int lastWire=fv.copyno();
  DTTopology topology(firstWire, WCounter, wireLength);

  DTLayerType layerType;

  DTLayer* layer = new DTLayer(layId, surf, topology, layerType, sl);

  sl->add(layer);
  return layer;
}
//----inizio dd4hep--
DTLayer* DTGeometryBuilderFromDDD::buildLayer(cms::DDFilteredView& fview,
                                              DTSuperLayer* sl,
                                              const MuonGeometryConstants& muonConstants) const {

 MuonGeometryNumbering mdddnum(muonConstants);
    DTNumberingScheme dtnum(muonConstants);
    int rawid = dtnum.baseNumberToUnitNumber(mdddnum.geoHistoryToBaseNumber(fview.history()));
  
  DTLayerId layId(rawid);

  auto const& par = fview.parameters();

  RCPPlane surf(plane(fview, new RectangularPlaneBounds(par[0], par[1], par[2])));

  // Loop on wires
  fview.down();
  bool doWire = fview.sibling();
  int firstWire = fview.volume()->GetNumber();  // copy no
  auto const& wpar = fview.parameters();
  float wireLength = wpar[1];

  int WCounter = 0;
  while (doWire) {
    doWire = fview.checkChild();
    WCounter++;
  }
  fview.up();

  DTTopology topology(firstWire, WCounter, wireLength);

  DTLayerType layerType;

  DTLayer* layer = new DTLayer(layId, surf, topology, layerType, sl);

  sl->add(layer);
  return layer;

}
//--------fine dd4hep--

vector<double> DTGeometryBuilderFromDDD::extractParameters(DDFilteredView& fv) const {
  vector<double> par;
  if (fv.logicalPart().solid().shape() != DDSolidShape::ddbox) {
    DDBooleanSolid bs(fv.logicalPart().solid());
    DDSolid A = bs.solidA();
    while (A.shape() != DDSolidShape::ddbox) {
      DDBooleanSolid bs(A);
      A = bs.solidA();
    }
    par = A.parameters();
  } else {
    par = fv.logicalPart().solid().parameters();
  }
  return par;
}

DTGeometryBuilderFromDDD::RCPPlane DTGeometryBuilderFromDDD::plane(const DDFilteredView& fv, Bounds* bounds) const {
  // extract the position
  const DDTranslation& trans(fv.translation());

  const Surface::PositionType posResult(
      float(convertMmToCm(trans.x())), float(convertMmToCm(trans.y())), float(convertMmToCm(trans.z())));
  LogTrace("DTGeometryBuilderFromDDD") << "DTGeometryBuilderFromDDD::plane "
                                       << " posResult: " << posResult << std::endl;
  // now the rotation
  //     'active' and 'passive' rotations are inverse to each other
  const DDRotationMatrix& rotation = fv.rotation();  //REMOVED .Inverse();
  DD3Vector x, y, z;
  rotation.GetComponents(x, y, z);
  //   std::cout << "INVERSE rotation by its own operator: "<< fv.rotation() << std::endl;
  //   std::cout << "INVERSE rotation manually: "
  // 	    << x.X() << ", " << x.Y() << ", " << x.Z() << std::endl
  // 	    << y.X() << ", " << y.Y() << ", " << y.Z() << std::endl
  // 	    << z.X() << ", " << z.Y() << ", " << z.Z() << std::endl;

  Surface::RotationType rotResult(float(x.X()),
                                  float(x.Y()),
                                  float(x.Z()),
                                  float(y.X()),
                                  float(y.Y()),
                                  float(y.Z()),
                                  float(z.X()),
                                  float(z.Y()),
                                  float(z.Z()));

  //   std::cout << "rotation by its own operator: "<< tmp << std::endl;
  //   DD3Vector tx, ty,tz;
  //   tmp.GetComponents(tx, ty, tz);
  //   std::cout << "rotation manually: "
  // 	    << tx.X() << ", " << tx.Y() << ", " << tx.Z() << std::endl
  // 	    << ty.X() << ", " << ty.Y() << ", " << ty.Z() << std::endl
  // 	    << tz.X() << ", " << tz.Y() << ", " << tz.Z() << std::endl;

  return RCPPlane(new Plane(posResult, rotResult, bounds));
}
//---------inizio dd4hep------
DTGeometryBuilderFromDDD::RCPPlane DTGeometryBuilderFromDDD::plane(const cms::DDFilteredView& fview, Bounds* bounds) const {
  const Double_t* tr = fview.trans();
  const Double_t* rot = fview.rot();

  return RCPPlane(
      new Plane(Surface::PositionType(tr[0], tr[1], tr[2]),
                Surface::RotationType(rot[0], rot[3], rot[6], rot[1], rot[4], rot[7], rot[2], rot[5], rot[8]),
                bounds));
}
//--------------fine dd4hep------------
