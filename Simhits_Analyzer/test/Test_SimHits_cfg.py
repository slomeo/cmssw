import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

#set up process
process = cms.Process("ntuples")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

options = VarParsing.VarParsing('standard')


#print only every 10000th event
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.source = cms.Source("PoolSource",
           fileNames = cms.untracked.vstring('file:/afs/cern.ch/work/r/rchatter/ECAL_Step1_Data_DD4HEP_31Oct/step1_ZMM_dd4hep.root')## Change path appropriately
	    )
# turn on VID producer, indicate data format  to be
#from PhysicsTools.SelectorUtils.tools.vid_id_tools import *
#dataFormat = DataFormat.AOD
#switchOnVIDElectronIdProducer(process, dataFormat)

# define which IDs we want to produce
#my_id_modules = ['RecoEgamma.ElectronIdentification.Identification.mvaElectronID_Spring16_GeneralPurpose_V1_cff']

#add them to the VID producer
#for idmod in my_id_modules:
#    setupAllVIDIdsInModule(process,idmod,setupVIDElectronSelection)

#tell the process what variables and plug-in to use
process.ntuples = cms.EDAnalyzer('Simhits_Analyzer',
				 )


#provide output file
process.TFileService = cms.Service("TFileService",
	            fileName = cms.string('file:myFile.root')
	    )


process.p = cms.Path(process.ntuples)

