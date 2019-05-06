#include "dcm/data_sequence.h"

#include "dcm/data_set.h"
#include "dcm/visitor.h"

namespace dcm {

DataSequence::DataSequence(Tag tag, Endian endian)
    : DataElement(tag, tag.IsEmpty() ? VR::UNKNOWN : VR::SQ, endian),
      delimitation_(nullptr) {
  // Undefined length, instead of 0, makes more sense to a sequence.
  length_ = kUndefinedLength;
}

DataSequence::~DataSequence() {
}

void DataSequence::Accept(Visitor& visitor) const {
  visitor.VisitDataSequence(this);

  // NOTE:
  // The visitor is responsible for tranversing the child elements.
  // The drawback is that we end up duplicating the tranversal code in each
  // concrete visitor. (See Design Patterns, P.339)
}

void DataSequence::NewItem(DataElement* prefix) {
  DataSet* data_set = new DataSet(endian_);

  items_.push_back({ prefix, nullptr, data_set });
}

void DataSequence::EndItem(DataElement* delimitation) {
  assert(!items_.empty());

  items_.back().delimitation = delimitation;
}

bool DataSequence::AppendToLastItem(DataElement* data_element) {
  if (items_.empty()) {
    return false;
  }
  items_.back().data_set->Append(data_element);
  return true;
}

}  // namespace dcm
