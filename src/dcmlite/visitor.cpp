#include "dcmlite/visitor.h"

#include <iostream>

#include "dcmlite/data_element.h"
#include "dcmlite/data_set.h"

namespace dcmlite {

PrintVisitor::PrintVisitor(std::ostream& os)
    : os_(os), level_(-1) {
}

void PrintVisitor::VisitDataElement(const DataElement* data_element) {
  if (level_ > 0) {
    os_ << std::string(level_, '\t');
  }
  os_ << *data_element << std::endl;
}

void PrintVisitor::VisitDataSet(const DataSet* data_set) {
  VisitDataElement(data_set);

  ++level_;

  const std::list<DataElement*>& elements = data_set->elements();
  for (DataElement* element : elements) {
    element->Accept(*this);
  }

  --level_;
}

}  // namespace dcmlite