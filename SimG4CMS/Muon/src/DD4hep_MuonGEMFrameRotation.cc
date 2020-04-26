#include "SimG4CMS/Muon/interface/DD4hep_MuonGEMFrameRotation.h"
#include "SimG4CMS/Muon/interface/DD4hep_MuonG4Numbering.h"

#include "G4Step.hh"

using namespace cms;

MuonGEMFrameRotation::MuonGEMFrameRotation(const MuonConstants&) : MuonFrameRotation::MuonFrameRotation() {}

MuonGEMFrameRotation::~MuonGEMFrameRotation() {}

Local3DPoint MuonGEMFrameRotation::transformPoint(const Local3DPoint& point, const G4Step*) const {
  return Local3DPoint(point.x(), point.z(), -point.y());
}
