#include "dcm/data_sequence.h"

#include "dcm/data_set.h"
#include "dcm/visitor.h"

namespace dcm {

DataSequence::DataSequence(Tag tag, VR::Type vr_type, ByteOrder byte_order)
    : DataElement(tag, VR::SQ, byte_order),
      vr_type_(vr_type),
      delimitation_(nullptr) {
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
  auto data_set = new DataSet(vr_type_, byte_order_/* TODO: charset*/);

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
  vr_type_ = VR::EXPLICIT;

  for (auto& item: items_) {
    delete item.prefix;
    delete item.delimitation;
    delete item.data_set;
  }
  items_.clear();

  delete delimitation_;
}

}  // namespace dcm
