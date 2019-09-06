/*
 *
 *
 */
#include "Geometry/RPCGeometryBuilder/src/RPCGeometryBuilderFromDDD.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCRollSpecs.h"

#include <DetectorDescription/Core/interface/DDFilter.h>
#include <DetectorDescription/Core/interface/DDFilteredView.h>
#include <DetectorDescription/DDCMS/interface/DDFilteredView.h>
#include <DetectorDescription/DDCMS/interface/DDCompactView.h>
#include <DetectorDescription/Core/interface/DDSolid.h>

#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"
#include "Geometry/MuonNumbering/interface/MuonDDDNumbering.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"
#include "Geometry/MuonNumbering/interface/RPCNumberingScheme.h"
#include "Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include "DataFormats/GeometryVector/interface/Basic3DVector.h"

#include "CLHEP/Units/GlobalSystemOfUnits.h"

#include <iostream>
#include <algorithm>
#include "DetectorDescription/DDCMS/interface/DDSpecParRegistry.h"
//#include "DD4hep/Detector.h"
#include "Geometry/MuonNumbering/interface/DD4hep_RPCNumberingScheme.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <math.h>

using namespace std;

RPCGeometryBuilderFromDDD::RPCGeometryBuilderFromDDD(bool comp11) : theComp11Flag(comp11) {}

RPCGeometryBuilderFromDDD::~RPCGeometryBuilderFromDDD() {}

// for DDD
RPCGeometry* RPCGeometryBuilderFromDDD::build(const DDCompactView* cview, const MuonDDDConstants& muonConstants) {
  const std::string attribute = "ReadOutName";
  const std::string value = "MuonRPCHits";

  // Asking only for the MuonRPC's
  DDSpecificsMatchesValueFilter filter{DDValue(attribute, value, 0.0)};
  DDFilteredView fview(*cview, filter);

  return this->buildGeometry(fview, muonConstants);
}

// for DD4hep
RPCGeometry* RPCGeometryBuilderFromDDD::build(const cms::DDCompactView* cview, const cms::MuonNumbering& muonConstants) {
  const std::string attribute = "ReadOutName";
  const std::string value = "MuonRPCHits";
  cms::DDFilteredView fview(cview->detector(), cview->detector()->worldVolume());
  cms::DDSpecParRefs refs;
  cms::DDSpecParRegistry mypar = cview->specpars(); 
  mypar.filter(refs, attribute, value);
  fview.mergedSpecifics(refs);
  return this->buildGeometry(fview, muonConstants);
}

