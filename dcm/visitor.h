#ifndef DCM_VISITOR_H_
#define DCM_VISITOR_H_

namespace dcm {

class DataElement;
class DataSequence;
class DataSet;

// Visitor interface.
class Visitor {
public:
  virtual ~Visitor() = default;

  virtual void VisitDataElement(const DataElement* data_element) = 0;
  virtual void VisitDataSequence(const DataSequence* data_sequence) = 0;
  virtual void VisitDataSet(const DataSet* data_set) = 0;
};

}  // namespace dcm 

#endif  // DCM_VISITOR_H_
