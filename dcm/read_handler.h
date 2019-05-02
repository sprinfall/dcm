#ifndef DCM_READER_HANDLER_H_
#define DCM_READER_HANDLER_H_

#include <vector>

#include "dcm/defs.h"
#include "dcm/tag.h"

namespace dcm {

class DataElement;
class DataSet;

// -----------------------------------------------------------------------------

// An interface for parsing DICOM files in a SAX (Simple API for XML) style.
class ReadHandler {
public:
  virtual ~ReadHandler() = default;

  bool should_stop() const { return should_stop_; }

  virtual void OnEndian(Endian endian) {}
  virtual void OnExplicitVR(bool explicit_vr) {}

  // The return value indicates whether OnElementEnd will be called.
  virtual bool OnElementStart(Tag tag) = 0;

  // The data element was newly allocated and should be deleted by read handler.
  virtual void OnElementEnd(DataElement* data_element) = 0;

  virtual void OnSeqElementStart(DataSet* data_set) = 0;
  virtual void OnSeqElementEnd(DataSet* data_set) = 0;

protected:
  ReadHandler() = default;

protected:
  // Set this flag to stop the read process.
  bool should_stop_ = false;
};

// -----------------------------------------------------------------------------

// A read handler to print each data element.
class DumpReadHandler : public ReadHandler {
public:
  DumpReadHandler();
  virtual ~DumpReadHandler();

  virtual bool OnElementStart(Tag tag) override;
  virtual void OnElementEnd(DataElement* data_element) override;

  virtual void OnSeqElementStart(DataSet* data_set) override;
  virtual void OnSeqElementEnd(DataSet* data_set) override;

private:
  int level_;
};

// -----------------------------------------------------------------------------

// A read handler to insert all data elements into a given data set.
class FullReadHandler : public ReadHandler {
public:
  //\param The destination data set.
  explicit FullReadHandler(DataSet* data_set);

  virtual ~FullReadHandler() {
  }

  virtual void OnEndian(Endian endian) override;
  virtual void OnExplicitVR(bool explicit_vr) override;

  virtual bool OnElementStart(Tag tag) override {
    return true;
  }

  virtual void OnElementEnd(DataElement* data_element) override;

  virtual void OnSeqElementStart(DataSet* data_set) override;
  virtual void OnSeqElementEnd(DataSet* data_set) override;

private:
  DataSet* data_set_;
  std::vector<DataSet*> data_set_stack_;
};

// -----------------------------------------------------------------------------

// A read handler to read data elements only for the specific tags.
// It avoids loading the full data set, so it's very fast.
class TagsReadHandler : public ReadHandler {
public:
  explicit TagsReadHandler(DataSet* data_set);
  virtual ~TagsReadHandler();

  virtual void OnEndian(Endian endian) override;
  virtual void OnExplicitVR(bool explicit_vr) override;

  virtual bool OnElementStart(Tag tag) override;
  virtual void OnElementEnd(DataElement* data_element) override;

  virtual void OnSeqElementStart(DataSet* data_set) override;
  virtual void OnSeqElementEnd(DataSet* data_set) override;

  // Add a tag to read.
  TagsReadHandler& AddTag(Tag tag);

  // Delete all tags.
  void ClearTags();

private:
  // Tags (sorted) to read.
  std::vector<Tag> tags_;

  DataSet* data_set_;
  std::vector<DataSet*> data_set_stack_;
};

}  // namespace dcm

#endif  // DCM_READER_HANDLER_H_
