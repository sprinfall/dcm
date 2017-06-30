#ifndef DCMLITE_READER_HANDLER_H_
#define DCMLITE_READER_HANDLER_H_
#pragma once

#include <vector>

namespace dcmlite {

class DataElement;
class DataSet;
class Tag;

////////////////////////////////////////////////////////////////////////////////

// An interface for parsing DICOM files in a SAX (Simple API for XML) style.
class ReadHandler {
public:
  virtual ~ReadHandler() {
  }

  bool should_stop() const {
    return should_stop_;
  }

  // About the return value:
  // - true: OnElementEnd will be called with a new allocated data element;
  // - false: OnElementEnd won't be called (this element will be skipped).
  virtual bool OnElementStart(const Tag& tag) = 0;

  virtual void OnElementEnd(DataElement* data_element) = 0;

  virtual void OnSeqElementStart(DataSet* data_set) = 0;
  virtual void OnSeqElementEnd(DataSet* data_set) = 0;

protected:
  ReadHandler() : should_stop_(false) {
  }

protected:
  // Set this flag to stop the read process.
  bool should_stop_;
};

////////////////////////////////////////////////////////////////////////////////

// A read handler to print each data element.
class DumpReadHandler : public ReadHandler {
public:
  DumpReadHandler();
  virtual ~DumpReadHandler();

  virtual bool OnElementStart(const Tag& tag) override;
  virtual void OnElementEnd(DataElement* data_element) override;

  virtual void OnSeqElementStart(DataSet* data_set) override;
  virtual void OnSeqElementEnd(DataSet* data_set) override;

private:
  int level_;
};

////////////////////////////////////////////////////////////////////////////////

// A read handler to insert all data elements into a given data set.
class FullReadHandler : public ReadHandler {
public:
  //\param The destination data set.
  explicit FullReadHandler(DataSet* data_set);

  virtual ~FullReadHandler() {
  }

  virtual bool OnElementStart(const Tag& tag) override {
    return true;
  }

  virtual void OnElementEnd(DataElement* data_element) override;

  virtual void OnSeqElementStart(DataSet* data_set) override;
  virtual void OnSeqElementEnd(DataSet* data_set) override;

private:
  DataSet* data_set_;
  std::vector<DataSet*> data_set_stack_;
};

////////////////////////////////////////////////////////////////////////////////

// A read handler to read data elements only for the specific tags.
// It avoids loading the full data set, so it's very fast.
class TagsReadHandler : public ReadHandler {
public:
  TagsReadHandler();
  virtual ~TagsReadHandler();

  virtual bool OnElementStart(const Tag& tag) override;
  virtual void OnElementEnd(DataElement* data_element) override;

  virtual void OnSeqElementStart(DataSet* data_set) override;
  virtual void OnSeqElementEnd(DataSet* data_set) override;

  // Add a tag to read.
  void AddTag(const Tag& tag);

  // Get a result data element by tag.
  DataElement* GetElement(const Tag& tag) const;

  size_t GetElementSize() const {
    return elements_.size();
  }

  // Delete all tags & data elements.
  void Clear();

private:
  // Tags (sorted) to read.
  std::vector<Tag> tags_;

  // Result data elements.
  std::vector<DataElement*> elements_;
};

}  // namespace dcmlite

#endif  // DCMLITE_READER_HANDLER_H_
