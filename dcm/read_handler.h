#ifndef DCM_READER_HANDLER_H_
#define DCM_READER_HANDLER_H_

#include "dcm/defs.h"

namespace dcm {

class DataElement;
class DataSequence;
class DataSet;

// An interface for parsing DICOM files in a SAX (Simple API for XML) style.
class ReadHandler {
public:
  virtual ~ReadHandler() = default;

  bool should_stop() const { return should_stop_; }

  virtual void OnTransferSyntax(VR::Type vr_type, ByteOrder byte_order) = 0;

  // The return value indicates whether OnElementEnd will be called.
  virtual bool OnElementStart(Tag tag) = 0;

  // The data element was newly allocated and should be deleted by read handler.
  virtual void OnElementEnd(DataElement* data_element) = 0;

  virtual void OnSequenceStart(DataSequence* data_sequence) = 0;
  virtual void OnSequenceEnd(DataElement* data_element = nullptr) = 0;

  virtual void OnSequenceItemStart(DataElement* data_element) = 0;
  virtual void OnSequenceItemEnd(DataElement* data_element = nullptr) = 0;

protected:
  ReadHandler() = default;

protected:
  // Set this flag to stop the read process.
  bool should_stop_ = false;
};

}  // namespace dcm

#endif  // DCM_READER_HANDLER_H_
