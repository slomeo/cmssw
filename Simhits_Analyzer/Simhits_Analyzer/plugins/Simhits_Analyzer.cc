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

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


class Simhits_Analyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit Simhits_Analyzer(const edm::ParameterSet&);
  ~Simhits_Analyzer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  void beginJob() override;
  void analyze(const edm::Event&, const edm::EventSetup&) override;
  void endJob() override;

  // ----------member data ---------------------------
  /*
  TH1F* NHits_EB;
  TH2F* Hits_EB;
  TH1F* NHits_ES_Plus;
  TH2F* Hits_ES_Plus;
  TH1F* NHits_ES_Minus;
  TH2F* Hits_ES_Minus;
  TH1F* NHits_EE_Plus;
  TH2F* Hits_EE_Plus;
  TH1F* NHits_EE_Minus;
  TH2F* Hits_EE_Minus;   
  */

  TH2F* XY_Hits_MUON;

  Long64_t run, event, lumi;

  edm::Handle<edm::View<reco::GenParticle> > particle;
  edm::EDGetTokenT<edm::View< reco::GenParticle > > particleToken;
 
  /*
  edm::Handle<edm::PCaloHitContainer> pCaloHits_EB_Handle;
  edm::EDGetTokenT<edm::PCaloHitContainer> pCaloHits_EB_Token;

  edm::Handle<edm::PCaloHitContainer> pCaloHits_ES_Handle;
  edm::EDGetTokenT<edm::PCaloHitContainer> pCaloHits_ES_Token;
  
  edm::Handle<edm::PCaloHitContainer> pCaloHits_EE_Handle;
  edm::EDGetTokenT<edm::PCaloHitContainer> pCaloHits_EE_Token;  
  */
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
  consumesMany<edm::PSimHitContainer>();// questo viene dal MyAnalyzer5
  usesResource("TFileService");
  particleToken = consumes< edm::View < reco::GenParticle> >(edm::InputTag("genParticles"));
	/*
	pCaloHits_EB_Token = consumes<edm::PCaloHitContainer>(edm::InputTag("g4SimHits", "EcalHitsEB", "SIM"));
        pCaloHits_ES_Token = consumes<edm::PCaloHitContainer>(edm::InputTag("g4SimHits","EcalHitsES","SIM"));
        pCaloHits_EE_Token = consumes<edm::PCaloHitContainer>(edm::InputTag("g4SimHits", "EcalHitsEE", "SIM"));

	*/
   theDTSimHitToken = consumes<edm::PSimHitContainer>(edm::InputTag("g4SimHits", "MuonDTHits", "SIM"));// OK
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  setupDataToken_ = esConsumes<SetupData, SetupRecord>();
#endif
  //now do what ever initialization is needed
}

