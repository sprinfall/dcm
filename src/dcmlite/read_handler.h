#ifndef DCMLITE_READER_HANDLER_H_
#define DCMLITE_READER_HANDLER_H_
#pragma once

#include <vector>

namespace dcmlite {

class DataElement;
class DataSet;

////////////////////////////////////////////////////////////////////////////////

// An interface for parsing DICOM files in a SAX (Simple API for XML) style.
class ReadHandler {
public:
  virtual ~ReadHandler() {
  }

  virtual bool OnElement(DataElement* data_element) = 0;

  virtual bool OnSeqElementStart(DataSet* data_set) = 0;
  virtual bool OnSeqElementEnd(DataSet* data_set) = 0;
};

////////////////////////////////////////////////////////////////////////////////

// A read handler to print each data element.
class DumpReadHandler : public ReadHandler {
public:
  DumpReadHandler() : indent_level_(0) {
  }

  virtual ~DumpReadHandler() {
  }

  virtual bool OnElement(DataElement* data_element) override;

  virtual bool OnSeqElementStart(DataSet* data_set) override;
  virtual bool OnSeqElementEnd(DataSet* data_set) override;

private:
  int indent_level_;
};

////////////////////////////////////////////////////////////////////////////////

// A read handler to insert all data elements into a given data set.
class FullReadHandler : public ReadHandler {
public:
  //\param The destination data set.
  explicit FullReadHandler(DataSet* data_set);

  virtual ~FullReadHandler() {
  }

  virtual bool OnElement(DataElement* data_element) override;

  virtual bool OnSeqElementStart(DataSet* data_set) override;
  virtual bool OnSeqElementEnd(DataSet* data_set) override;

private:
  DataSet* data_set_;
  std::vector<DataSet*> data_set_stack_;
};

}  // namespace dcmlite

#endif  // DCMLITE_READER_HANDLER_H_
