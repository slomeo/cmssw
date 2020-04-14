/*
//\class GEMGeometryValidate

 Description: ME0 GeometryValidate from DD & DD4hep
 
 //
// Author:  Sergio Lo Meo (sergio.lo.meo@cern.ch) following what Ianna Osburne made for DTs (DD4HEP migration)
//          Created:  02 Dec 2019 
*/

#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include "Geometry/GEMGeometry/interface/ME0Chamber.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "Fireworks/Core/interface/FWGeometry.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include <TFile.h>
#include <TH1.h>

#include <limits>
#include <string>
#include <type_traits>
#include <algorithm>

// to debug
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

using namespace std;// to debug

template <class T>
typename enable_if<!numeric_limits<T>::is_integer, bool>::type almost_equal(T x, T y, int ulp) {
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  return abs(x - y) <= numeric_limits<T>::epsilon() * abs(x + y) * ulp
    // unless the result is subnormal
    || abs(x - y) < numeric_limits<T>::min();
}

using namespace edm;

class ME0GeometryValidate : public one::EDAnalyzer<> {
public:
  explicit ME0GeometryValidate(const ParameterSet&);
  ~ME0GeometryValidate() override {}
  
private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;
  
  void validateME0ChamberGeometry();
  void validateME0EtaPartitionGeometry();
  
  void compareTransform(const GlobalPoint&, const TGeoMatrix*);
  void compareShape(const GeomDet*, const float*);
  
  float getDistance(const GlobalPoint&, const GlobalPoint&);
  float getDiff(const float, const float);
  
  void makeHistograms(const char*);
  void makeHistograms2(const char*);
  void makeHistogram(const string&, vector<float>&);
  
  void clearData() {
    globalDistances_.clear();
    topWidths_.clear();
    bottomWidths_.clear();
    lengths_.clear();
    thicknesses_.clear();
  }

  void clearData2() {
    nstrips_.clear();
    pitch_.clear();
    stripslen_.clear();
  }
  
  edm::ESHandle<ME0Geometry> me0Geometry_;
  FWGeometry fwGeometry_;
  TFile* outFile_;
  vector<float> globalDistances_;
  vector<float> topWidths_;
  vector<float> bottomWidths_;
  vector<float> lengths_;
  vector<float> thicknesses_;
  vector<float> nstrips_;
  vector<float> pitch_;
  vector<float> stripslen_;
  string infileName_;
  string outfileName_;
  int tolerance_;
};

ME0GeometryValidate::ME0GeometryValidate(const edm::ParameterSet& iConfig)
  : infileName_(iConfig.getUntrackedParameter<string>("infileName", "cmsRecoGeom-2026.root")),//it was cmsGeom10.root
    outfileName_(iConfig.getUntrackedParameter<string>("outfileName", "validateME0Geometry.root")),
    tolerance_(iConfig.getUntrackedParameter<int>("tolerance", 6)) {
  fwGeometry_.loadMap(infileName_.c_str());
  outFile_ = new TFile(outfileName_.c_str(), "RECREATE");
}

void ME0GeometryValidate::analyze(const edm::Event& event, const edm::EventSetup& eventSetup) {
  cout<<" MYVALIDATE, before get me0Geometry_"<<endl;
  eventSetup.get<MuonGeometryRecord>().get(me0Geometry_);
  cout<<" MYVALIDATE, after get me0Geometry_"<<endl;
  if (me0Geometry_.isValid()) {
    cout<<" MYVALIDATE, me0Geometry_.isvalid()"<<endl;
    LogVerbatim("ME0Geometry") << "Validating ME0 chamber geometry";
    cout<<" MYVALIDATE, start validate chambers"<<endl;
    validateME0ChamberGeometry();
    cout<<" MYVALIDATE, end validate chambers"<<endl;
    cout<<" MYVALIDATE, start validate eta partitions"<<endl;
    validateME0EtaPartitionGeometry();
    cout<<" MYVALIDATE, end validate eta partitions"<<endl;
  } else
    LogVerbatim("ME0Geometry") << "Invalid ME0 geometry";
}

