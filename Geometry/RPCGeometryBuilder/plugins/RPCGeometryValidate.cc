#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCChamber.h"
#include "Geometry/RPCGeometry/interface/RPCRoll.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "Geometry/Records/interface/MuonGeometryRecord.h"

#include "Fireworks/Core/interface/FWGeometry.h"

#include "DataFormats/GeometrySurface/interface/RectangularPlaneBounds.h"
#include "DataFormats/GeometrySurface/interface/TrapezoidalPlaneBounds.h"

#include <TFile.h>
#include <TH1.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <math.h>

#include <cmath>
#include <limits>
#include <string>
#include <type_traits>
#include <algorithm>

using namespace std;

template <class T>
typename enable_if<!numeric_limits<T>::is_integer, bool>::type almost_equal(T x, T y, int ulp) {
  // the machine epsilon has to be scaled to the magnitude of the values used
  // and multiplied by the desired precision in ULPs (units in the last place)
  return abs(x - y) <= numeric_limits<T>::epsilon() * abs(x + y) * ulp
         // unless the result is subnormal
         || abs(x - y) < numeric_limits<T>::min();
}

using namespace edm;

class RPCGeometryValidate : public one::EDAnalyzer<> {
public:
  explicit RPCGeometryValidate(const ParameterSet&);
  ~RPCGeometryValidate() override {}

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  void validateRPCChamberGeometry();
  void validateRPCStripsGeometry();

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

  //edm::ESGetToken<RPCGeometry, MuonGeometryRecord> rpcGeometryToken_;
  edm::ESHandle<RPCGeometry> rpcGeometry_;
  // RPCGeometry rpcGeometry;
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

RPCGeometryValidate::RPCGeometryValidate(const edm::ParameterSet& iConfig)
  : //rpcGeometryToken_{esConsumes<RPCGeometry, MuonGeometryRecord>(edm::ESInputTag{})},
      infileName_(iConfig.getUntrackedParameter<string>("infileName", "cmsGeom10.root")),
      outfileName_(iConfig.getUntrackedParameter<string>("outfileName", "validateRPCGeometry.root")),
      tolerance_(iConfig.getUntrackedParameter<int>("tolerance", 6)) {
  fwGeometry_.loadMap(infileName_.c_str());
  outFile_ = new TFile(outfileName_.c_str(), "RECREATE");
  //  cout<<"MYDEBUG: RPCGeometryValide Constructor"<<endl;
}

void RPCGeometryValidate::analyze(const edm::Event& event, const edm::EventSetup& eventSetup) {

  //cout<<"MYDEBUG: Start Analyze 1"<<endl;
  //edm::ESHandle<RPCGeometry> rpcGeometry;
  //cout<<"MYDEBUG: Start Analyze 2"<<endl;
  eventSetup.get<MuonGeometryRecord>().get(rpcGeometry_);
  //cout<<"MYDEBUG: Start Analyze 3"<<endl;  
  //rpcGeometry_ = eventSetup.getHandle(rpcGeometryToken_);
  // cout<<"MYDEBUG: after eventSetup.getHandle(rpcGeometryToken_)"<<endl;
  if (rpcGeometry_.isValid()) {
    LogVerbatim("RPCGeometry") << "Validating RPC chamber geometry";
    cout<<"MYDEBUG: Calling validateRPCChamberGeometry"<<endl;   
    validateRPCChamberGeometry();
    cout<<"MYDEBUG: after called validateRPCChamberGeometry"<<endl;   
    // LogVerbatim("RPCGeometry") << "Validating RPC roll geometry";
    validateRPCStripsGeometry();
  } else
    LogVerbatim("RPCGeometry") << "Invalid RPC geometry";
  cout<<"MYDEBUG: Invalid RPC geometry"<<endl;
}

void RPCGeometryValidate::validateRPCChamberGeometry() {
  
  clearData();

  for (auto const& it : rpcGeometry_->rolls()) {// it was chambers but it does't work
    RPCDetId chId = it->id();
    GlobalPoint gp = it->surface().toGlobal(LocalPoint(0.0, 0.0, 0.0));

    const TGeoMatrix* matrix = fwGeometry_.getMatrix(chId.rawId());
    //matrix->Print();

    if (!matrix) {
      LogVerbatim("RPCGeometry") << "Failed to get matrix of RPC chamber with detid: " << chId.rawId();
      cout<<"MYDEBUG: Failed to get matrix of RPC chamber with detid: "<<chId.rawId()<<endl;
      continue;
    }
    // cout<<"MYDEBUG: before compareTrasform"<<endl
    // cout<<"MYVALIDATE detid: "<<chId.rawId()<<endl;;
    compareTransform(gp, matrix);

    auto const& shape = fwGeometry_.getShapePars(chId.rawId());

    if (!shape) {
      LogVerbatim("RPCGeometry") << "Failed to get shape of RPC chamber with detid: " << chId.rawId();
      cout<<"MYDEBUG: Failed to get shape of RPC chamber with detid: "<<chId.rawId()<<endl;
      continue;
    }
    // cout<<"MYDEBUG: before compareShape"<<endl;
    // cout<<"MYVALIDATE: DetId, Shape (0,1,2,3,4): "<<chId.rawId()<<" "<<shape[0]<<" "<<shape[1]<<" "<<shape[2]<<" "<<shape[3]<<" "<<shape[4]<<endl;//it was OK the same values for DD and DD4HEP
    compareShape(it, shape);
  }
  //cout<<"MYDEBUG: before makeHistograms"<<endl;
  makeHistograms("RPC Chamber");
}


