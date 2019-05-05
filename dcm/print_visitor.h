#ifndef DCM_PRINT_VISITOR_H_
#define DCM_PRINT_VISITOR_H_

#include <iosfwd>
#include <string>

#include "dcm/visitor.h"

namespace dcm {

// A visitor to print the data set recursively.
// Usage:
//   PrintVisitor v(std::cout);
//   data_set.Accept(v);
class PrintVisitor : public Visitor {
public:
  explicit PrintVisitor(std::ostream& os, const char* indent = "  ");

  ~PrintVisitor() override = default;

  void VisitDataElement(const DataElement* data_element) override;
  void VisitDataSequence(const DataSequence* data_sequence) override;
  void VisitDataSet(const DataSet* data_set) override;

private:
  std::ostream& os_;
  std::string indent_;
  int level_;
};

}  // namespace dcm 

#endif  // DCM_PRINT_VISITOR_H_
