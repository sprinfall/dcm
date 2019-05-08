#ifndef DCM_WRITE_VISITOR_H_
#define DCM_WRITE_VISITOR_H_

#include "dcm/defs.h"
#include "dcm/visitor.h"

namespace dcm {

class Writer;

// A visitor to write the data set.
//
// Usage:
//   WriteVisitor v("path/to/file");
//   data_set.Accept(v);
//
class WriteVisitor : public Visitor {
public:
  explicit WriteVisitor(Writer* writer) : writer_(writer) {}

  ~WriteVisitor() override = default;

  void VisitDataElement(const DataElement* data_element) override;
  void VisitDataSequence(const DataSequence* data_sequence) override;
  void VisitDataSet(const DataSet* data_set) override;

private:
  Writer* writer_;

  // If the current visited data set is explicit VR or not.
  VR::Type vr_type_ = VR::EXPLICIT;

  int level_ = 0;
};

}  // namespace dcm 

#endif  // DCM_WRITE_VISITOR_H_