// for DDD
RPCGeometry* RPCGeometryBuilderFromDDD::buildGeometry(DDFilteredView& fview, const MuonDDDConstants& muonConstants) {
  LogDebug("RPCGeometryBuilderFromDDD") << "Building the geometry service";
  RPCGeometry* geometry = new RPCGeometry();

  LogDebug("RPCGeometryBuilderFromDDD") << "About to run through the RPC structure\n"
                                        << " First logical part " << fview.logicalPart().name().name();
  bool doSubDets = fview.firstChild();

  LogDebug("RPCGeometryBuilderFromDDD") << "doSubDets = " << doSubDets;
  while (doSubDets) {
    LogDebug("RPCGeometryBuilderFromDDD") << "start the loop";

    // Get the Base Muon Number
    MuonDDDNumbering mdddnum(muonConstants);
    LogDebug("RPCGeometryBuilderFromDDD") << "Getting the Muon base Number";
    MuonBaseNumber mbn = mdddnum.geoHistoryToBaseNumber(fview.geoHistory());
    LogDebug("RPCGeometryBuilderFromDDD") << "Start the Rpc Numbering Schema";

    const int mylevel = mbn.getLevels();
    cout<<"------------myDEBUG  DDD--------------------------------------------------"<<endl;
    cout<<"DDD RPCGeometryBuilderFromDDD.cc: levels "<<mylevel<<endl;
    cout<<"DDD RPCGeometryBuilderFromDDD.cc: superNo "<<mbn.getSuperNo(mylevel)<<endl;
    cout<<"DDD RPCGeometryBuilderFromDDD.cc: baseNo "<<mbn.getBaseNo(mylevel)<<endl;
    cout<<"---------------------------------------------------------------------"<<endl;


    // Get the The Rpc det Id
    RPCNumberingScheme rpcnum(muonConstants);


    LogDebug("RPCGeometryBuilderFromDDD") << "Getting the Unit Number";
    const int detid = rpcnum.baseNumberToUnitNumber(mbn);

    cout<<"------------myDEBUG  DDD------------"<<endl;
    cout<<"DDD detid: "<<detid<<endl;
    cout<<"------------------------------------"<<endl;

    LogDebug("RPCGeometryBuilderFromDDD") << "Getting the RPC det Id " << detid;

    RPCDetId rpcid(detid);
    RPCDetId chid(rpcid.region(), rpcid.ring(), rpcid.station(), rpcid.sector(), rpcid.layer(), rpcid.subsector(), 0);

    LogDebug("RPCGeometryBuilderFromDDD") << "The RPCDetid is " << rpcid;

    DDValue numbOfStrips("nStrips");

    std::vector<const DDsvalues_type*> specs(fview.specifics());
    int nStrips = 0;
    for (auto& spec : specs) {
      if (DDfetch(spec, numbOfStrips)) {
        nStrips = int(numbOfStrips.doubles()[0]);
      }
    }

    LogDebug("RPCGeometryBuilderFromDDD") << ((nStrips == 0) ? ("No strip found!!") : (""));

    std::vector<double> dpar = fview.logicalPart().solid().parameters();
    std::string name = fview.logicalPart().name().name();
    DDTranslation tran = fview.translation();
    //removed .Inverse after comparing to DT...
    DDRotationMatrix rota = fview.rotation();  //.Inverse();
    Surface::PositionType pos(tran.x() / cm, tran.y() / cm, tran.z() / cm);
    // CLHEP way
    // Surface::RotationType rot(rota.xx(),rota.xy(),rota.xz(),
    // 			                     rota.yx(),rota.yy(),rota.yz(),
    // 			                     rota.zx(),rota.zy(),rota.zz());

    //ROOT::Math way
    DD3Vector x, y, z;
    rota.GetComponents(x, y, z);
    // doesn't this just re-inverse???
    Surface::RotationType rot(float(x.X()),
                              float(x.Y()),
                              float(x.Z()),
                              float(y.X()),
                              float(y.Y()),
                              float(y.Z()),
                              float(z.X()),
                              float(z.Y()),
                              float(z.Z()));

    RPCRollSpecs* rollspecs = nullptr;
    Bounds* bounds = nullptr;

    if (dpar.size() == 3) {
      const float width = dpar[0] / cm;
      const float length = dpar[1] / cm;
      const float thickness = dpar[2] / cm;
      //RectangularPlaneBounds*
      bounds = new RectangularPlaneBounds(width, length, thickness);
      const std::vector<float> pars = {width, length, float(numbOfStrips.doubles()[0]) /*h/2*/};

      if (!theComp11Flag) {
        //Correction of the orientation to get the REAL geometry.
        //Change of axes for the +z part only.
        //Including the 0 wheel
        if (tran.z() > -1500.) {
          Basic3DVector<float> newX(-1., 0., 0.);
          Basic3DVector<float> newY(0., -1., 0.);
          Basic3DVector<float> newZ(0., 0., 1.);
          rot.rotateAxes(newX, newY, newZ);
        }
      }

      rollspecs = new RPCRollSpecs(GeomDetEnumerators::RPCBarrel, name, pars);
      LogDebug("RPCGeometryBuilderFromDDD")
          << "Barrel " << name << " par " << width << " " << length << " " << thickness;
    } else {
      const float be = dpar[4] / cm;
      const float te = dpar[8] / cm;
      const float ap = dpar[0] / cm;
      const float ti = 0.4 / cm;
      //  TrapezoidalPlaneBounds*
      bounds = new TrapezoidalPlaneBounds(be, te, ap, ti);
      const std::vector<float> pars = {float(dpar[4] / cm) /*b/2*/,
                                       float(dpar[8] / cm) /*B/2*/,
                                       float(dpar[0] / cm) /*h/2*/,
                                       float(numbOfStrips.doubles()[0]) /*h/2*/};

      LogDebug("RPCGeometryBuilderFromDDD") << "Forward " << name << " par " << dpar[4] / cm << " " << dpar[8] / cm
                                            << " " << dpar[3] / cm << " " << dpar[0];

      rollspecs = new RPCRollSpecs(GeomDetEnumerators::RPCEndcap, name, pars);

      //Change of axes for the forward
      Basic3DVector<float> newX(1., 0., 0.);
      Basic3DVector<float> newY(0., 0., 1.);
      //      if (tran.z() > 0. )
      newY *= -1;
      Basic3DVector<float> newZ(0., 1., 0.);
      rot.rotateAxes(newX, newY, newZ);
    }
    LogDebug("RPCGeometryBuilderFromDDD") << "   Number of strips " << nStrips;

    BoundPlane* bp = new BoundPlane(pos, rot, bounds);
    ReferenceCountingPointer<BoundPlane> surf(bp);
    RPCRoll* r = new RPCRoll(rpcid, surf, rollspecs);
    geometry->add(r);

    auto rls = chids.find(chid);
    if (rls == chids.end())
      rls = chids.insert(std::make_pair(chid, std::list<RPCRoll*>())).first;
    rls->second.emplace_back(r);

    doSubDets = fview.nextSibling();  // go to next layer
  }
  // Create the RPCChambers and store them on the Geometry
  for (auto& ich : chids) {
    const RPCDetId& chid = ich.first;
    const auto& rls = ich.second;

    // compute the overall boundplane.
    BoundPlane* bp = nullptr;
    if (!rls.empty()) {
      // First set the baseline plane to calculate relative poisions
      const auto& refSurf = (*rls.begin())->surface();
      if (chid.region() == 0) {
        float corners[6] = {0, 0, 0, 0, 0, 0};
        for (auto rl : rls) {
          const double h2 = rl->surface().bounds().length() / 2;
          const double w2 = rl->surface().bounds().width() / 2;
          const auto x1y1AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(-w2, -h2, 0)));
          const auto x2y2AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(+w2, +h2, 0)));
          corners[0] = std::min(corners[0], x1y1AtRef.x());
          corners[1] = std::min(corners[1], x1y1AtRef.y());
          corners[2] = std::max(corners[2], x2y2AtRef.x());
          corners[3] = std::max(corners[3], x2y2AtRef.y());

          corners[4] = std::min(corners[4], x1y1AtRef.z());
          corners[5] = std::max(corners[5], x1y1AtRef.z());
        }
        const LocalPoint lpOfCentre((corners[0] + corners[2]) / 2, (corners[1] + corners[3]) / 2, 0);
        const auto gpOfCentre = refSurf.toGlobal(lpOfCentre);
        auto bounds = new RectangularPlaneBounds(
            (corners[2] - corners[0]) / 2, (corners[3] - corners[1]) / 2, (corners[5] - corners[4]) + 0.5);
        bp = new BoundPlane(gpOfCentre, refSurf.rotation(), bounds);
      } else {
        float cornersLo[3] = {0, 0, 0}, cornersHi[3] = {0, 0, 0};
        float cornersZ[2] = {0, 0};
        for (auto rl : rls) {
          const double h2 = rl->surface().bounds().length() / 2;
          const double w2 = rl->surface().bounds().width() / 2;
          const auto& topo = dynamic_cast<const TrapezoidalStripTopology&>(rl->specificTopology());
          const double r = topo.radius();
          const double wAtLo = w2 / r * (r - h2);  // tan(theta/2) = (w/2)/r = x/(r-h/2)
          const double wAtHi = w2 / r * (r + h2);

          const auto x1y1AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(-wAtLo, -h2, 0)));
          const auto x2y1AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(+wAtLo, -h2, 0)));
          const auto x1y2AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(-wAtHi, +h2, 0)));
          const auto x2y2AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(+wAtHi, +h2, 0)));

          cornersLo[0] = std::min(cornersLo[0], x1y1AtRef.x());
          cornersLo[1] = std::max(cornersLo[1], x2y1AtRef.x());
          cornersLo[2] = std::min(cornersLo[2], x1y1AtRef.y());

          cornersHi[0] = std::min(cornersHi[0], x1y2AtRef.x());
          cornersHi[1] = std::max(cornersHi[1], x2y2AtRef.x());
          cornersHi[2] = std::max(cornersHi[2], x1y2AtRef.y());

          cornersZ[0] = std::min(cornersZ[0], x1y1AtRef.z());
          cornersZ[1] = std::max(cornersZ[1], x1y1AtRef.z());
        }
        const LocalPoint lpOfCentre((cornersHi[0] + cornersHi[1]) / 2, (cornersLo[2] + cornersHi[2]) / 2, 0);
        const auto gpOfCentre = refSurf.toGlobal(lpOfCentre);
        auto bounds = new TrapezoidalPlaneBounds((cornersLo[1] - cornersLo[0]) / 2,
                                                 (cornersHi[1] - cornersHi[0]) / 2,
                                                 (cornersHi[2] - cornersLo[2]) / 2,
                                                 (cornersZ[1] - cornersZ[0]) + 0.5);
        bp = new BoundPlane(gpOfCentre, refSurf.rotation(), bounds);
      }
    }

    ReferenceCountingPointer<BoundPlane> surf(bp);
    // Create the chamber
    RPCChamber* ch = new RPCChamber(chid, surf);
    // Add the rolls to rhe chamber
    for (auto rl : rls)
      ch->add(rl);
    // Add the chamber to the geometry
    geometry->add(ch);
  }
  return geometry;
}


