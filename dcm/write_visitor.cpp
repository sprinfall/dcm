#include "dcm/write_visitor.h"

#include "dcm/data_element.h"
#include "dcm/data_sequence.h"
#include "dcm/data_set.h"
#include "dcm/util.h"
#include "dcm/writer.h"

namespace dcm {

void WriteVisitor::VisitDataElement(const DataElement* data_element) {
  tag_ = data_element->tag();

  // Tag
  WriteUint16(tag_.group());
  WriteUint16(tag_.element());

  // Special data element for SQ.
  if (tag_ == tags::kSeqDelimatation || tag_ == tags::kSeqItemDelimatation ||
      tag_ == tags::kSeqItemPrefix) {
    // Value length of kSeqEndTag and kSeqItemEndTag should both be 0.
    WriteUint32(data_element->length());
    return;
  }

  VR vr = data_element->vr();
  std::size_t length = data_element->length();

  // VR
  if (vr_type_ == VR::EXPLICIT || tag_.group() == 2) {
    // TODO: VR::UNKNOWN
    writer_->WriteByte(vr.byte1());
    writer_->WriteByte(vr.byte2());

    if (vr.Is16BitsFollowingReversed()) {
      // 2 reversed bytes.
      WriteUint16(0);
      // 4 bytes value length.
      WriteUint32(length);
    } else {
      // 2 bytes value length.
      WriteUint16(static_cast<std::uint16_t>(length));
    }
  } else {
    // Implicit VR
    // 4 bytes value length.
    WriteUint32(length);
  }

  if (vr != VR::SQ) {
    if (length > 0) {
      const Buffer& buffer = data_element->buffer();
      writer_->WriteBytes(&buffer[0], length);
    }
  }
}

void WriteVisitor::VisitDataSequence(const DataSequence* data_sequence) {
  // Visit the sequence as a normal data element.
  VisitDataElement(data_sequence);

  // Visit sequence items.
  for (std::size_t i = 0; i < data_sequence->size(); ++i) {
    const auto& item = data_sequence->GetItem(i);

    VisitDataElement(item.prefix);

    VisitDataSet(item.data_set);

    if (item.delimitation != nullptr) {
      VisitDataElement(item.delimitation);
    }
  }
}

void WriteVisitor::VisitDataSet(const DataSet* data_set) {
  vr_type_ = data_set->vr_type();
  byte_order_ = data_set->byte_order();

  if (level_ == 0) {
    // Root data set.
    // Just write the preamble and DICOM prefix.

    // Preamble (128 bytes)
    for (std::size_t i = 0; i < 32; ++i) {
      WriteUint32(0);
    }

    // Prefix
    writer_->WriteBytes("DICM", 4);
  }

  ++level_;

  // Visit the child data elements one by one.
  std::size_t size = data_set->size();
  for (std::size_t i = 0; i < size; ++i) {
    (*data_set)[i]->Accept(*this);
  }

  --level_;
}

void WriteVisitor::WriteUint16(std::uint16_t value) {
  if (value != 0) {
    if (tag_.group() == 2) {
      // Meta header always in Little Endian.
      if (kByteOrderOS != ByteOrder::LE) {
        Swap16(&value);
      }
    } else {
      if (kByteOrderOS != byte_order_) {
        Swap16(&value);
      }
    }
  }

  writer_->WriteUint16(value);
}

void WriteVisitor::WriteUint32(std::uint32_t value) {
  if (value != 0) {
    if (tag_.group() == 2) {
      // Meta header always in Little Endian.
      if (kByteOrderOS != ByteOrder::LE) {
        Swap32(&value);
      }
    } else {
      if (kByteOrderOS != byte_order_) {
        Swap32(&value);
      }
    }
  }

  writer_->WriteUint32(value);
}

}  // namespace dcm
