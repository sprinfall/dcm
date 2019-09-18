#include <iostream>

#include "boost/core/ignore_unused.hpp"

#include "dcm/data_sequence.h"
#include "dcm/data_set.h"
#include "dcm/dicom_reader.h"
#include "dcm/logger.h"
#include "dcm/read_handler.h"

#include "apps/print.h"

// -----------------------------------------------------------------------------

// A read handler to dump each data element.
class DumpReadHandler : public dcm::ReadHandler {
public:
  explicit DumpReadHandler(std::ostream& os, const char* indent = "  ");

  ~DumpReadHandler() override = default;

  void OnTransferSyntax(dcm::VR::Type vr_type, dcm::ByteOrder byte_order) override;

  bool OnElementStart(dcm::Tag tag) override;
  void OnElementEnd(dcm::DataElement* data_element) override;

  void OnSequenceStart(dcm::DataSequence* data_sequence) override;
  void OnSequenceEnd(dcm::DataElement* data_element = nullptr) override;

  void OnSequenceItemStart(dcm::DataElement* data_element) override;
  void OnSequenceItemEnd(dcm::DataElement* data_element = nullptr) override;

private:
  void PrintIndent();

private:
  std::ostream& os_;
  std::string indent_;
  int level_ = 0;
};

// -----------------------------------------------------------------------------

DumpReadHandler::DumpReadHandler(std::ostream& os, const char* indent)
    : os_(os), indent_(indent), level_(0) {
}

void DumpReadHandler::OnTransferSyntax(dcm::VR::Type vr_type,
                                       dcm::ByteOrder byte_order) {
  boost::ignore_unused(vr_type);
  boost::ignore_unused(byte_order);
}

bool DumpReadHandler::OnElementStart(dcm::Tag tag) {
  return true;  // Go on to read the element.
}

void DumpReadHandler::OnElementEnd(dcm::DataElement* data_element) {
  PrintIndent();

  Print(os_, data_element);
  os_ << std::endl;

  delete data_element;
}

void DumpReadHandler::OnSequenceStart(dcm::DataSequence* data_sequence) {
  LOG_INFO("OnSequenceStart");

  PrintIndent();

  Print(os_, data_sequence);
  os_ << std::endl;

  delete data_sequence;
  ++level_;
}

void DumpReadHandler::OnSequenceEnd(dcm::DataElement* data_element) {
  LOG_INFO("OnSequenceEnd");

  if (data_element != nullptr) {
    // Sequence delimitation tag read.
    assert(data_element->tag() == dcm::tags::kSeqDelimatation);

    PrintIndent();

    Print(os_, data_element);
    os_ << std::endl;

    delete data_element;
  } else {
    // Sequence ended.
    --level_;
  }
}

void DumpReadHandler::OnSequenceItemStart(dcm::DataElement* data_element) {
  LOG_INFO("OnSequenceItemStart");

  PrintIndent();

  Print(os_, data_element);
  os_ << std::endl;

  delete data_element;
  ++level_;
}

void DumpReadHandler::OnSequenceItemEnd(dcm::DataElement* data_element) {
  LOG_INFO("OnSequenceItemEnd");

  if (data_element != nullptr) {
    // Sequence item delimitation tag read.
    assert(data_element->tag() == dcm::tags::kSeqItemDelimatation);

    PrintIndent();

    Print(os_, data_element);
    os_ << std::endl;

    delete data_element;
  } else {
    // Sequence item ended.
    --level_;
  }
}

void DumpReadHandler::PrintIndent() {
  for (int i = 0; i < level_; ++i) {
    os_ << indent_;
  }
}

// -----------------------------------------------------------------------------

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cout << "Dump a DICOM file without firstly loading the whole data set "
                 "into memory." << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << argv[0] << " <file path>" << std::endl;
    return 1;
  }

  DCM_LOG_INIT("", dcm::LOG_CONSOLE);

  const char* path = argv[1];

  DumpReadHandler read_handler(std::cout);
  dcm::DicomReader reader(&read_handler);

  if (!reader.ReadFile(path)) {
    std::cerr << "Failed to read file." << std::endl;
    return 1;
  }

  return 0;
}
