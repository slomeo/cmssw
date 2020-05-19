#ifndef RPCGeometry_RPCGeometryBuilderFromDDD_H
#define RPCGeometry_RPCGeometryBuilderFromDDD_H

/*
//\class RPCGeometryBuilder

 Description: RPC Geometry builder from DD & DD4hep
              DD4hep part added to the original old file (DD version) made by M. Maggi (INFN Bari)
//
// Author:  Sergio Lo Meo (sergio.lo.meo@cern.ch) following what Ianna Osborne made for DTs (DD4HEP migration)
//          Created:  Fri, 20 Sep 2019 
//          Modified: Tue, 19 May 2020, following what Sunanda Banerjee made in PR #29842 
*/

#include <string>
#include <map>
#include <list>
#include <memory>

class DDCompactView;
class DDFilteredView;
namespace cms {
  class DDFilteredView;
  class DDCompactView;
  class MuonNumbering;
  struct DDSpecPar;
  struct DDSpecParRegistry;
}  
//class RPCNumberingScheme;
class RPCGeometry;
class RPCDetId;
class RPCRoll;
class MuonGeometryConstants;

class RPCGeometryBuilderFromDDD {
public:
  RPCGeometryBuilderFromDDD(bool comp11);
  ~RPCGeometryBuilderFromDDD();
  // for DDD
  RPCGeometry* build(const DDCompactView* cview, const MuonGeometryConstants& muonConstants);
  // for DD4hep
  RPCGeometry* build(const cms::DDCompactView* cview, const MuonGeometryConstants& muonConstants);

private:
  // for DDD
  RPCGeometry* buildGeometry(DDFilteredView& fview, const MuonGeometryConstants& muonConstants);
  // for DD4hep
  RPCGeometry* buildGeometry(cms::DDFilteredView& fview, const MuonGeometryConstants& muonConstants);

  std::map<RPCDetId, std::list<RPCRoll*> > chids;
  //std::unique_ptr<RPCNumberingScheme> rpcnum_ = nullptr;
  bool theComp11Flag;
};

#endif