Simhits_Analyzer::~Simhits_Analyzer() {
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  //
  // please remove this method altogether if it would be left empty
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

  iEvent.getByToken(particleToken, particle);         //GenParticles
  /*
  iEvent.getByToken(pCaloHits_EB_Token, pCaloHits_EB_Handle); //PCaloHits EB
  iEvent.getByToken(pCaloHits_ES_Token, pCaloHits_ES_Handle); //PCaloHits ES
  iEvent.getByToken(pCaloHits_EE_Token, pCaloHits_EE_Handle); //PCaloHits EE
  */

  std::vector<edm::Handle<edm::PSimHitContainer> > theSimHitContainers;
  iEvent.getManyByType(theSimHitContainers); 

  std::vector<PSimHit> theSimHits;
  for (int i = 0; i < int(theSimHitContainers.size()); ++i) {
    theSimHits.insert(theSimHits.end(),theSimHitContainers.at(i)->begin(),theSimHitContainers.at(i)->end());
  }

 //OK 
  //new line for Muon
  iEvent.getByToken(theDTSimHitToken, theDTSimHitHandle); //DT Sim Hits
  ESHandle<DTGeometry> dtGeometry;
  iSetup.get<MuonGeometryRecord>().get(dtGeometry); // crea problemi...
  const DTGeometry* dtgeo = dtGeometry.product(); //forse non c'e' bisogno di questa linea

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

     if((pid==13) || (pid==-13))
       {
	 cout<<"PID: "<<pid<<" Muon Hit in: "<<endl;
	 cout<<" Wheel Id: "<<myDTChamberId.wheel()<<" Station Id: "<<myDTChamberId.station()<<" Sector Id: "<<myDTChamberId.sector()<<endl;
	 cout<<" SuperLayer Id: "<<myDTSuperLayerId.superlayer()<<" Layer Id: "<<myDTLayerId.layer()<<" Wire ID: "<<wireId.wire()<<endl;
	 cout<<" R: "<<DT_GlobalPoint_R<<" x: "<<DTGlobalPoint.x()<<" y: "<<DTGlobalPoint.y()<<" z: "<<DTGlobalPoint.z()<<endl;
	 XY_Hits_MUON->Fill(DTGlobalPoint.x(), DTGlobalPoint.y());
       }
     }// end DT Sim Hits -------------------------------------------------------------

  }// End SimHits +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  
  
  /*
  ESHandle<CaloGeometry> pG;
  iSetup.get<CaloGeometryRecord>().get(pG);
  const CaloGeometry* geo = pG.product();

  EBDetId* DidEB;
  ESDetId* DidES;
  EEDetId* DidEE;
 

  int NHits, NHits_Plus, NHits_Minus = 0;
  float Energy = 0.;

  edm::PCaloHitContainer::const_iterator caloHitsItr;

  const CaloSubdetectorGeometry* ecalEBGeom = static_cast<const CaloSubdetectorGeometry*>(geo->getSubdetectorGeometry(DetId::Ecal, EcalBarrel));

  
  for(caloHitsItr = pCaloHits_EB_Handle->begin(); caloHitsItr != pCaloHits_EB_Handle->end(); caloHitsItr++){
          int temp = caloHitsItr->depth();
          DidEB = new EBDetId(caloHitsItr->id());
          temp &= 0x0003;
          double depth = (double)(caloHitsItr->depth()>>2)/16383.;
	  NHits++;
          Energy += caloHitsItr->energy();
          Hits_EB->Fill(DidEB->ieta(), DidEB->iphi());
	  std::shared_ptr<const CaloCellGeometry> geom = ecalEBGeom->getGeometry(*DidEB);  
	  //	  Hits_EB_cartesian->Fill(geom->getPosition().x(), geom->getPosition().y());
//          cout << "EB ==> temp flag: " << temp << " || depth: " << depth << " || energy: " << caloHitsItr->energy() << endl << endl;
//          cout << endl << "iEta = "<< DidEB->ieta() << " iPhi = "<<DidEB->iphi()<<endl;
  }

  NHits_EB->Fill(NHits);
  cout<<endl<<"EB Total Hits = "<< NHits <<  "Total Energy = "<< Energy <<endl;


  const CaloSubdetectorGeometry* ecalESGeom = static_cast<const CaloSubdetectorGeometry*>(geo->getSubdetectorGeometry(DetId::Ecal, EcalPreshower));
  NHits_Plus = 0;
  NHits_Minus = 0;
  Energy = 0.;

  for(caloHitsItr = pCaloHits_ES_Handle->begin(); caloHitsItr != pCaloHits_ES_Handle->end(); caloHitsItr++){
          int temp = caloHitsItr->depth();
          DidES = new ESDetId(caloHitsItr->id());
          NHits++;
          Energy += caloHitsItr->energy();
	  std::shared_ptr<const CaloCellGeometry> geom = ecalESGeom->getGeometry(*DidES);
//	  cout<<endl<<geom->getPosition().x()<<" "<<geom->getPosition().y()<<endl;
	  if (DidES->zside() == 1){
          	Hits_ES_Plus->Fill(geom->getPosition().x(), geom->getPosition().y());
		NHits_Plus++;
	  }
	  if (DidES->zside() == -1){
                Hits_ES_Minus->Fill(geom->getPosition().x(), geom->getPosition().y());
		NHits_Minus++;
	  }

  }


  NHits_ES_Plus->Fill(NHits_Plus);
  NHits_ES_Minus->Fill(NHits_Minus);
  NHits_Plus = 0;
  NHits_Minus = 0;
  Energy = 0.;

  for(caloHitsItr = pCaloHits_EE_Handle->begin(); caloHitsItr != pCaloHits_EE_Handle->end(); caloHitsItr++){
          int temp = caloHitsItr->depth();
          DidEE = new EEDetId(caloHitsItr->id());
          temp &= 0x0003;
	  NHits++;
	  Energy += caloHitsItr->energy();
          double depth = (double)(caloHitsItr->depth()>>2)/16383.;
//          cout << "EE ==> temp flag: " << temp << " || depth: " << depth << " || energy: " << caloHitsItr->energy() << endl << endl;
//	  cout << endl << "iEta = "<< DidEE->ix() << " iPhi = "<<DidEE->iy()<<" "<<DidEE->zside()<<endl;
	  if (DidEE->zside() == 1){
		Hits_EE_Plus->Fill(DidEE->ix(), DidEE->iy());
		NHits_Plus++;
	  }
	  if (DidEE->zside() == -1){
                Hits_EE_Minus->Fill(DidEE->ix(), DidEE->iy());
		NHits_Minus++;
	  }
  }

  NHits_EE_Plus->Fill(NHits_Plus);
  NHits_EE_Minus->Fill(NHits_Minus);
  */

  for (auto &p : *particle){
    // if((p.pdgId()==13)||(p.pdgId()==-13)) cout<<endl<<" PDG Id = "<<p.pdgId()<<" Energy  = "<<p.energy()<<" Eta = "<<p.eta()<<" Phi = "<<p.phi()<<endl;
  }


