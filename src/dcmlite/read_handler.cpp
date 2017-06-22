#include "dcmlite/read_handler.h"

#include <cassert>
#include <iostream>

#include "dcmlite/data_element.h"
#include "dcmlite/data_set.h"

namespace dcmlite {

////////////////////////////////////////////////////////////////////////////////

bool DumpReadHandler::OnElement(DataElement* data_element) {
  if (indent_level_ > 0) {
    std::cout << std::string(indent_level_, '\t');
  }
  std::cout << *data_element << std::endl;
  delete data_element;
  return true;
}

bool DumpReadHandler::OnSeqElementStart(DataSet* data_set) {
  if (indent_level_ > 0) {
    std::cout << std::string(indent_level_, '\t');
  }
  std::cout << *data_set << std::endl;
  ++indent_level_;
  return true;
}

bool DumpReadHandler::OnSeqElementEnd(DataSet* data_set) {
  --indent_level_;
  delete data_set;
  return true;
}

////////////////////////////////////////////////////////////////////////////////

FullReadHandler::FullReadHandler(DataSet* data_set)
    : data_set_(data_set) {
  data_set_stack_.push_back(data_set);
}

bool FullReadHandler::OnElement(DataElement* data_element) {
  assert(!data_set_stack_.empty());

  data_set_stack_.back()->AddElement(data_element);

  return true;
}

bool FullReadHandler::OnSeqElementStart(DataSet* data_set) {
  assert(!data_set_stack_.empty());
  data_set_stack_.back()->AddElement(data_set);

  data_set_stack_.push_back(data_set);

  return true;
}

bool FullReadHandler::OnSeqElementEnd(DataSet* /*data_set*/) {
  data_set_stack_.pop_back();

  return true;
}

}  // namespace dcmlite
