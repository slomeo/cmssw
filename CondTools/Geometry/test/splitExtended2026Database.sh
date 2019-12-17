#!/bin/sh

conddb_import -f sqlite_file:myfile.db -c sqlite_file:GeometryFileExtended2026D49.db -t XMLFILE_Geometry_106YV4_Extended2026D49_mc -i XMLFILE_Geometry_106YV4_Extended2026D49_mc
conddb_import -f sqlite_file:myfile.db -c sqlite_file:TKRECO_Geometry.db -t TKRECO_Geometry_106YV4 -i TKRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:TKExtra_Geometry.db -t TKExtra_Geometry_106YV4 -i TKExtra_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:TKParameters_Geometry.db -t TKParameters_Geometry_106YV4 -i TKParameters_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:EBRECO_Geometry.db -t EBRECO_Geometry_106YV4 -i EBRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:HCALRECO_Geometry.db -t HCALRECO_Geometry_106YV4 -i HCALRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:HCALParameters_Geometry.db -t HCALParameters_Geometry_106YV4 -i HCALParameters_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:HGCALRECO_Geometry.db -t HGCALRECO_Geometry_106YV4 -i HGCALRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:HGCALParameters_Geometry.db -t HGCALParameters_Geometry_106YV4 -i HGCALParameters_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:CTRECO_Geometry.db -t CTRECO_Geometry_106YV4 -i CTRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:ZDCRECO_Geometry.db -t ZDCRECO_Geometry_106YV4 -i ZDCRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:CASTORRECO_Geometry.db -t CASTORRECO_Geometry_106YV4 -i CASTORRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:CSCRECO_Geometry.db -t CSCRECO_Geometry_106YV4 -i CSCRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:CSCRECODIGI_Geometry.db -t CSCRECODIGI_Geometry_106YV4 -i CSCRECODIGI_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:DTRECO_Geometry.db -t DTRECO_Geometry_106YV4 -i DTRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:RPCRECO_Geometry.db -t RPCRECO_Geometry_106YV4 -i RPCRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:GEMRECO_Geometry.db -t GEMRECO_Geometry_106YV4 -i GEMRECO_Geometry_106YV4
conddb_import -f sqlite_file:myfile.db -c sqlite_file:ME0RECO_Geometry.db -t ME0RECO_Geometry_106YV4 -i ME0RECO_Geometry_106YV4
