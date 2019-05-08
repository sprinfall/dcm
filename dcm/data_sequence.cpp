#include "dcm/data_sequence.h"

#include "dcm/data_set.h"
#include "dcm/visitor.h"

namespace dcm {

DataSequence::DataSequence(Tag tag, bool explicit_vr, ByteOrder byte_order)
    : DataElement(tag, tag.IsEmpty() ? VR::UNKNOWN : VR::SQ, byte_order),
      explicit_vr_(explicit_vr),
      delimitation_(nullptr) {
  // Undefined length, instead of 0, makes more sense to a sequence.
  length_ = kUndefinedLength;
}

DataSequence::~DataSequence() {
  Clear();
}

void DataSequence::Accept(Visitor& visitor) const {
  visitor.VisitDataSequence(this);

  // NOTE:
  // The visitor is responsible for tranversing the child elements.
  // The drawback is that we end up duplicating the tranversal code in each
  // concrete visitor. (See Design Patterns, P.339)
}

void DataSequence::NewItem(DataElement* prefix) {
  auto data_set = new DataSet(explicit_vr_, byte_order_/* TODO: charset*/);

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

void DataSequence::Clear() {
  explicit_vr_ = true;

  for (auto& item: items_) {
    delete item.prefix;
    delete item.delimitation;
    delete item.data_set;
  }
  items_.clear();

  delete delimitation_;
}

}  // namespace dcm
