#include "dcmlite/visitor.h"

#include <iostream>
#include <fstream>

#include "dcmlite/binary_file.h"
#include "dcmlite/data_element.h"
#include "dcmlite/data_set.h"
#include "dcmlite/util.h"

namespace dcmlite {

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

WriteVisitor::WriteVisitor(BinaryFile* file)
    : explicit_vr_(false)
    , level_(0)
    , file_(file) {
}

void WriteVisitor::VisitDataElement(DataElement* data_element) {
  Tag tag = data_element->tag();
  VR::Type vr_type = data_element->vr_type();

  // Tag
  file_->WriteUint16(tag.group());
  file_->WriteUint16(tag.element());

  // Special data element for SQ.
  if (tag == kSeqEndTag ||
      tag == kSeqItemEndTag ||
      tag == kSeqItemPrefixTag) {
    file_->WriteUint32(data_element->length());
    return;
  }

  std::size_t length = data_element->length();

  // VR
  if (explicit_vr_ || tag.group() == 2) {
    file_->WriteString(VR::ToString(vr_type));

    if (Is16BitsFollowingVrReversed(vr_type)) {
      // 2 reversed bytes.
      file_->WriteUint16(0);
      // 4 bytes value length.
      file_->WriteUint32(length);
    } else {
      // 2 bytes value length.
      file_->WriteUint16(static_cast<std::uint16_t>(length));
    }
  } else {
    // Implicit VR
    // 4 bytes value length.
    file_->WriteUint32(length);
  }

  if (vr_type != VR::SQ) {  // SQ is handled in VisitDataSet().
    if (length > 0) {
      // TODO: Convert endian for numbers if necessary.
      file_->WriteBytes(data_element->buffer().get(), length);
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
      file_->WriteUint32(0);
    }

    // Prefix
    file_->WriteString("DICM");

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

}  // namespace dcmlite
