/** \class MuonG4Numbering for DD4HEP migration
 *
 * \author Sergio Lo Meo (sergio.lo.meo@cern.ch)
 * \date 23 April 2020
 * Old DD version: original author Arno Straessner, CERN <arno.straessner@cern.ch>
 *
 * 
 */
#include "SimG4CMS/Muon/interface/DD4hep_MuonG4Numbering.h"
#include "Geometry/MuonNumbering/interface/MuonBaseNumber.h"
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "G4Step.hh"

#include <iostream>
#include <cassert>

using namespace cms;
//#define LOCAL_DEBUG


MuonG4Numbering::MuonG4Numbering(const MuonConstants& muonConstants) {

  int theLevelPart = get("level", muonConstants);
  assert(theLevelPart != 0);
  theSuperPart = get("super", muonConstants);
  theBasePart = get("base", muonConstants);
  theStartCopyNo = get("xml_starts_with_copyno", muonConstants);

  // some consistency checks

  if (theBasePart != 1) {
    std::cout << "MuonNumbering finds unusual base constant:" << theBasePart << std::endl;
  }
  if (theSuperPart < 100) {
    std::cout << "MuonNumbering finds unusual super constant:" << theSuperPart << std::endl;
  }
  if (theLevelPart < 10 * theSuperPart) {
    std::cout << "MuonNumbering finds unusual level constant:" << theLevelPart << std::endl;
  }
  if ((theStartCopyNo != 0) && (theStartCopyNo != 1)) {
    std::cout << "MuonNumbering finds unusual start value for copy numbers:" << theStartCopyNo << std::endl;
  }

#ifdef LOCAL_DEBUG
  std::cout << "StartCopyNo = " << theStartCopyNo << std::endl;
  std::cout << "MuonG4Numbering configured with" << std::endl;
  std::cout << "Level = " << theLevelPart << " ";
  std::cout << "Super = " << theSuperPart << " ";
  std::cout << "Base = " << theBasePart << " ";
  std::cout << "StartCopyNo = " << theStartCopyNo << std::endl;
#endif
}

MuonBaseNumber MuonG4Numbering::PhysicalVolumeToBaseNumber(const G4Step* aStep) {
  MuonBaseNumber num;
  const G4VTouchable* touch = aStep->GetPreStepPoint()->GetTouchable();

  for (int ii = 0; ii < touch->GetHistoryDepth(); ii++) {
    G4VPhysicalVolume* vol = touch->GetVolume(ii);
    int copyno = vol->GetCopyNo();
#ifdef LOCAL_DEBUG
    std::cout << "MuonG4Numbering: " << vol->GetName() << " " << copyno << std::endl
              << "Split " << copyNoRelevant(copyno) << ":" << theLevelPart << ":" << theSuperPart << " ";
#endif
    if (copyNoRelevant(copyno)) {
      num.addBase(getCopyNoLevel(copyno), getCopyNoSuperNo(copyno), getCopyNoBaseNo(copyno) - theStartCopyNo);
#ifdef LOCAL_DEBUG
      std::cout << " NoLevel " << getCopyNoLevel(copyno) << " Super " << getCopyNoSuperNo(copyno) << " Base "
                << getCopyNoBaseNo(copyno) << " Start " << theStartCopyNo;
#endif
    }
#ifdef LOCAL_DEBUG
    std::cout << std::endl;
#endif
  }

  return num;
}

const int MuonG4Numbering::getCopyNoLevel(const int copyno) { return copyno / theLevelPart; }

const int MuonG4Numbering::getCopyNoSuperNo(const int copyno) { return (copyno % theLevelPart) / theSuperPart; }

const int MuonG4Numbering::getCopyNoBaseNo(const int copyno) { return copyno % theSuperPart; }

const bool MuonG4Numbering::copyNoRelevant(const int copyno) { return (copyno / theLevelPart) > 0; }

const int MuonG4Numbering::get(const char* key, const MuonConstants& muonConstants) const {
  int result(0);
  auto const& it = (muonConstants.find(key));
  if (it != end(muonConstants))
    result = it->second;
  return result;
}
