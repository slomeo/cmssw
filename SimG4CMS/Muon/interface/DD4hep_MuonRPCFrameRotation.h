#ifndef SIMG4CMS_MUON_MUONRPCFRAMEROTATION_H
#define SIMG4CMS_MUON_MUONRPCFRAMEROTATION_H

/** \class MuonRPCFrameRotation for DD4HEP
 *
 * \author Sergio Lo Meo (sergio.lo.meo@cern.ch)
 * Date: 26 April 2020
 * Original author of the DD version: Arno Straessner, CERN <arno.straessner@cern.ch>
 * 
 *
 */

#include "SimG4CMS/Muon/interface/MuonFrameRotation.h"
#include "SimG4CMS/Muon/interface/DD4hep_MuonG4Numbering.h"
#include "G4Step.hh"

namespace cms {
class MuonG4Numbering;
 
 class MuonRPCFrameRotation : public MuonFrameRotation {
 public:
   MuonRPCFrameRotation(const MuonConstants& constants);
   ~MuonRPCFrameRotation() override;
   Local3DPoint transformPoint(const Local3DPoint&, const G4Step*) const override;
   
 private:
   const int get(const char*, const MuonConstants&) const;
   MuonG4Numbering* g4numbering;
   int theRegion;
 };
}  // namespace cms
#endif
