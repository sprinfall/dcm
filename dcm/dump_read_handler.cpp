#include "dcm/dump_read_handler.h"

#include <ostream>

#include "dcm/data_sequence.h"
#include "dcm/data_set.h"
#include "dcm/logger.h"

namespace dcm {

DumpReadHandler::DumpReadHandler(std::ostream& os, const char* indent)
    : os_(os), indent_(indent), level_(0) {
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

void DumpReadHandler::OnSequenceEnd() {
  LOG_INFO("OnSequenceEnd");

  --level_;
}

void DumpReadHandler::OnSequenceItemStart(DataElement* data_element) {
  LOG_INFO("OnSequenceItemStart");

  PrintIndent();

  data_element->Print(os_);
  os_ << std::endl;

  delete data_element;
  ++level_;
}

void DumpReadHandler::OnSequenceItemEnd() {
  LOG_INFO("OnSequenceItemEnd");

  --level_;
}

void DumpReadHandler::PrintIndent() {
  for (int i = 0; i < level_; ++i) {
    os_ << indent_;
  }
}

}  // namespace dcm
