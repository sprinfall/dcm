#include "dcmlite/read_handler.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "dcmlite/data_element.h"
#include "dcmlite/data_set.h"

namespace dcmlite {

////////////////////////////////////////////////////////////////////////////////

DumpReadHandler::DumpReadHandler()
    : level_(0) {
}

DumpReadHandler::~DumpReadHandler() {
}

bool DumpReadHandler::OnElementStart(const Tag& tag) {
  return true;  // Go on to read the element.
}

void DumpReadHandler::OnElementEnd(DataElement* data_element) {
  if (level_ > 0) {
    std::cout << std::string(level_, '\t');
  }
  std::cout << *data_element << std::endl;
  delete data_element;
}

void DumpReadHandler::OnSeqElementStart(DataSet* data_set) {
  if (level_ > 0) {
    std::cout << std::string(level_, '\t');
  }
  std::cout << *data_set << std::endl;
  ++level_;
}

void DumpReadHandler::OnSeqElementEnd(DataSet* data_set) {
  --level_;
  delete data_set;
}

////////////////////////////////////////////////////////////////////////////////

FullReadHandler::FullReadHandler(DataSet* data_set)
    : data_set_(data_set) {
  data_set_stack_.push_back(data_set);
}

void FullReadHandler::OnElementEnd(DataElement* data_element) {
  assert(!data_set_stack_.empty());
  data_set_stack_.back()->AddElement(data_element);
}

void FullReadHandler::OnSeqElementStart(DataSet* data_set) {
  assert(!data_set_stack_.empty());
  data_set_stack_.back()->AddElement(data_set);
  data_set_stack_.push_back(data_set);
}

void FullReadHandler::OnSeqElementEnd(DataSet* /*data_set*/) {
  data_set_stack_.pop_back();
}

////////////////////////////////////////////////////////////////////////////////

TagsReadHandler::TagsReadHandler() {
}

TagsReadHandler::~TagsReadHandler() {
  Clear();
}

bool TagsReadHandler::OnElementStart(const Tag& tag) {
  if (tags_.empty()) {
    // All tags have been read, stop the process.
    should_stop_ = true;
    return false;
  }

  if (tag.group() != 0xFFFE && tag > tags_.back()) {
    // Larger than the last tag to read, stop the process.
    should_stop_ = true;
    return false;
  }

  auto it = std::find(tags_.begin(), tags_.end(), tag);

  if (it == tags_.end()) {
    // Not a tag to read, skip it.
    return false;
  }

  // Remove the tag.
  tags_.erase(it);

  return true;
}

void TagsReadHandler::OnElementEnd(DataElement* data_element) {
  elements_.push_back(data_element);
}

void TagsReadHandler::OnSeqElementStart(DataSet* data_set) {
}

void TagsReadHandler::OnSeqElementEnd(DataSet* data_set) {
  delete data_set;
}

void TagsReadHandler::AddTag(const Tag& tag) {
  if (tags_.empty()) {
    tags_.push_back(tag);
    return;
  }

  // Make sure the tags are sorted.
  auto lower = std::lower_bound(tags_.begin(), tags_.end(), tag);
  if (lower == tags_.end() || *lower != tag) {
    tags_.insert(lower, tag);
  }
}

DataElement* TagsReadHandler::GetElement(const Tag& tag) const {
  for (DataElement* element : elements_) {
    if (element->tag() == tag) {
      return element;
    }
  }
  return NULL;
}

void TagsReadHandler::Clear() {
  tags_.clear();

  for (DataElement* element : elements_) {
    delete element;
  }
  elements_.clear();
}

}  // namespace dcmlite
