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
  explicit WriteVisitor(Writer* writer);

  ~WriteVisitor() override = default;

  void VisitDataElement(const DataElement* data_element) override;
  void VisitDataSequence(const DataSequence* data_sequence) override;
  void VisitDataSet(const DataSet* data_set) override;

private:
  void WriteUint16(std::uint16_t value);
  void WriteUint32(std::uint32_t value);

private:
  Writer* writer_;

  VR::Type vr_type_ = VR::EXPLICIT;
  ByteOrder byte_order_ = ByteOrder::LE;

  // Data set level (0 means root data set).
  int level_ = 0;

  // The tag currently being written.
  Tag tag_;
};

}  // namespace dcm 

#endif  // DCM_WRITE_VISITOR_H_
