#ifndef DCM_DICOM_WRITER_H_
#define DCM_DICOM_WRITER_H_

#include "dcm/defs.h"

namespace dcm {

class DataSet;

class DicomWriter {
public:
  explicit DicomWriter(DataSet* data_set)
      : data_set_(data_set) {
  }

  // Write the data set to file.
  bool WriteFile(const Path& path) const;

private:
  DataSet* data_set_;
};

}  // namespace dcm

#endif  // DCM_DICOM_WRITER_H_
