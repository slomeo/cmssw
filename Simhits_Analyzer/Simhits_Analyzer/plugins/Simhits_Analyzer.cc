// -*- C++ -*-
//
// Package:    Simhits_Analyzer/Simhits_Analyzer
// Class:      Simhits_Analyzer
//
/**\class Simhits_Analyzer Simhits_Analyzer.cc Simhits_Analyzer/Simhits_Analyzer/plugins/Simhits_Analyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Sergio Lo Meo
//         Created:  Mon, 16 Nov 2020 14:03:21 GMT
//
//

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/Service.h"


#include "DataFormats/HepMCCandidate/interface/GenParticle.h"


#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "Geometry/CommonDetUnit/interface/GeomDet.h"
#include "Geometry/DTGeometry/interface/DTGeometry.h"
#include "Geometry/CSCGeometry/interface/CSCGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeometry.h"
#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"
#include <Geometry/RPCGeometry/interface/RPCRoll.h>
#include "Geometry/GEMGeometry/interface/GEMGeometry.h"
#include "Geometry/GEMGeometry/interface/ME0Geometry.h"
#include <Geometry/CommonTopologies/interface/RectangularStripTopology.h>
#include <Geometry/CommonTopologies/interface/TrapezoidalStripTopology.h>
#include "Geometry/Records/interface/MuonGeometryRecord.h"
#include "Geometry/DTGeometry/interface/DTLayer.h"
#include "Geometry/DTGeometry/interface/DTSuperLayer.h"
#include <Geometry/CSCGeometry/interface/CSCLayer.h>
#include "Geometry/CommonDetUnit/interface/GlobalTrackingGeometry.h"

//--
#include "DataFormats/GeometrySurface/interface/Surface.h"
#include <DataFormats/GeometrySurface/interface/LocalError.h>
#include <DataFormats/GeometryVector/interface/LocalPoint.h>
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/DetId/interface/DetId.h"
#include "DataFormats/MuonDetId/interface/MuonSubdetId.h"
#include <DataFormats/MuonDetId/interface/RPCDetId.h>
#include <DataFormats/MuonDetId/interface/CSCDetId.h>
#include "DataFormats/MuonDetId/interface/DTWireId.h"
#include "DataFormats/MuonDetId/interface/DTLayerId.h"
#include "DataFormats/MuonDetId/interface/DTSuperLayerId.h"
#include "DataFormats/MuonDetId/interface/DTChamberId.h"
#include <DataFormats/MuonDetId/interface/GEMDetId.h>
#include <DataFormats/MuonDetId/interface/ME0DetId.h>
#include "DataFormats/RPCDigi/interface/RPCDigi.h"
#include "DataFormats/RPCDigi/interface/RPCDigiCollection.h"
#include <DataFormats/RPCRecHit/interface/RPCRecHit.h>
#include "DataFormats/RPCRecHit/interface/RPCRecHitCollection.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/Candidate/interface/CandMatchMap.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"
#include "DataFormats/TrajectorySeed/interface/TrajectorySeedCollection.h"
#include "DataFormats/Math/interface/LorentzVectorFwd.h"
#include "DataFormats/TrackCandidate/interface/TrackCandidate.h"
#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/DTRecHit/interface/DTSLRecSegment2D.h"
#include "DataFormats/MuonReco/interface/MuonCocktails.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtra.h"
#include "DataFormats/MuonReco/interface/MuonTimeExtraMap.h"
#include <DataFormats/CSCRecHit/interface/CSCRecHit2D.h>
#include <DataFormats/CSCRecHit/interface/CSCRangeMapAccessor.h>
#include "DataFormats/TrackingRecHit/interface/TrackingRecHitFwd.h"
#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"
//
#include "SimDataFormats/Track/interface/SimTrack.h"
#include "SimDataFormats/Track/interface/SimTrackContainer.h"
#include "SimDataFormats/Vertex/interface/SimVertex.h"
#include "SimDataFormats/Vertex/interface/SimVertexContainer.h"
#include "SimDataFormats/TrackingHit/interface/PSimHitContainer.h"
//
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/GeomPropagators/interface/Propagator.h"
#include "TrackingTools/GeomPropagators/interface/AnalyticalPropagator.h"
#include "TrackPropagation/SteppingHelixPropagator/interface/SteppingHelixPropagator.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajTrackAssociation.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
//
#include "RecoMuon/TrackingTools/interface/MuonPatternRecoDumper.h"
#include "RecoMuon/TrackingTools/interface/MuonServiceProxy.h"
#include "RecoLocalMuon/DTSegment/src/DTSegmentUpdator.h"
#include "RecoLocalMuon/DTSegment/src/DTSegmentCleaner.h"
#include "RecoLocalMuon/DTSegment/src/DTHitPairForFit.h"
#include "RecoLocalMuon/DTSegment/src/DTSegmentCand.h"
#include "RecoVertex/KalmanVertexFit/interface/KalmanVertexFitter.h"
//--

#include "Geometry/CaloGeometry/interface/CaloSubdetectorGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloGeometry.h"
#include "Geometry/CaloGeometry/interface/CaloCellGeometry.h"
#include "Geometry/CaloGeometry/interface/TruncatedPyramid.h"
#include "Geometry/EcalAlgo/interface/EcalPreshowerGeometry.h"
#include "Geometry/CaloTopology/interface/EcalPreshowerTopology.h"
#include "Geometry/Records/interface/CaloGeometryRecord.h"
#include "Geometry/CaloEventSetup/interface/CaloTopologyRecord.h"
#include "Geometry/CaloTopology/interface/CaloTopology.h"


#include "SimDataFormats/CaloHit/interface/PCaloHit.h"
#include "SimDataFormats/CaloHit/interface/PCaloHitContainer.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDetId/interface/ESDetId.h"
#include "DataFormats/EcalDetId/interface/EEDetId.h"


#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "TTree.h"
#include "TH1.h"
#include "TH2F.h"
#include "TCanvas.h"
#include <iostream>
#include <fstream>


#include <string>
#include <sstream>

#include <iomanip>
#include <math.h>


#include <TRandom.h>
#include "TROOT.h"
#include "TStyle.h"
#include "TFile.h"
#include "TH1F.h"
#include "THStack.h"
#include "TLegend.h"
#include "TTree.h"
#include "TDirectoryFile.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TLatex.h"
#include "TNtuple.h"
#include "TNtupleD.h"
#include <TSystem.h>
#include <TStyle.h>
#include <TFrame.h>
#include "TLorentzVector.h"
#include "TMath.h"


//
// class declaration
//

class Simhits_Analyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit Simhits_Analyzer(const edm::ParameterSet&);
  ~Simhits_Analyzer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // Member Data

  // only mu- mu+
  TH1F* Z_DTHits_Muon;
  TH2F* XY_DTHits_Muon;
  TH2F* ZR_DTHits_Muon;
  TH2F* Wheel_Minus2_XY_DTHits_Muon;
  TH2F* Wheel_Minus1_XY_DTHits_Muon;
  TH2F* Wheel_0_XY_DTHits_Muon;
  TH2F* Wheel_1_XY_DTHits_Muon;
  TH2F* Wheel_2_XY_DTHits_Muon;
  // all particles
  TH1F* Z_DTHits_AllParticles;
  TH2F* XY_DTHits_AllParticles;
  TH2F* Wheel_Minus2_XY_DTHits_AllParticles;
  TH2F* Wheel_Minus1_XY_DTHits_AllParticles;
  TH2F* Wheel_0_XY_DTHits_AllParticles;
  TH2F* Wheel_1_XY_DTHits_AllParticles;
  TH2F* Wheel_2_XY_DTHits_AllParticles;

  Long64_t run, event, lumi;

  edm::Handle<edm::View<reco::GenParticle> > particle;
  edm::EDGetTokenT<edm::View< reco::GenParticle > > particleToken;
 
  // parte nuova per i Muoni
   edm::Handle<edm::PSimHitContainer> theDTSimHitHandle;
  edm::EDGetTokenT<edm::PSimHitContainer> theDTSimHitToken;
  // fine parte nuova per i Muoni

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  edm::ESGetToken<SetupData, SetupRecord> setupToken_;
#endif
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
Simhits_Analyzer::Simhits_Analyzer(const edm::ParameterSet& iConfig){
  consumesMany<edm::PSimHitContainer>();
  usesResource("TFileService");
  particleToken = consumes< edm::View < reco::GenParticle> >(edm::InputTag("genParticles"));
  theDTSimHitToken = consumes<edm::PSimHitContainer>(edm::InputTag("g4SimHits", "MuonDTHits", "SIM"));
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  setupDataToken_ = esConsumes<SetupData, SetupRecord>();
#endif
 
}

Simhits_Analyzer::~Simhits_Analyzer() {
 
}

//
// member functions
//

// ------------ method called for each event  ------------
void Simhits_Analyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  using namespace edm;
  using namespace std;
  using namespace reco;

  run = iEvent.id().run();
  event = iEvent.id().event();
  lumi = iEvent.id().luminosityBlock();

  iEvent.getByToken(particleToken, particle);  
  
  std::vector<edm::Handle<edm::PSimHitContainer> > theSimHitContainers;
  iEvent.getManyByType(theSimHitContainers); 

  std::vector<PSimHit> theSimHits;
  for (int i = 0; i < int(theSimHitContainers.size()); ++i) {
    theSimHits.insert(theSimHits.end(),theSimHitContainers.at(i)->begin(),theSimHitContainers.at(i)->end());
  }
  // DT
  iEvent.getByToken(theDTSimHitToken, theDTSimHitHandle); 
  ESHandle<DTGeometry> dtGeometry;
  iSetup.get<MuonGeometryRecord>().get(dtGeometry); 
  const DTGeometry* dtgeo = dtGeometry.product(); 

  // SimiHits +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  for (std::vector<PSimHit>::const_iterator iHit = theSimHits.begin(); iHit != theSimHits.end(); ++iHit) {

     int pid            = (*iHit).particleType();
     DetId theDetUnitId((*iHit).detUnitId());
     DetId simdetid= DetId((*iHit).detUnitId());

     // DT Sim Hits ------------------------------------------------------------------
     if(simdetid.det()==DetId::Muon &&  simdetid.subdetId()== MuonSubdetId::DT){
     
     DTWireId wireId(theDetUnitId);

     GlobalPoint DTGlobalPoint = dtgeo->idToDet(wireId)->toGlobal((*iHit).localPosition());
     double DT_GlobalPoint_R = sqrt(pow(DTGlobalPoint.x(),2)+pow(DTGlobalPoint.y(),2));     

     DTLayerId myDTLayerId = wireId.layerId();
     DTSuperLayerId myDTSuperLayerId = myDTLayerId.superlayerId();
     DTChamberId myDTChamberId = myDTSuperLayerId.chamberId();
     // all particles
     Z_DTHits_AllParticles->Fill(DTGlobalPoint.z());
     XY_DTHits_AllParticles->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
     if(myDTChamberId.wheel() == -2) Wheel_Minus2_XY_DTHits_AllParticles->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
     if(myDTChamberId.wheel() == -1) Wheel_Minus1_XY_DTHits_AllParticles->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
     if(myDTChamberId.wheel() == 0) Wheel_0_XY_DTHits_AllParticles->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
     if(myDTChamberId.wheel() == 1) Wheel_1_XY_DTHits_AllParticles->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
     if(myDTChamberId.wheel() == 2) Wheel_2_XY_DTHits_AllParticles->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());

     // only mu- mu+
     if((pid==13) || (pid==-13))
       {
	 cout<<"PID: "<<pid<<" Muon Hit in: "<<endl;
	 cout<<" Wheel Id: "<<myDTChamberId.wheel()<<" Station Id: "<<myDTChamberId.station()<<" Sector Id: "<<myDTChamberId.sector()<<endl;
	 cout<<" SuperLayer Id: "<<myDTSuperLayerId.superlayer()<<" Layer Id: "<<myDTLayerId.layer()<<" Wire ID: "<<wireId.wire()<<endl;
	 cout<<" R: "<<DT_GlobalPoint_R<<" x: "<<DTGlobalPoint.x()<<" y: "<<DTGlobalPoint.y()<<" z: "<<DTGlobalPoint.z()<<endl;

	 Z_DTHits_Muon->Fill(DTGlobalPoint.z());
	 XY_DTHits_Muon->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
	 ZR_DTHits_Muon->Fill(DTGlobalPoint.z(),DT_GlobalPoint_R);
	 if(myDTChamberId.wheel() == -2) Wheel_Minus2_XY_DTHits_Muon->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
	 if(myDTChamberId.wheel() == -1) Wheel_Minus1_XY_DTHits_Muon->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
	 if(myDTChamberId.wheel() == 0) Wheel_0_XY_DTHits_Muon->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
	 if(myDTChamberId.wheel() == 1) Wheel_1_XY_DTHits_Muon->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
	 if(myDTChamberId.wheel() == 2) Wheel_2_XY_DTHits_Muon->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
       } // end only mu- mu+

     }// end DT Sim Hits -------------------------------------------------------------

  }// End SimHits +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
 
  for (auto &p : *particle){
    // if((p.pdgId()==13)||(p.pdgId()==-13)) cout<<endl<<" PDG Id = "<<p.pdgId()<<" Energy  = "<<p.energy()<<" Eta = "<<p.eta()<<" Phi = "<<p.phi()<<endl;
  }


#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   auto setup = iSetup.getData(setupToken_);
   auto pSetup = iSetup.getHandle(setupToken_);
#endif
}

// ------------ method called once each job just before starting event loop  ------------
void Simhits_Analyzer::beginJob() {
 
  edm::Service<TFileService> fs;
  Z_DTHits_Muon = fs->make<TH1F>("Z_DTHits_Muon","Z_DTHits_Muon",2000,-1000, 1000); 
  XY_DTHits_Muon = fs->make<TH2F>("XY_DTHits_Muon","XY_DTHits_Muon",2000,-1000, 1000, 2000,-1000, 1000); 
  ZR_DTHits_Muon = fs->make<TH2F>("ZR_DTHits_Muon","ZR_DTHits_Muon",2000,-1000, 1000, 2000, 0, 1000); 
  Wheel_Minus2_XY_DTHits_Muon  = fs->make<TH2F>("Wheel_Minus2_XY_DTHits_Muon","Wheel_Minus2_XY_DTHits_Muon",2000,-1000, 1000, 2000,-1000, 1000); 
  Wheel_Minus1_XY_DTHits_Muon  = fs->make<TH2F>("Wheel_Minus1_XY_DTHits_Muon","Wheel_Minus1_XY_DTHits_Muon",2000,-1000, 1000, 2000,-1000, 1000); 
  Wheel_0_XY_DTHits_Muon  = fs->make<TH2F>("Wheel_0_XY_DTHits_Muon","Wheel_0_XY_DTHits_Muon",2000,-1000, 1000, 2000,-1000, 1000); 
  Wheel_1_XY_DTHits_Muon  = fs->make<TH2F>("Wheel_1_XY_DTHits_Muon","Wheel_1_XY_DTHits_Muon",2000,-1000, 1000, 2000,-1000, 1000); 
  Wheel_2_XY_DTHits_Muon  = fs->make<TH2F>("Wheel_2_XY_DTHits_Muon","Wheel_2_XY_DTHits_Muon",2000,-1000, 1000, 2000,-1000, 1000);

  Z_DTHits_AllParticles = fs->make<TH1F>("Z_DTHits_AllParticles","Z_DTHits_AllParticles",2000,-1000, 1000); 
  XY_DTHits_AllParticles = fs->make<TH2F>("XY_DTHits_AllParticles","XY_DTHits_AllParticles",2000,-1000, 1000, 2000,-1000, 1000); 
  Wheel_Minus2_XY_DTHits_AllParticles  = fs->make<TH2F>("Wheel_Minus2_XY_DTHits_AllParticles","Wheel_Minus2_XY_DTHits_AllParticles",2000,-1000, 1000, 2000,-1000, 1000); 
  Wheel_Minus1_XY_DTHits_AllParticles  = fs->make<TH2F>("Wheel_Minus1_XY_DTHits_AllParticles","Wheel_Minus1_XY_DTHits_AllParticles",2000,-1000, 1000, 2000,-1000, 1000); 
  Wheel_0_XY_DTHits_AllParticles  = fs->make<TH2F>("Wheel_0_XY_DTHits_AllParticles","Wheel_0_XY_DTHits_AllParticles",2000,-1000, 1000, 2000,-1000, 1000); 
  Wheel_1_XY_DTHits_AllParticles  = fs->make<TH2F>("Wheel_1_XY_DTHits_AllParticles","Wheel_1_XY_DTHits_AllParticles",2000,-1000, 1000, 2000,-1000, 1000); 
  Wheel_2_XY_DTHits_AllParticles  = fs->make<TH2F>("Wheel_2_XY_DTHits_AllParticles","Wheel_2_XY_DTHits_AllParticles",2000,-1000, 1000, 2000,-1000, 1000);
 
}

// ------------ method called once each job just after ending the event loop  ------------
void Simhits_Analyzer::endJob() {
 
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void Simhits_Analyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
 
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

DEFINE_FWK_MODULE(Simhits_Analyzer);
