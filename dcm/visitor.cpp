#include "dcm/visitor.h"

#include <iostream>

#include "dcm/writer.h"
#include "dcm/data_element.h"
#include "dcm/data_set.h"
#include "dcm/util.h"

namespace dcm {

PrintVisitor::PrintVisitor(std::ostream& os)
    : os_(os), level_(-1) {
}

void PrintVisitor::VisitDataElement(DataElement* data_element) {
  if (level_ > 0) {
    os_ << std::string(level_, '\t');
  }
  os_ << *data_element << std::endl;
}

void PrintVisitor::VisitDataSet(DataSet* data_set) {
  // Visit the data set as a data element.
  VisitDataElement(data_set);

  ++level_;

  // Visit the child data elements one by one.
  std::size_t size = data_set->GetSize();
  for (std::size_t i = 0; i < size; ++i) {
    (*data_set)[i]->Accept(*this);
  }

  --level_;
}

////////////////////////////////////////////////////////////////////////////////

void WriteVisitor::VisitDataElement(DataElement* data_element) {
  Tag tag = data_element->tag();
  VR vr = data_element->vr();

  // Tag
  writer_->WriteUint16(tag.group());
  writer_->WriteUint16(tag.element());

  // Special data element for SQ.
  if (tag == kSeqEndTag ||
      tag == kSeqItemEndTag ||
      tag == kSeqItemPrefixTag) {
    writer_->WriteUint32(data_element->length());
    return;
  }

  std::size_t length = data_element->length();

  // VR
  if (explicit_vr_ || tag.group() == 2) {
    writer_->WriteString(VRToString(data_element->vr()));

    if (Is16BitsFollowingVrReversed(data_element->vr())) {
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

  if (vr != VR::SQ) {  // SQ is handled in VisitDataSet().
    if (length > 0) {
      // TODO: Convert endian for numbers if necessary.
      writer_->WriteBytes(&(data_element->buffer()[0]), length);
    }
  }
}

void WriteVisitor::VisitDataSet(DataSet* data_set) {
  explicit_vr_ = data_set->explicit_vr();

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

  } if (level_ > 0) {
    // Visit the data set as a data element.
    VisitDataElement(data_set);
  }

  ++level_;

  // Visit the child data elements one by one.
  std::size_t size = data_set->GetSize();
  for (std::size_t i = 0; i < size; ++i) {
    (*data_set)[i]->Accept(*this);
  }

  --level_;
}

}  // namespace dcm
