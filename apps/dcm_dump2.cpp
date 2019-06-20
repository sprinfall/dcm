#include <iostream>

#include "dcm/data_element.h"
#include "dcm/data_sequence.h"
#include "dcm/data_set.h"
#include "dcm/dicom_file.h"
#include "dcm/logger.h"
#include "dcm/visitor.h"

#include "apps/print.h"

// -----------------------------------------------------------------------------

// A visitor to dump the data set recursively.
// Usage:
//   DumpVisitor v(std::cout);
//   data_set.Accept(v);
class DumpVisitor : public dcm::Visitor {
public:
  explicit DumpVisitor(std::ostream& os, const char* indent = "  ");

  ~DumpVisitor() override = default;

  void VisitDataElement(const dcm::DataElement* data_element) override;
  void VisitDataSequence(const dcm::DataSequence* data_sequence) override;
  void VisitDataSet(const dcm::DataSet* data_set) override;

private:
  std::ostream& os_;
  std::string indent_;
  int level_;
};

// -----------------------------------------------------------------------------

DumpVisitor::DumpVisitor(std::ostream& os, const char* indent)
  : os_(os), indent_(indent), level_(-1) {
}

void DumpVisitor::VisitDataElement(const dcm::DataElement* data_element) {
  for (int i = 0; i < level_; ++i) {
    os_ << indent_;
  }

  Print(os_, data_element);
  os_ << std::endl;
}

void DumpVisitor::VisitDataSequence(const dcm::DataSequence* data_sequence) {
  // Visit the sequence as a data element.
  VisitDataElement(data_sequence);

  ++level_;

  // Visit sequence items.
  for (std::size_t i = 0; i < data_sequence->size(); ++i) {
    const auto& item = data_sequence->At(i);

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

void DumpVisitor::VisitDataSet(const dcm::DataSet* data_set) {
  ++level_;

  // Visit the child data elements one by one.
  for (std::size_t i = 0; i < data_set->size(); ++i) {
    (*data_set)[i]->Accept(*this);
  }

  --level_;
}

// -----------------------------------------------------------------------------

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Dump a DICOM file by firstly loading the whole data set."
              << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  //DCM_LOG_INIT("", dcm::LOG_CONSOLE);

  const char* path = argv[1];

  dcm::DicomFile dicom_file(path);
  if (!dicom_file.Load()) {
    std::cerr << "Failed to load file." << std::endl;
    return 1;
  }

  DumpVisitor v(std::cout);
  dicom_file.Accept(v);

  return 0;
}
