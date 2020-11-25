import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing

#set up process
process = cms.Process("ntuples")

# delete the comment below if you are working with DT, RPC, CSC----------
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
# ------------------------------------------------------------------------
# comment the lines below if you are working with DT, RPC, CSC----------
# only for DD4HEP (comment the line for DDD)
#process.load('Configuration.StandardSequences.DD4hep_GeometrySim_cff')
# only for DD (comment the line for DD4HEP)
#process.load('Configuration.Geometry.GeometryExtended2021_cff')
#
#process.load("Geometry.MuonNumbering.muonGeometryConstants_cff")
#process.load("Geometry.GEMGeometryBuilder.gemGeometry_cff")
# ------------------------------------------------------------------------

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

options = VarParsing.VarParsing('standard')

#print only every 20000th event
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = cms.untracked.int32(1)
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(20000) )

# select DDD or DD4HEP file 
process.source = cms.Source("PoolSource",
                            fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/s/slomeo/step1_SingleMu_ddd.root')
#                           fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/s/slomeo/step1_SingleMu_dd4hep.root')
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

# comment the 3 lines below if you are working with DT, RPC, CSC----------
# only for GEM DD4HEP (comment these lines for DDD)
#process.muonGeometryConstants.fromDD4Hep = True
#process.gemGeometry.fromDDD = False
#process.gemGeometry.fromDD4Hep = True
#-------------------------------------------------------------------------

process.p = cms.Path(process.ntuples)

