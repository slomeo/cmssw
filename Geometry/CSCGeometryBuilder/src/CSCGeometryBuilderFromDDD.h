#ifndef CSCGeometryBuilder_CSCGeometryBuilderFromDDD_h
#define CSCGeometryBuilder_CSCGeometryBuilderFromDDD_h

/** \class CSCGeometryBuilderFromDDD
 *
 *  Build the CSCGeometry from the DDD description.
 *
 *  \author Tim Cox
 */

#include <DataFormats/MuonDetId/interface/CSCDetId.h>
#include "Geometry/MuonNumbering/interface/DD4hep_MuonNumbering.h"
#include <string>

class DDCompactView;
class CSCGeometry;
class MuonDDDConstants;
namespace cms {
  class DDFilteredView;
  class DDCompactView;
  class MuonNumbering;
} 

class CSCGeometryBuilderFromDDD {
public:
  /// Constructor
  CSCGeometryBuilderFromDDD();

  /// Destructor
  virtual ~CSCGeometryBuilderFromDDD();

  // Build the geometry DDD
void build(CSCGeometry& geom, const DDCompactView* fv, const MuonDDDConstants& muonConstants);

// Build the geometry dd4hep
void build(CSCGeometry& geom, const cms::DDCompactView* cview, const cms::MuonNumbering& muonConstants);

protected:
private:
  const std::string myName;
};
#endif
