#ifndef DCMLITE_VISITOR_H_
#define DCMLITE_VISITOR_H_
#pragma once

#include <iosfwd>

namespace dcmlite {

class BinaryFile;
class DataElement;
class DataSet;

////////////////////////////////////////////////////////////////////////////////

// Visitor interface.
class Visitor {
public:
  virtual ~Visitor() {
  }

  virtual void VisitDataElement(DataElement* data_element) = 0;
  virtual void VisitDataSet(DataSet* data_set) = 0;
};

////////////////////////////////////////////////////////////////////////////////

// A visitor to print the data set recursively.
// Usage:
//   PrintVisitor v(std::cout);
//   data_set.Accept(v);
class PrintVisitor : public Visitor {
public:
  explicit PrintVisitor(std::ostream& os);

  virtual ~PrintVisitor() {
  }

  void VisitDataElement(DataElement* data_element) override;
  void VisitDataSet(DataSet* data_set) override;

private:
  std::ostream& os_;
  int level_;
};

////////////////////////////////////////////////////////////////////////////////

// A visitor to write the data set to file.
// Usage:
//   std::ofstream ofs("path/to/file");
//   WriteVisitor v(ofs);
//   data_set.Accept(v);
class WriteVisitor : public Visitor {
public:
  explicit WriteVisitor(BinaryFile* file);

  ~WriteVisitor() override {
  }

  void VisitDataElement(DataElement* data_element) override;
  void VisitDataSet(DataSet* data_set) override;

private:
  bool explicit_vr_;  // Of current visited data set.
  int level_;

  BinaryFile* file_;
};

}  // namespace dcmlite 

#endif  // DCMLITE_VISITOR_H_
