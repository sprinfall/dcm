#include "dcm/write_visitor.h"

#include "dcm/data_element.h"
#include "dcm/data_sequence.h"
#include "dcm/data_set.h"
#include "dcm/util.h"
#include "dcm/writer.h"

namespace dcm {

void WriteVisitor::VisitDataElement(const DataElement* data_element) {
  Tag tag = data_element->tag();

  // Tag
  writer_->WriteUint16(tag.group());
  writer_->WriteUint16(tag.element());

  // Special data element for SQ.
  if (tag == kSeqEndTag) {
    writer_->WriteUint32(data_element->length());  // 0
    return;
  }
  if (tag == kSeqItemEndTag) {
    writer_->WriteUint32(data_element->length());  // 0
    return;
  }
  if (tag == kSeqItemPrefixTag) {
    writer_->WriteUint32(data_element->length());
    return;
  }

  VR vr = data_element->vr();
  std::size_t length = data_element->length();

  // VR
  if (vr_type_ == VR::EXPLICIT || tag.group() == 2) {
    writer_->WriteString(data_element->vr().ToString());

    if (data_element->vr().Is16BitsFollowingReversed()) {
      // 2 reversed bytes.
      writer_->WriteUint16(0);
      // 4 bytes value length.
      writer_->WriteUint32(length);
    } else {
      // 2 bytes value length.
      writer_->WriteUint16(static_cast<std::uint16_t>(length));
    }
  } else {
    // Implicit VR
    // 4 bytes value length.
    writer_->WriteUint32(length);
  }

  if (vr != VR::SQ) {  // SQ item has no value by itself.
    if (length > 0) {
      // TODO: Convert byte order for numbers if necessary.
      writer_->WriteBytes(&(data_element->buffer()[0]), length);
    }
  }
}

void WriteVisitor::VisitDataSequence(const DataSequence* data_sequence) {
  // Visit the sequence as a data element.
  VisitDataElement(data_sequence);

  ++level_;

  // Visit sequence items.
  for (std::size_t i = 0; i < data_sequence->size(); ++i) {
    const auto& item = data_sequence->GetItem(i);

    VisitDataElement(item.prefix);

    VisitDataSet(item.data_set);

    if (item.delimitation != nullptr) {
      VisitDataElement(item.delimitation);
    }
  }

  --level_;
}

void WriteVisitor::VisitDataSet(const DataSet* data_set) {
  vr_type_ = data_set->vr_type();

  // level_ starts from 0.
  if (level_ == 0) {
    // level_ == 0 means root data set.
    // The data element itself doesn't need to be written.
    // Just write the Preamble & prefix.

    // Preamble (128 bytes)
    for (std::size_t i = 0; i < 32; ++i) {
      writer_->WriteUint32(0);
    }

    // Prefix
    writer_->WriteString("DICM");
  }

  // Visit the child data elements one by one.
  std::size_t size = data_set->size();
  for (std::size_t i = 0; i < size; ++i) {
    (*data_set)[i]->Accept(*this);
  }
}

}  // namespace dcm
