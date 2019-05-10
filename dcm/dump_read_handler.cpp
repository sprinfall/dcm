#include "dcm/dump_read_handler.h"

#include <ostream>

#include "boost/core/ignore_unused.hpp"

#include "dcm/data_sequence.h"
#include "dcm/data_set.h"
#include "dcm/logger.h"

namespace dcm {

DumpReadHandler::DumpReadHandler(std::ostream& os, const char* indent)
    : os_(os), indent_(indent), level_(0) {
}

void DumpReadHandler::OnTransferSyntax(VR::Type vr_type, ByteOrder byte_order) {
  boost::ignore_unused(vr_type);
  boost::ignore_unused(byte_order);
}

bool DumpReadHandler::OnElementStart(Tag tag) {
  return true;  // Go on to read the element.
}

void DumpReadHandler::OnElementEnd(DataElement* data_element) {
  PrintIndent();

  data_element->Print(os_);
  os_ << std::endl;

  delete data_element;
}

void DumpReadHandler::OnSequenceStart(DataSequence* data_sequence) {
  LOG_INFO("OnSequenceStart");

  PrintIndent();

  static_cast<DataElement*>(data_sequence)->Print(os_);
  os_ << std::endl;

  delete data_sequence;
  ++level_;
}

void DumpReadHandler::OnSequenceEnd(DataElement* data_element) {
  LOG_INFO("OnSequenceEnd");

  if (data_element != nullptr) {
    // Sequence delimitation tag read.
    assert(data_element->tag() == tags::kSeqDelimatation);

    PrintIndent();

    data_element->Print(os_);
    os_ << std::endl;

    delete data_element;
  } else {
    // Sequence ended.
    --level_;
  }
}

void DumpReadHandler::OnSequenceItemStart(DataElement* data_element) {
  LOG_INFO("OnSequenceItemStart");

  PrintIndent();

  data_element->Print(os_);
  os_ << std::endl;

  delete data_element;
  ++level_;
}

void DumpReadHandler::OnSequenceItemEnd(DataElement* data_element) {
  LOG_INFO("OnSequenceItemEnd");

  if (data_element != nullptr) {
    // Sequence item delimitation tag read.
    assert(data_element->tag() == tags::kSeqItemDelimatation);

    PrintIndent();

    data_element->Print(os_);
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

}  // namespace dcm
