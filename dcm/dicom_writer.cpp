#include "dcm/dicom_writer.h"

#include "boost/filesystem/fstream.hpp"

#include "dcm/data_set.h"
#include "dcm/write_visitor.h"
#include "dcm/writer.h"

namespace bfs = boost::filesystem;

namespace dcm {

bool DicomWriter::WriteFile(const Path& path) const {
  bfs::ofstream stream(path, std::ios::binary);
  if (stream.bad()) {
    return false;
  }

  dcm::Writer writer(&stream);
  dcm::WriteVisitor v(&writer);

  data_set_->Accept(v);

  return true;
}

}  // namespace dcm
