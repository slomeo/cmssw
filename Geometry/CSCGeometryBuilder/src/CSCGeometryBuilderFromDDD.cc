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
  
  /* // delete comment after modifications in CSCGeometryParsFromDD
  CSCGeometryParsFromDD cscp;
  if (!cscp.build(cview, muonConstants, rig, rdp)) {
    throw cms::Exception("CSCGeometryBuilderFromDDD", "Failed to build the necessary objects from the DDD");
  }
 */
  CSCGeometryBuilder realbuilder;
  realbuilder.build(geom, rig, rdp);
 
}