#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
  // if the SetupData is always needed
  auto setup = iSetup.getData(setupToken_);
  // if need the ESHandle to check if the SetupData was there or not
  auto pSetup = iSetup.getHandle(setupToken_);
#endif
}

// ------------ method called once each job just before starting event loop  ------------
void Simhits_Analyzer::beginJob() {
 
  edm::Service<TFileService> fs;
  XY_Hits_MUON = fs->make<TH2F>("XY_Hits_MUON","XY_Hits_MUON",2000,-1000, 1000, 2000,-1000, 1000); 
// please remove this method if not needed
  /* 
 
  NHits_EB = fs->make<TH1F>("NHits_EB","NHits_EB",500,0, 500);
  Hits_EB = fs->make<TH2F>("Hits_EB","Hits_EB",400,-200, 200, 360,0, 360);

  NHits_ES_Plus = fs->make<TH1F>("NHits_ES_Plus","NHits_ES_Plus",500,0, 500);
 
  NHits_ES_Minus = fs->make<TH1F>("NHits_ES_Minus","NHits_ES_Minus",500,0, 500);
  Hits_ES_Minus = fs->make<TH2F>("Hits_ES_Minus","Hits_ES_Minus",400,-200, 200, 400,-200, 200);

  NHits_EE_Plus = fs->make<TH1F>("NHits_EE_Plus","NHits_EE_Plus",500,0, 500);
  Hits_EE_Plus = fs->make<TH2F>("Hits_EE_Plus","Hits_EE_Plus",100,0, 100, 100,0, 100);
  NHits_EE_Minus = fs->make<TH1F>("NHits_EE_Minus","NHits_EE_Minus",500,0, 500);
  Hits_EE_Minus = fs->make<TH2F>("Hits_EE_Minus","Hits_EE_Minus",100,0, 100, 100,0, 100);
  */
}

// ------------ method called once each job just after ending the event loop  ------------
void Simhits_Analyzer::endJob() {
  // please remove this method if not needed
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void Simhits_Analyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Simhits_Analyzer);
