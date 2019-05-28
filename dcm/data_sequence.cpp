#include "dcm/data_sequence.h"

#include "dcm/data_set.h"
#include "dcm/visitor.h"

namespace dcm {

DataSequence::DataSequence(Tag tag)
    : DataElement(tag, VR::SQ), delimitation_(nullptr) {
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

std::uint32_t DataSequence::GetElementLength(VR::Type vr_type,
                                             bool recursively) const {
  std::uint32_t element_length = DataElement::GetElementLength(vr_type);

  if (recursively) {
    for (auto& item : items_) {
      if (item.prefix != nullptr) {
        element_length += item.prefix->GetElementLength(vr_type);
      }

      for (std::size_t i = 0; i < item.data_set->size(); ++i) {
        element_length += item.data_set->At(i)->GetElementLength(vr_type);
      }

      if (item.delimitation != nullptr) {
        element_length += item.delimitation->GetElementLength(vr_type);
      }
    }
  }

  return element_length;
}

void DataSequence::NewItem(DataElement* prefix, VR::Type vr_type,
                           ByteOrder byte_order) {
  // TODO: charset
  auto data_set = new DataSet(vr_type, byte_order);

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
  for (auto& item: items_) {
    delete item.prefix;
    delete item.delimitation;
    delete item.data_set;
  }
  items_.clear();

  delete delimitation_;
}

}  // namespace dcm
