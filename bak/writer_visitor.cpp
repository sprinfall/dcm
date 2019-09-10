
  void WriteNumber16(const void* value);
  void WriteNumber32(const void* value);
  void WriteNumber64(const void* value);

  void WriteBuffer(const DataElement* data_element);


void WriteVisitor::WriteNumber16(const void* value) {
  if (kByteOrderOS != (tag_.group() == 2 ? ByteOrder::LE : byte_order_)) {
    char buf[2];
    memcpy(buf, value, 2);
    Swap16(buf);
    writer_->WriteBytes(buf, 2);
  } else {
    writer_->WriteBytes(value, 2);
  }
}

// TODO: refine
void WriteVisitor::WriteNumber32(const void* value) {
  if (kByteOrderOS != (tag_.group() == 2 ? ByteOrder::LE : byte_order_)) {
    char buf[4];
    memcpy(buf, value, 4);
    Swap32(buf);
    writer_->WriteBytes(buf, 4);
  } else {
    writer_->WriteBytes(value, 4);
  }
}

// TODO: refine
void WriteVisitor::WriteNumber64(const void* value) {
  if (kByteOrderOS != (tag_.group() == 2 ? ByteOrder::LE : byte_order_)) {
    char buf[8];
    memcpy(buf, value, 8);
    Swap64(buf);
    writer_->WriteBytes(buf, 8);
  } else {
    writer_->WriteBytes(value, 8);
  }
}

void WriteVisitor::WriteBuffer(const DataElement* data_element) {
  const Buffer& buffer = data_element->buffer();

  VR vr = data_element->vr();

  // TODO: VR::AT

  switch (vr.code()) {
    //case VR::SS:  // Signed Short (2 bytes)
    //case VR::US:  // Unsigned Short (2 bytes)
    //  assert(buffer.size() == 2);
    //  WriteNumber16(&buffer[0]);
    //  break;

    //case VR::FL:  // Floating Point Single (4 bytes)
    case VR::SL:  // Signed Long (4 bytes)
    case VR::UL:  // Unsigned Long (4 bytes)
      //assert(buffer.size() == 4);
      WriteNumber32(&buffer[0]);
      break;

    //case VR::FD:  // Floating Point Double (8 bytes)
    //  assert(buffer.size() == 8);
    //  WriteNumber64(&buffer[0]);
    //  break;

    default:
      writer_->WriteBytes(&buffer[0], buffer.size());
      break;
  }

  // TODO: Other XXX

  // Other Double
  // A stream of 64-bit IEEE 754:1985 floating point words. OD is a VR that
  // requires byte swapping within each 64-bit word whenchanging byte ordering.
  //VR::OD;

  // Other Float
  // A stream of 32-bit IEEE 754:1985 floating point words. OF is a VR that
  // requires byte swapping within each 32-bit word when changingbyte ordering.
  //VR::OF;

  // Other Long
  // A stream of 32-bit words where the encoding of the contents is specified
  // by the negotiated Transfer Syntax. OL is a VR that requires byte swapping
  // within each word when changing byte ordering.
  //VR::OL;

  // Other Word
  // A stream of 16-bit words where the encoding of the contents is specified
  // by the negotiated Transfer Syntax. OW is a VR that requires byte swapping
  // within each word when changing byte ordering.
  //VR::OW;
}