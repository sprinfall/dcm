#ifndef DCM_VISITOR_H_
#define DCM_VISITOR_H_

#include <iosfwd>

namespace dcm {

class DataElement;
class DataSet;
class Writer;

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

  ~PrintVisitor() override = default;

  void VisitDataElement(DataElement* data_element) override;
  void VisitDataSet(DataSet* data_set) override;

private:
  std::ostream& os_;
  int level_;
};

////////////////////////////////////////////////////////////////////////////////

// A visitor to write the data set to file.
// Usage:
//   WriteVisitor v("path/to/file");
//   data_set.Accept(v);
class WriteVisitor : public Visitor {
public:
  explicit WriteVisitor(Writer *writer) : writer_(writer) {}

  ~WriteVisitor() override = default;

  void VisitDataElement(DataElement* data_element) override;
  void VisitDataSet(DataSet* data_set) override;

private:
  Writer* writer_;

  // If the current visited data set is explicit VR or not.
  bool explicit_vr_ = false;

  int level_ = 0;
};

}  // namespace dcm 

#endif  // DCM_VISITOR_H_
