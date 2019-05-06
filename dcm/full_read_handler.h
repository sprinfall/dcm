#ifndef DCM_FULL_READER_HANDLER_H_
#define DCM_FULL_READER_HANDLER_H_

#include <stack>

#include "dcm/read_handler.h"

namespace dcm {

// A read handler to insert all data elements into a given data set.
class FullReadHandler : public ReadHandler {
public:
  //\param data_set The destination data set.
  explicit FullReadHandler(DataSet* data_set);

  virtual ~FullReadHandler() override = default;

  void OnEndian(Endian endian) override;
  void OnExplicitVR(bool explicit_vr) override;

  bool OnElementStart(Tag tag) override;
  void OnElementEnd(DataElement* data_element) override;

  void OnSequenceStart(DataSequence* data_sequence) override;
  void OnSequenceEnd(DataElement* data_element = nullptr) override;

  void OnSequenceItemStart(DataElement* data_element) override;
  void OnSequenceItemEnd(DataElement* data_element = nullptr) override;

private:
  // Append data element to its parent data set.
  void AppendElement(DataElement* data_element);

private:
  DataSet* data_set_;  // Root data set
  std::stack<DataSequence*> sequence_stack_;
};

}  // namespace dcm

#endif  // DCM_FULL_READER_HANDLER_H_
