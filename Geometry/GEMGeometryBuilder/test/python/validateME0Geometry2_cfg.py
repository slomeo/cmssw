import FWCore.ParameterSet.Config as cms

process = cms.Process('VALID')

process.source = cms.Source('EmptySource')

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(1)
    )


process.load('Configuration.Geometry.GeometryExtended2021_cff')
process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.load("Geometry.MuonNumbering.muonNumberingInitialization_cfi")



process.ME0GeometryESProducer = cms.ESProducer("ME0GeometryESModule",
                                               DDDetector = cms.ESInputTag('',''),
                                              # applyAlignment = cms.bool(False),
                                              # alignmentsLabel = cms.string(''),
                                               attribute = cms.string('MuStructure'),
                                               value = cms.string('MuonEndCapME0'),
                                               useDDD = cms.bool(True),
                                               useDD4hep = cms.bool(False)
                                              )

process.DDSpecParRegistryESProducer = cms.ESProducer("DDSpecParRegistryESProducer",
                                                     appendToDataLabel = cms.string('MUON')
                                                     )

process.MuonNumberingESProducer = cms.ESProducer("MuonNumberingESProducer",
                                                 label = cms.string('MUON'),
                                                 key = cms.string('MuonCommonNumbering')
                                                 )

#
# Note: Please, download the geometry file from a location
#       specified by Fireworks/Geometry/data/download.url
#
# For example: wget http://cmsdoc.cern.ch/cms/data/CMSSW/Fireworks/Geometry/data/v4/cmsGeom10.root
#
process.valid = cms.EDAnalyzer("ME0GeometryValidate",
                               infileName = cms.untracked.string('cmsRecoGeom-2021.root'),
                               outfileName = cms.untracked.string('validateME0Geometry2.root'),
                               tolerance = cms.untracked.int32(7)
                               )

process.p = cms.Path(process.valid)