// for DD4hep
RPCGeometry* RPCGeometryBuilderFromDDD::buildGeometry(cms::DDFilteredView& fview, const cms::MuonNumbering& muonConstants) {

  // cout<<"------------myDEBUG------------"<<endl;
  //  cout<<"inside buidlGeometry from DD4HEP"<<endl;
  //  cout<<"-------------------------------"<<endl;


  RPCGeometry* geometry = new RPCGeometry();

  bool doSubDets = fview.firstChild();

  //cout<<"------------myDEBUG------------"<<endl;
  // cout<<"doSubDets: "<<doSubDets<<endl;
  //cout<<"-------------------------------"<<endl;

  while (doSubDets) {
    //    cout<<"From RPCGeometryBuilderFromDDD.cc "<<endl;
    //    cout<<"------------myDEBUG------------"<<endl;
    // cout<<"inside while(doSubDets)"<<endl;
    //cout<<"-------------------------------"<<endl;

    MuonBaseNumber mbn = muonConstants.geoHistoryToBaseNumber(fview.history());
    const int mylevel = mbn.getLevels();
    cout<<"------------myDEBUG  DD4HEP--------------------------------------------------"<<endl;
    cout<<"DD4HEP RPCGeometryBuilderFromDDD.cc: levels "<<mylevel<<endl;
    cout<<"DD4HEP RPCGeometryBuilderFromDDD.cc: superNo "<<mbn.getSuperNo(mylevel)<<endl;
    cout<<"DD4HEP RPCGeometryBuilderFromDDD.cc: baseNo "<<mbn.getBaseNo(mylevel)<<endl;
    cout<<"---------------------------------------------------------------------"<<endl;
    
    //    for (auto& i: muonConstants.values()) 
    //  { 
    //	cout<<"RPCGeometryBuilderFromDDD.cc  "<<i.first<<": "<<i.second<<endl; 
    //  } 

    cms::RPCNumberingScheme rpcnum(muonConstants.values());
    
    //cout<<"------------myDEBUG--------------------------------------------------"<<endl;
    //cout<<"after line:  cms::RPCNumberingScheme rpcnum(muonConstants.values()); "<<endl;
    //cout<<"---------------------------------------------------------------------"<<endl;

    rpcnum.baseNumberToUnitNumber(mbn);
    int detid = rpcnum.GetDetId();
   
    cout<<"------------myDEBUG------------"<<endl;
    cout<<"detid: "<<detid<<endl;
    cout<<"-------------------------------"<<endl;

    RPCDetId rpcid(detid);   
    RPCDetId chid(rpcid.region(), rpcid.ring(), rpcid.station(), rpcid.sector(), rpcid.layer(), rpcid.subsector(), 0);

    // DD4HEP: to be implemented
    // DDValue numbOfStrips("nStrips");
    const int nStrips = 42;
      
    // std::vector<double> dpar = fview.logicalPart().solid().parameters(); //DDD
    std::vector<double> dpar = fview.extractParameters();
    //std::string name = fview.logicalPart().name().name();    //DDD
    std::string_view name = fview.name();
    
    //DDTranslation tran = fview.translation();// DDD
    const Double_t* tran = fview.trans();
    //DDRotationMatrix rota = fview.rotation();// DDD
    const Double_t* rota = fview.rot();  
    // Surface::PositionType pos(tran.x() / cm, tran.y() / cm, tran.z() / cm);//DDD
    Surface::PositionType pos(tran[0] / cm, tran[1] / cm, tran[2] / cm);
   

    Surface::RotationType rot(rota[0],
			      rota[3],
			      rota[6],
			      rota[1],
			      rota[4],
			      rota[7],
			      rota[2],
			      rota[5],
			      rota[8]);
    
    RPCRollSpecs* rollspecs = nullptr;
    Bounds* bounds = nullptr;

    if (dpar.size() == 3) {
      const float width = dpar[0] / cm;
      const float length = dpar[1] / cm;
      const float thickness = dpar[2] / cm;
      bounds = new RectangularPlaneBounds(width, length, thickness);
      // const std::vector<float> pars = {width, length, float(numbOfStrips.doubles()[0])};//DDD
      const std::vector<float> pars = {width, length, float(nStrips)};//to be implemented for DD4HEP     

      if (!theComp11Flag) {
	//if (tran.z() > -1500.) {//DDD
	if (tran[2] > -1500.) {

	  ROOT::Math::Rotation3D myMatrix = ROOT::Math::Rotation3D(-1.,0.,0.,0.,-1.,0.,0.,0.,1.);
	  fview.rot(myMatrix);
	}
      }

      //rollspecs = new RPCRollSpecs(GeomDetEnumerators::RPCBarrel, name, pars); //DDD
      rollspecs = new RPCRollSpecs(GeomDetEnumerators::RPCBarrel, std::string(name).c_str(), pars);
  
    } else {
      const float be = dpar[4] / cm;
      const float te = dpar[8] / cm;
      const float ap = dpar[0] / cm;
      const float ti = 0.4 / cm;
      //  TrapezoidalPlaneBounds*
      bounds = new TrapezoidalPlaneBounds(be, te, ap, ti);
      const std::vector<float> pars = {float(dpar[4] / cm) ,
                                       float(dpar[8] / cm) ,
                                       float(dpar[0] / cm) ,
                                       float(nStrips)};
      /* //DDD
      const std::vector<float> pars = {float(dpar[4] / cm) ,
                                       float(dpar[8] / cm) ,
                                       float(dpar[0] / cm) ,
                                       float(numbOfStrips.doubles()[0])};
      */
      // rollspecs = new RPCRollSpecs(GeomDetEnumerators::RPCEndcap, name, pars); //DDD
      rollspecs = new RPCRollSpecs(GeomDetEnumerators::RPCEndcap, std::string(name).c_str(), pars);

      ROOT::Math::Rotation3D myMatrix2 = ROOT::Math::Rotation3D(1.,0.,1.,0.,0.,-1.,0.,1.,0.);
      fview.rot(myMatrix2);
    }
    
    BoundPlane* bp = new BoundPlane(pos, rot, bounds);
    ReferenceCountingPointer<BoundPlane> surf(bp);
    RPCRoll* r = new RPCRoll(rpcid, surf, rollspecs);
    geometry->add(r);

    auto rls = chids.find(chid);
    if (rls == chids.end())
      rls = chids.insert(std::make_pair(chid, std::list<RPCRoll*>())).first;
    rls->second.emplace_back(r);

    doSubDets = fview.nextSibling(); 
  }
  
  for (auto& ich : chids) {
    const RPCDetId& chid = ich.first;
    const auto& rls = ich.second;

  
    BoundPlane* bp = nullptr;
    if (!rls.empty()) {
  
      const auto& refSurf = (*rls.begin())->surface();
      if (chid.region() == 0) {
        float corners[6] = {0, 0, 0, 0, 0, 0};
        for (auto rl : rls) {
          const double h2 = rl->surface().bounds().length() / 2;
          const double w2 = rl->surface().bounds().width() / 2;
          const auto x1y1AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(-w2, -h2, 0)));
          const auto x2y2AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(+w2, +h2, 0)));
          corners[0] = std::min(corners[0], x1y1AtRef.x());
          corners[1] = std::min(corners[1], x1y1AtRef.y());
          corners[2] = std::max(corners[2], x2y2AtRef.x());
          corners[3] = std::max(corners[3], x2y2AtRef.y());

          corners[4] = std::min(corners[4], x1y1AtRef.z());
          corners[5] = std::max(corners[5], x1y1AtRef.z());
        }
        const LocalPoint lpOfCentre((corners[0] + corners[2]) / 2, (corners[1] + corners[3]) / 2, 0);
        const auto gpOfCentre = refSurf.toGlobal(lpOfCentre);
        auto bounds = new RectangularPlaneBounds(
            (corners[2] - corners[0]) / 2, (corners[3] - corners[1]) / 2, (corners[5] - corners[4]) + 0.5);
        bp = new BoundPlane(gpOfCentre, refSurf.rotation(), bounds);
      } else {
        float cornersLo[3] = {0, 0, 0}, cornersHi[3] = {0, 0, 0};
        float cornersZ[2] = {0, 0};
        for (auto rl : rls) {
          const double h2 = rl->surface().bounds().length() / 2;
          const double w2 = rl->surface().bounds().width() / 2;
          const auto& topo = dynamic_cast<const TrapezoidalStripTopology&>(rl->specificTopology());
          const double r = topo.radius();
          const double wAtLo = w2 / r * (r - h2); 
          const double wAtHi = w2 / r * (r + h2);

          const auto x1y1AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(-wAtLo, -h2, 0)));
          const auto x2y1AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(+wAtLo, -h2, 0)));
          const auto x1y2AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(-wAtHi, +h2, 0)));
          const auto x2y2AtRef = refSurf.toLocal(rl->toGlobal(LocalPoint(+wAtHi, +h2, 0)));

          cornersLo[0] = std::min(cornersLo[0], x1y1AtRef.x());
          cornersLo[1] = std::max(cornersLo[1], x2y1AtRef.x());
          cornersLo[2] = std::min(cornersLo[2], x1y1AtRef.y());

          cornersHi[0] = std::min(cornersHi[0], x1y2AtRef.x());
          cornersHi[1] = std::max(cornersHi[1], x2y2AtRef.x());
          cornersHi[2] = std::max(cornersHi[2], x1y2AtRef.y());

          cornersZ[0] = std::min(cornersZ[0], x1y1AtRef.z());
          cornersZ[1] = std::max(cornersZ[1], x1y1AtRef.z());
        }
        const LocalPoint lpOfCentre((cornersHi[0] + cornersHi[1]) / 2, (cornersLo[2] + cornersHi[2]) / 2, 0);
        const auto gpOfCentre = refSurf.toGlobal(lpOfCentre);
        auto bounds = new TrapezoidalPlaneBounds((cornersLo[1] - cornersLo[0]) / 2,
                                                 (cornersHi[1] - cornersHi[0]) / 2,
                                                 (cornersHi[2] - cornersLo[2]) / 2,
                                                 (cornersZ[1] - cornersZ[0]) + 0.5);
        bp = new BoundPlane(gpOfCentre, refSurf.rotation(), bounds);
      }
    }

    ReferenceCountingPointer<BoundPlane> surf(bp);
   
    RPCChamber* ch = new RPCChamber(chid, surf);
   
    for (auto rl : rls)
      ch->add(rl);
   
    geometry->add(ch);
  }

   return geometry;
}

