#include "dcm/print_visitor.h"

#include <ostream>

#include "dcm/data_element.h"
#include "dcm/data_sequence.h"
#include "dcm/data_set.h"

namespace dcm {

PrintVisitor::PrintVisitor(std::ostream& os, const char* indent)
    : os_(os), indent_(indent), level_(-1) {
}

void PrintVisitor::VisitDataElement(const DataElement* data_element) {
  for (int i = 0; i < level_; ++i) {
    os_ << indent_;
  }

  data_element->Print(os_);
  os_ << std::endl;
}

void PrintVisitor::VisitDataSequence(const DataSequence* data_sequence) {
  // Visit the sequence as a data element.
  VisitDataElement(data_sequence);

  ++level_;

  // Visit sequence items.
  for (std::size_t i = 0; i < data_sequence->size(); ++i) {
    const auto& item = data_sequence->GetItem(i);

    VisitDataElement(item.prefix);

    VisitDataSet(item.data_set);

    if (item.delimitation != nullptr) {
      VisitDataElement(item.delimitation);
    }
  }

  if (data_sequence->delimitation() != nullptr) {
    VisitDataElement(data_sequence->delimitation());
  }

  --level_;
}

void PrintVisitor::VisitDataSet(const DataSet* data_set) {
  ++level_;

  // Visit the child data elements one by one.
  for (std::size_t i = 0; i < data_set->size(); ++i) {
    (*data_set)[i]->Accept(*this);
  }

  --level_;
}

}  // namespace dcm
