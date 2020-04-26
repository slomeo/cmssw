#include "SimG4CMS/Muon/interface/DD4hep_MuonRPCFrameRotation.h"
#include "SimG4CMS/Muon/interface/DD4hep_MuonG4Numbering.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"

#include "G4Step.hh"

using namespace cms;

MuonRPCFrameRotation::MuonRPCFrameRotation(const MuonConstants& muonConstants)
    : MuonFrameRotation::MuonFrameRotation() {
  g4numbering = new MuonG4Numbering(muonConstants);
  int theLevelPart = get("level", muonConstants);
   theRegion = get("mr_region", muonConstants) / theLevelPart;
}

MuonRPCFrameRotation::~MuonRPCFrameRotation() { delete g4numbering; }

Local3DPoint MuonRPCFrameRotation::transformPoint(const Local3DPoint& point, const G4Step* aStep) const {
  //check if endcap
  MuonBaseNumber num = g4numbering->PhysicalVolumeToBaseNumber(aStep);
  bool endcap_muon = (num.getSuperNo(theRegion) != 1);
  return (endcap_muon) ? Local3DPoint(point.x(), point.z(), -point.y()) : Local3DPoint(point.x(), point.y(), point.z());
}

const int MuonRPCFrameRotation::get(const char* key, const MuonConstants& muonConstants) const {
  int result(0);
  auto const& it = (muonConstants.find(key));
  if (it != end(muonConstants))
    result = it->second;
  return result;
}
