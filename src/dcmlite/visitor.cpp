#include "dcmlite/visitor.h"

#include <iostream>

#include "dcmlite/data_element.h"
#include "dcmlite/data_set.h"

namespace dcmlite {

PrintVisitor::PrintVisitor(std::ostream& os)
    : os_(os), level_(-1) {
}

void PrintVisitor::VisitDataElement(DataElement* data_element) {
  if (level_ > 0) {
    os_ << std::string(level_, '\t');
  }
  os_ << *data_element << std::endl;
}

void PrintVisitor::VisitDataSet(DataSet* data_set) {
  VisitDataElement(data_set);

  ++level_;

  size_t size = data_set->GetSize();
  for (size_t i = 0; i < size; ++i) {
    (*data_set)[i]->Accept(*this);
  }

  --level_;
}

}  // namespace dcmlite