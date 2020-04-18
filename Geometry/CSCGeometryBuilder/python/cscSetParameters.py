from Geometry.CSCGeometryBuilder.cscSetParameters_cfi import cscSetParameters

from Configuration.ProcessModifiers.dd4hep_cff import dd4hep

dd4hep.toModify(cscSetParameters, 
                useDDD = False, 
                useDD4hep = True,
                applyAlignment = False, 
                debugV = cms.untracked.bool(False),
                useGangedStripsInME1a = False,
                useOnlyWiresInME1a = False,
                useRealWireGeometry = True,
                useCentreTIOffsets = False,   
                alignmentsLabel = '', 
                appendToDataLabel = '', 
                attribute = 'MuStructure', 
                value = 'MuonEndcapCSC',
                DDDetector = cms.ESInputTag('',''))
               
