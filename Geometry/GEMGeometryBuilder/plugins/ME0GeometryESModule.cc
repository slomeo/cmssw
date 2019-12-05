/*
//\class ME0GeometryESModule

 Description: ME0 GeometryESModule from DD & DD4hep
              DD4hep part added to the original old file (DD version) made by M. Maggi (INFN Bari)
//
// Author:  Sergio Lo Meo (sergio.lo.meo@cern.ch) following what Ianna Osburne made for DTs (DD4HEP migration)
//          Created:  05 Dec 2019 
*/

#include "Geometry/GEMGeometryBuilder/src/ME0GeometryBuilderFromDDD.h"
#include "Geometry/GEMGeometryBuilder/src/ME0GeometryBuilderFromCondDB.h"

#include "Geometry/Records/interface/IdealGeometryRecord.h"
#include "Geometry/MuonNumbering/interface/MuonDDDConstants.h"
#include "DetectorDescription/Core/interface/DDCompactView.h"

#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"
#include <DetectorDescription/DDCMS/interface/DDCompactView.h>

#include "Geometry/Records/interface/ME0RecoGeometryRcd.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "CondFormats/GeometryObjects/interface/RecoIdealGeometry.h"

#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESTransientHandle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/ModuleFactory.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <memory>

// to debug
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

using namespace std;// to debug

using namespace edm;

class ME0GeometryESModule : public edm::ESProducer {
public:
  /// Constructor
  ME0GeometryESModule(const edm::ParameterSet& p);

  /// Destructor
  ~ME0GeometryESModule() override;

  /// Produce ME0Geometry.
  std::unique_ptr<ME0Geometry> produce(const MuonGeometryRecord& record);

private:
  edm::ESGetToken<DDCompactView, IdealGeometryRecord> cpvToken_;
  edm::ESGetToken<MuonDDDConstants, MuonNumberingRecord> mdcToken_;
  edm::ESGetToken<cms::DDCompactView, IdealGeometryRecord> dd4hepcpvToken_;
  edm::ESGetToken<cms::MuonNumbering, MuonNumberingRecord> dd4hepmdcToken_;
  edm::ESGetToken<RecoIdealGeometry, ME0RecoGeometryRcd> rigme0Token_;
  // use the DDD or DD4hep as Geometry source
  bool useDDD_;
  bool useDD4hep_;
};

ME0GeometryESModule::ME0GeometryESModule(const edm::ParameterSet& p) {
  useDDD_ = p.getParameter<bool>("useDDD");
  useDD4hep_ = p.getParameter<bool>("useDD4hep");
  auto cc = setWhatProduced(this);
  if (useDDD_) {
    cout<<" MYDEBUG, ME0GeometryESModule inside if useDDD_"<<endl;
    cc.setConsumes(cpvToken_).setConsumes(mdcToken_);
  } else if (useDD4hep_) {
    cout<<" MYDEBUG, ME0GeometryESModule inside if useDD4hep_"<<endl;
    cc.setConsumes(dd4hepcpvToken_).setConsumes(dd4hepmdcToken_);
  }   else {
    cout<<" MYDEBUG, ME0GeometryESModule inside else and NOT if useDDD_"<<endl;
    cc.setConsumes(rigme0Token_);
  }
}

ME0GeometryESModule::~ME0GeometryESModule() {}

std::unique_ptr<ME0Geometry> ME0GeometryESModule::produce(const MuonGeometryRecord& record) {
  LogTrace("ME0GeometryESModule") << "ME0GeometryESModule::produce with useDDD = " << useDDD_;
  cout<<" MYDEBUG, ME0GeometryESModule inside produce "<<endl;
  if (useDDD_) {
    LogTrace("ME0GeometryESModule") << "ME0GeometryESModule::produce :: ME0GeometryBuilderFromDDD builder";
    cout<<" MYDEBUG, ME0GeometryESModule inside produce and if useDDD_"<<endl;
    auto cpv = record.getTransientHandle(cpvToken_);
    const auto& mdc = record.get(mdcToken_);
    ME0GeometryBuilderFromDDD builder;
    return std::unique_ptr<ME0Geometry>(builder.build(cpv.product(), mdc));
  } /* else if (useDD4hep_) { // TO BE FIXED SEE ME0GEOMETRYBUILDERFROMMDDD CLASS
    LogTrace("ME0GeometryESModule") << "ME0GeometryESModule::produce :: ME0GeometryBuilderFromDDD builder DD4hep";
    cout<<" MYDEBUG, ME0GeometryESModule inside produce and if useDD4hep_"<<endl;
    auto cpv = record.getTransientHandle(dd4hepcpvToken_);
    const auto& mdc = record.get(dd4hepmdcToken_);
    ME0GeometryBuilderFromDDD builder;
    return std::unique_ptr<ME0Geometry>(builder.build(cpv.product(), mdc));
    } */else {
    LogTrace("ME0GeometryESModule") << "ME0GeometryESModule::produce :: ME0GeometryBuilderFromCondDB builder";
    cout<<" MYDEBUG, ME0GeometryESModule inside produce and else and NOT if  useDDD_"<<endl;
    const auto& rigme0 = record.get(rigme0Token_);
    ME0GeometryBuilderFromCondDB builder;
    return std::unique_ptr<ME0Geometry>(builder.build(rigme0));
  }
}

DEFINE_FWK_EVENTSETUP_MODULE(ME0GeometryESModule);
