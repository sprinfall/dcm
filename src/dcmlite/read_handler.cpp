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

bool DumpReadHandler::OnElementStart(Tag /*tag*/) {
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

void FullReadHandler::OnEndian(Endian endian) {
  data_set_->set_endian(endian);
}

void FullReadHandler::OnExplicitVR(bool explicit_vr) {
  data_set_->set_explicit_vr(explicit_vr);
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

TagsReadHandler::TagsReadHandler(DataSet* data_set)
    : data_set_(data_set) {
  data_set_stack_.push_back(data_set);
}

TagsReadHandler::~TagsReadHandler() {
  ClearTags();
}

void TagsReadHandler::OnEndian(Endian endian) {
  data_set_->set_endian(endian);
}

void TagsReadHandler::OnExplicitVR(bool explicit_vr) {
  data_set_->set_explicit_vr(explicit_vr);
}

bool TagsReadHandler::OnElementStart(Tag tag) {
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
  assert(!data_set_stack_.empty());
  data_set_stack_.back()->AddElement(data_element);
}

void TagsReadHandler::OnSeqElementStart(DataSet* data_set) {
  assert(!data_set_stack_.empty());
  data_set_stack_.back()->AddElement(data_set);
  data_set_stack_.push_back(data_set);
}

void TagsReadHandler::OnSeqElementEnd(DataSet* /*data_set*/) {
  data_set_stack_.pop_back();
}

TagsReadHandler& TagsReadHandler::AddTag(Tag tag) {
  if (tags_.empty()) {
    tags_.push_back(tag);
    return *this;
  }

  // Make sure the tags are sorted.
  auto lower = std::lower_bound(tags_.begin(), tags_.end(), tag);
  if (lower == tags_.end() || *lower != tag) {
    tags_.insert(lower, tag);
  }

  return *this;
}

void TagsReadHandler::ClearTags() {
  tags_.clear();
}

}  // namespace dcmlite
