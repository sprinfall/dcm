#include "dcm/dicom_file.h"

#include "boost/filesystem/fstream.hpp"

#include "dcm/dicom_reader.h"
#include "dcm/full_read_handler.h"
#include "dcm/logger.h"
#include "dcm/write_visitor.h"
#include "dcm/writer.h"

namespace bfs = boost::filesystem;

namespace dcm {

DicomFile::DicomFile(const Path& path)
    : path_(path) {
}

bool DicomFile::Load() {
  FullReadHandler read_handler(this);
  DicomReader reader(&read_handler);

  if (!reader.ReadFile(path_)) {
    return false;
  }

  return true;
}

bool DicomFile::Save(const Path& new_path) {
  bfs::ofstream stream(new_path, std::ios::binary);
  if (stream.bad()) {
    return false;
  }

  dcm::Writer writer(&stream);
  dcm::WriteVisitor v(&writer);

  Accept(v);

  return true;
}

}  // namespace dcm