void ME0GeometryValidate::validateME0ChamberGeometry() {
  
  clearData();
    cout<<" MYVALIDATE, inside validate chambers"<<endl;

  for (auto const& it : me0Geometry_->chambers()) {
    cout<<" MYVALIDATE, inside chambers loop"<<endl;
    ME0DetId chId = it->id();
    GlobalPoint gp = it->surface().toGlobal(LocalPoint(0.0, 0.0, 0.0));

    const TGeoMatrix* matrix = fwGeometry_.getMatrix(chId.rawId());

    if (!matrix) {
    cout<<" MYVALIDATE, Failed to get matrix of ME0 chamber with detid:"  << chId.rawId()<<endl;
      LogVerbatim("ME0Geometry") << "Failed to get matrix of ME0 chamber with detid: " << chId.rawId();
      continue;
    }
    compareTransform(gp, matrix);

    auto const& shape = fwGeometry_.getShapePars(chId.rawId());

    if (!shape) {
      LogVerbatim("ME0Geometry") << "Failed to get shape of ME0 chamber with detid: " << chId.rawId();
      cout<<" MYVALIDATE, Failed to get shape of ME0 chamber with detid:"  << chId.rawId()<<endl;
      continue;
    }
    compareShape(it, shape);
  }
  makeHistograms("ME0 Chamber");

  cout<<" MYVALIDATE, done histos ME0 Chambers"<<endl;

}

void ME0GeometryValidate::validateME0EtaPartitionGeometry() {
  clearData2();
  //  cout<<" MYVALIDATE, inside validate EtaPartion (strips)"<<endl;

  for (auto const& it : me0Geometry_->etaPartitions()) {
    // cout<<" MYVALIDATE, inside Eta partition loop"<<endl;
    ME0DetId chId = it->id();
    const int n_strips = it->nstrips();
    const float n_pitch = it->pitch();
    const int n_pads = it->npads();
    const StripTopology& topo = it->specificTopology();
    const float stripLen = topo.stripLength();
    const float* parameters = fwGeometry_.getParameters(chId.rawId());
    nstrips_.push_back(fabs(n_strips - parameters[0]));
    // cout<<" MYVALIDATE, nstrips: "<<n_strips<<" nstrips RECO: "<<parameters[0]<<endl;
    // cout<<" MYVALIDATE, npads: "<<n_pads<<endl;
    if (n_strips) {
      for (int istrips = 1; istrips <= n_strips; istrips++) {

	// cout<<" MYVALIDATE, npitch: "<<n_pitch<<" npitch RECO: "<<parameters[2]<<endl;

	if (fabs(n_pitch - parameters[2]) < 1.0e-05)
	pitch_.push_back(0.0f);  
	else
	pitch_.push_back(fabs(n_pitch - parameters[2]));	 

      //pitch_.push_back(fabs(n_pitch - parameters[2]));	 

      if (fabs(stripLen - parameters[1]) < 1.0e-05)
	pitch_.push_back(0.0f);  
	else
	stripslen_.push_back(fabs(stripLen - parameters[1]));
	//cout<<" MYVALIDATE, striplen: "<<stripLen<<" stripLen RECO: "<<parameters[1]<<endl;
        //stripslen_.push_back(fabs(stripLen - parameters[1]));
      }
    } else {
      LogVerbatim("ME0Geometry") << "ATTENTION! nStrips == 0";
    }
  }
  makeHistograms2("ME0 Eta Partition");

  // cout<<" MYVALIDATE, done histos ME0 EtaPartitions"<<endl;
}

void ME0GeometryValidate::compareTransform(const GlobalPoint& gp, const TGeoMatrix* matrix) {
  double local[3] = {0.0, 0.0, 0.0};
  double global[3];

  matrix->LocalToMaster(local, global);

  float distance = getDistance(GlobalPoint(global[0], global[1], global[2]), gp);
  if ((distance >= 0.0) && (distance < 1.0e-7))
    distance = 0.0;  // set a tollerance for the distance inside Histos
  globalDistances_.push_back(distance);
}

