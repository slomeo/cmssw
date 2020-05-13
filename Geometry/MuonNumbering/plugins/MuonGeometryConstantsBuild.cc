#include "Geometry/MuonNumbering/plugins/MuonGeometryConstantsBuild.h"
#include "Geometry/MuonNumbering/interface/MuonGeometryConstants.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Utilities/interface/Exception.h"

#include "DetectorDescription/Core/interface/DDValue.h"
#include "DetectorDescription/Core/interface/DDFilteredView.h"
#include "DetectorDescription/DDCMS/interface/DDFilteredView.h"

#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"

//#define EDM_ML_DEBUG

bool MuonGeometryConstantsBuild::build(const DDCompactView* cpv, MuonGeometryConstants& php) {
#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("Geometry")
      << "MuonGeometryConstantsBuild;:build (const DDCompactView* cpv, MuonGeometryConstants& php)";
#endif
  std::string attribute = "OnlyForMuonNumbering";

  DDSpecificsHasNamedValueFilter filter(attribute);
  DDFilteredView fview((*cpv), filter);

  DDValue val2("level");
  const DDsvalues_type params(fview.mergedSpecifics());

  fview.firstChild();

  const DDsvalues_type mySpecs(fview.mergedSpecifics());
#ifdef EDM_ML_DEBUG
  edm::LogVerbatim("Geometry") << "MuonGeometryConstantsBuild::mySpecs.size() = " << mySpecs.size();
#endif
  if (mySpecs.size() < 25) {
    edm::LogError("MuonGeometryConstantsBuild")
        << " MuonGeometryConstantsBuild: Missing SpecPars from DetectorDescription.";
    std::string msg =
        "MuonGeometryConstantsBuild does not have the appropriate number of SpecPars associated with the part //MUON.";
    throw cms::Exception("GeometryBuildFailure", msg);
  }

  DDsvalues_type::const_iterator bit = mySpecs.begin();
  DDsvalues_type::const_iterator eit = mySpecs.end();
  for (; bit != eit; ++bit) {
    if (bit->second.isEvaluated()) {
      php.addValue(bit->second.name(), int(bit->second.doubles()[0]));
#ifdef EDM_ML_DEBUG
      edm::LogVerbatim("Geometry") << "MuonGeometryConstantsBuild::adding DDConstant of " << bit->second.name() << " = "
                                   << int(bit->second.doubles()[0]);
#endif
    }
  }
  return true;
}

// dd4hep

bool MuonGeometryConstantsBuild::build(const cms::DDCompactView* cpv, MuonGeometryConstants& php) {

  std::string attribute = "OnlyForMuonNumbering";
  std::string value = "level";
  cms::DDFilteredView fv(cpv->detector(), cpv->detector()->worldVolume());
  cms::DDSpecParRefs refs;
  const cms::DDSpecParRegistry& mypar = cpv->specpars();
  mypar.filter(refs, attribute, value);
  fv.mergedSpecifics(refs);
  
  fv.firstChild();
  
  return false;
}