 // TO BE IMPLEMENTED
void RPCGeometryValidate::validateRPCStripsGeometry() {

  clearData2();

  for (auto const& it : rpcGeometry_->rolls()) {
    RPCDetId chId = it->id();
    const int n_strips = it->nstrips();
    const float n_pitch = it->pitch();
    const StripTopology& topo = it->specificTopology();
    const float stripLen = topo.stripLength();
   cout<<"MYVALIDATE From Geo, detid: "<<chId.rawId()<<" nstrips: "<<n_strips<<" n pitch: "<<n_pitch<<" stripLen: "<<stripLen<<endl; //OK for DDand DD4HEP

    const float* parameters = fwGeometry_.getParameters(chId.rawId());

    cout<<"MYVALIDATE From Reco, detid: "<<chId.rawId()<<" nstrips: "<<parameters[0]<<" n pitch: "<<parameters[2]<<" stripLen: "<<parameters[1]<<endl; 


    cout<<"MYVALIDATE (fabs(n_strips - parameters[0])) "<<fabs(n_strips - parameters[0])<<endl;
    cout<<"MYVALIDATE (fabs(n_pitch - parameters[2])) "<<fabs(n_pitch - parameters[2])<<endl;
    cout<<"MYVALIDATE (fabs(stripLen - parameters[1])) "<<fabs(stripLen - parameters[1])<<endl;

      
 if (n_strips)  {//OK for DDand DD4HEP
   cout<<"MYVALIDATE detid: "<<chId.rawId()<<" nstrips: "<<n_strips<<endl;;
      for (int istrips = 1; istrips <= n_strips; istrips++) {
	GlobalPoint gp = it->surface().toGlobal(it->centreOfStrip(istrips));
  cout<<"MYVALIDATE detId "<<chId.rawId()<<" nstrips: "<<n_strips<<" n pitch: "<<n_pitch<<" stripLen: "<<stripLen<<" gp "<<gp.x()<<" "<<gp.y()<<" "<<gp.z()<<endl;
	cout<<"MYVALIDATE  gp "<<gp.x()<<" "<<gp.y()<<" "<<gp.z()<<endl;
	nstrips_.push_back(fabs(n_strips - parameters[0]));
	pitch_.push_back(fabs(n_pitch - parameters[2]));
	stripslen_.push_back(fabs(stripLen - parameters[1]));
      } 
    } else {cout<<"MYVALIDATE ATTENTION nStrips = "<<n_strips<<endl; }
   
   
  }
  

  makeHistograms2("RPC Strips");
}


void RPCGeometryValidate::compareTransform(const GlobalPoint& gp, const TGeoMatrix* matrix) {
  //cout<<"MYDEBUG: Compare Transform"<<endl;
  double local[3] = {0.0, 0.0, 0.0};
  double global[3];

  matrix->LocalToMaster(local, global);

  float distance = getDistance(GlobalPoint(global[0], global[1], global[2]), gp);
  if((distance >= 0.0) && (distance < 1.0e-7)) distance = 0.0;// set a tollerance for the distance
  globalDistances_.push_back(distance);
  
  float glo_x = global[0];
  float glo_y = global[1];
  float glo_z = global[2];
  float gp_x = gp.x();
  float gp_y = gp.y();
  float gp_z = gp.z();
 
  if((glo_x > -1.0e-7) && (glo_x < 1.0e-7)) glo_x = 0.0;
  if((glo_y > -1.0e-7) && (glo_y < 1.0e-7)) glo_y = 0.0;
  if((glo_z > -1.0e-7) && (glo_z < 1.0e-7)) glo_z = 0.0;
  if((gp_x > -1.0e-7) && (gp_x < 1.0e-7)) gp_x = 0.0;
  if((gp_y > -1.0e-7) && (gp_y < 1.0e-7)) gp_y = 0.0;
  if((gp_z > -1.0e-7) && (gp_z < 1.0e-7)) gp_z = 0.0;
  if((distance >= 0.0) && (distance < 1.0e-7)) distance = 0.0;
 
  cout<<"MYVALIDATE distance: "<<distance<<", Pos: "<<glo_x<<" "<<glo_y<<" "<<glo_z<<", Pos Reco: "<<gp_x<<" "<<gp_y<<" "<<gp_z<<endl;;
  
}

void RPCGeometryValidate::compareShape(const GeomDet* det, const float* shape) {
  //cout<<"MYDEBUG: Compare Shape"<<endl;
  float shapeTopWidth;
  float shapeBottomWidth;
  float shapeLength;
  float shapeThickness;

  if (shape[0] == 1) {
    shapeTopWidth = shape[2];
    shapeBottomWidth = shape[1];
    shapeLength = shape[4];
    shapeThickness = shape[3];
  } else if (shape[0] == 2) {
    shapeTopWidth = shape[1];
    shapeBottomWidth = shape[1];
    shapeLength = shape[2];
    shapeThickness = shape[3];
  } else {
    LogVerbatim("RPCGeometry") << "Failed to get box or trapezoid from shape";
    cout<<"MYDEBUG: Failed to get box or trapezoid from shape"<<endl;
    return;
  }

  float topWidth, bottomWidth;
  float length, thickness;

  const Bounds* bounds = &(det->surface().bounds());
  //  cout<<"MYDEBUG: Before the first dynamic_cast"<<endl;
  if (const TrapezoidalPlaneBounds* tpbs = dynamic_cast<const TrapezoidalPlaneBounds*>(bounds)) {
    array<const float, 4> const& ps = tpbs->parameters();

    assert(ps.size() == 4);

    bottomWidth = ps[0];
    topWidth = ps[1];
    thickness = ps[2];
    length = ps[3];
   cout<<"MYVALIDATE: Trap: "<<bottomWidth<<" "<<topWidth<<" "<<thickness<<" "<<length<<endl;
  } else if ((dynamic_cast<const RectangularPlaneBounds*>(bounds))) {
    length = det->surface().bounds().length() * 0.5;
    topWidth = det->surface().bounds().width() * 0.5;
    bottomWidth = topWidth;
    thickness = det->surface().bounds().thickness() * 0.5;
    cout<<"MYVALIDATE: Rect: "<<bottomWidth<<" "<<topWidth<<" "<<thickness<<" "<<length<<endl;
  } else {
    LogVerbatim("RPCGeometry") << "Failed to get bounds";
    cout<<"MYDEBUG: Failed to get bounds"<<endl;
    return;
  }
  topWidths_.push_back(fabs(shapeTopWidth - topWidth));
  bottomWidths_.push_back(fabs(shapeBottomWidth - bottomWidth));
  lengths_.push_back(fabs(shapeLength - length));
  thicknesses_.push_back(fabs(shapeThickness - thickness));
}

float RPCGeometryValidate::getDistance(const GlobalPoint& p1, const GlobalPoint& p2) {
  return sqrt((p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()) +
              (p1.z() - p2.z()) * (p1.z() - p2.z()));
}

float RPCGeometryValidate::getDiff(const float val1, const float val2) {
  if (almost_equal(val1, val2, tolerance_))
    return 0.0f;
  else
    return (val1 - val2);
}

void RPCGeometryValidate::makeHistograms(const char* detector) {
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

void RPCGeometryValidate::makeHistograms2(const char* detector) {
  outFile_->cd();

  string d(detector);

  string ns = d + ": absolute difference between nStrips";
  makeHistogram(ns, nstrips_);

  string pi = d + ": absolute difference between Strips Pitch";
  makeHistogram(pi, pitch_);

  string pl = d + ": absolute difference between Strips Length";
  makeHistogram(pl, stripslen_);
}


void RPCGeometryValidate::makeHistogram(const string& name, vector<float>& data) {
  if (data.empty())
    return;

  const auto [minE, maxE] = minmax_element(begin(data), end(data));

  TH1D hist(name.c_str(), name.c_str(), 100, *minE * (1 + 0.10), *maxE * (1 + 0.10));

  for (auto const& it : data)
    hist.Fill(it);

  hist.GetXaxis()->SetTitle("[cm]");
  hist.Write();
}

void RPCGeometryValidate::beginJob() {
 outFile_->cd(); 

}

void RPCGeometryValidate::endJob() {
  LogVerbatim("RPCGeometry") << "Done.";
  LogVerbatim("RPCGeometry") << "Results written to " << outfileName_;
  outFile_->Close();
  cout<<"MYDEBUG: That's all folks!"<<endl;
}

DEFINE_FWK_MODULE(RPCGeometryValidate);