void ME0GeometryValidate::compareShape(const GeomDet* det, const float* shape) {
  float shapeTopWidth;
  float shapeBottomWidth;
  float shapeLength;
  float shapeThickness;

  if (shape[0] == 1) {
    shapeTopWidth = shape[2];
    shapeBottomWidth = shape[1];
    shapeLength = shape[4];
    shapeThickness = shape[3];
    cout<<" MYVALIDATE, RECO shape[0] == 1"<<endl;
    cout<<" MYVALIDATE, RECO: shapeTopWidth: "<<shapeTopWidth<<" shapeBottomWidth: "<<shapeBottomWidth<<" shapeLength: "<<shapeLength<<" shapeThickness: "<<shapeThickness<<endl;
  } else if (shape[0] == 2) {
    cout<<" MYVALIDATE, RECO shape[0] == 2"<<endl;
    cout<<" MYVALIDATE, RECO: shapeTopWidth: "<<shapeTopWidth<<" shapeBottomWidth: "<<shapeBottomWidth<<" shapeLength: "<<shapeLength<<" shapeThickness: "<<shapeThickness<<endl;
    shapeTopWidth = shape[1];
    shapeBottomWidth = shape[1];
    shapeLength = shape[2];
    shapeThickness = shape[3];
  } else {
    cout<<" MYVALIDATE, Failed  to get box or trapezoid from shape"<<endl;
    LogVerbatim("ME0Geometry") << "Failed to get box or trapezoid from shape";

    return;
  }

  float topWidth, bottomWidth;
  float length, thickness;

  const Bounds* bounds = &(det->surface().bounds());
  if (const TrapezoidalPlaneBounds* tpbs = dynamic_cast<const TrapezoidalPlaneBounds*>(bounds)) {
    array<const float, 4> const& ps = tpbs->parameters();

    assert(ps.size() == 4);
    cout<<" MYVALIDATE, inside TrapezoidalPlaneBounds"<<endl;
    bottomWidth = ps[0];
    topWidth = ps[1];
    thickness = ps[2];
    length = ps[3];
    cout<<" MYVALIDATE : shapeTopWidth: "<<topWidth<<" shapeBottomWidth: "<<bottomWidth<<" shapeLength: "<<length<<" shapeThickness: "<<thickness<<endl;
  } else if ((dynamic_cast<const RectangularPlaneBounds*>(bounds))) {
    cout<<" MYVALIDATE, inside RectangularPlaneBounds"<<endl;
    length = det->surface().bounds().length() * 0.5;
    topWidth = det->surface().bounds().width() * 0.5;
    bottomWidth = topWidth;
    thickness = det->surface().bounds().thickness() * 0.5;
    cout<<" MYVALIDATE : shapeTopWidth: "<<topWidth<<" shapeBottomWidth: "<<bottomWidth<<" shapeLength: "<<length<<" shapeThickness: "<<thickness<<endl;
  } else {
    LogVerbatim("ME0Geometry") << "Failed to get bounds";
    cout<<" MYVALIDATE, Failed to get bounds"<<endl;
    return;
  }
  topWidths_.push_back(fabs(shapeTopWidth - topWidth));
  bottomWidths_.push_back(fabs(shapeBottomWidth - bottomWidth));
  lengths_.push_back(fabs(shapeLength - length));
  thicknesses_.push_back(fabs(shapeThickness - thickness));
}

float ME0GeometryValidate::getDistance(const GlobalPoint& p1, const GlobalPoint& p2) {
  return sqrt((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()) +
              (p1.z() - p2.z()) * (p1.z() - p2.z()));
}

float ME0GeometryValidate::getDiff(const float val1, const float val2) {
  if (almost_equal(val1, val2, tolerance_))
    return 0.0f;
  else
    return (val1 - val2);
}

void ME0GeometryValidate::makeHistograms(const char* detector) {
  outFile_->cd();

  string d(detector);

  string gdn = d + ": distance between points in global coordinates";
  makeHistogram(gdn, globalDistances_);

  string twn = d + ": absolute difference between top widths (along X)";
  makeHistogram(twn, topWidths_);

  string bwn = d + ": absolute difference between bottom widths (along X)";
  makeHistogram(bwn, bottomWidths_);

  string ln = d + ": absolute difference between lengths (along Y)";
  makeHistogram(ln, lengths_);

  string tn = d + ": absolute difference between thicknesses (along Z)";
  makeHistogram(tn, thicknesses_);
}

void ME0GeometryValidate::makeHistograms2(const char* detector) {
  outFile_->cd();

  string d(detector);

  string ns = d + ": absolute difference between nStrips";
  makeHistogram(ns, nstrips_);

  string pi = d + ": absolute difference between Strips Pitch";
  makeHistogram(pi, pitch_);

  string pl = d + ": absolute difference between Strips Length";
  makeHistogram(pl, stripslen_);
}

void ME0GeometryValidate::makeHistogram(const string& name, vector<float>& data) {
  if (data.empty())
    return;

  const auto [minE, maxE] = minmax_element(begin(data), end(data));

  TH1D hist(name.c_str(), name.c_str(), 100, *minE * (1 + 0.10), *maxE * (1 + 0.10));

  for (auto const& it : data)
    hist.Fill(it);

  hist.GetXaxis()->SetTitle("[cm]");
  hist.Write();
}

void ME0GeometryValidate::beginJob() { outFile_->cd(); }

void ME0GeometryValidate::endJob() {
  LogVerbatim("ME0Geometry") << "Done.";
  LogVerbatim("ME0Geometry") << "Results written to " << outfileName_;
  outFile_->Close();
}

DEFINE_FWK_MODULE(ME0GeometryValidate);
