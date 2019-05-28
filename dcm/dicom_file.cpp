#include "dcm/dicom_file.h"

#include "boost/filesystem/fstream.hpp"

#include "dcm/dicom_reader.h"
#include "dcm/full_read_handler.h"
#include "dcm/logger.h"
#include "dcm/write_visitor.h"
#include "dcm/writer.h"

namespace bfs = boost::filesystem;

namespace dcm {

DicomFile::DicomFile(const Path& path) : path_(path) {
}

bool DicomFile::Load() {
  FullReadHandler read_handler(this);
  DicomReader reader(&read_handler);

  if (!reader.ReadFile(path_)) {
    return false;
  }

  return true;
}

bool DicomFile::SetTransferSyntax(const std::string& transfer_syntax_uid) {
  if (transfer_syntax_uid.empty()) {
    // Keep the original transfer syntax.
    return false;
  }

  using namespace transfer_syntax_uids;

  if (transfer_syntax_uid == kImplicitLittleEndian) {
    SetVRType(VR::IMPLICIT);
    SetByteOrder(ByteOrder::LE);
  } else if (transfer_syntax_uid == kExplicitLittleEndian) {
    SetVRType(VR::EXPLICIT);
    SetByteOrder(ByteOrder::LE);
  } else if (transfer_syntax_uid == kExplicitBigEndian) {
    SetVRType(VR::EXPLICIT);
    SetByteOrder(ByteOrder::BE);
  } else {
    // TODO
    return false;
  }

  transfer_syntax_uid_ = transfer_syntax_uid;

  SetString(tags::kTransferSyntaxUID, transfer_syntax_uid_);

  UpdateGroupLength(tags::kTransferSyntaxUID.group());

  return true;
}

bool DicomFile::Save(const Path& new_path) {
  bfs::ofstream stream(new_path, std::ios::binary);
  if (stream.bad()) {
    return false;
  }

  Writer writer(&stream);
  WriteVisitor v(&writer);

  Accept(v);

  return true;
}

}  // namespace dcm
