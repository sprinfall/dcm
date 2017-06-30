#ifndef DCMLITE_VISITOR_H_
#define DCMLITE_VISITOR_H_
#pragma once

#include <iosfwd>

namespace dcmlite {

class DataElement;
class DataSet;

////////////////////////////////////////////////////////////////////////////////

// Visitor interface.
class Visitor {
public:
  virtual ~Visitor() {
  }

  virtual void VisitDataElement(const DataElement* data_element) = 0;
  virtual void VisitDataSet(const DataSet* data_set) = 0;
};

////////////////////////////////////////////////////////////////////////////////

// A visitor to print the whole data set.
// Usage:
//   dcmlite::PrintVisitor print_visitor(std::cout);
//   data_set.Accept(print_visitor);
class PrintVisitor : public Visitor {
public:
  explicit PrintVisitor(std::ostream& os);

  virtual ~PrintVisitor() {
  }

  virtual void VisitDataElement(const DataElement* data_element) override;
  virtual void VisitDataSet(const DataSet* data_set) override;

private:
  std::ostream& os_;
  int level_;
};

}  // namespace dcmlite 

#endif  // DCMLITE_VISITOR_H_
