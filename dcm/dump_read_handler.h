#ifndef DCM_DUMP_READER_HANDLER_H_
#define DCM_DUMP_READER_HANDLER_H_

#include <iosfwd>

#include "dcm/read_handler.h"

namespace dcm {

// A read handler to print each data element.
class DumpReadHandler : public ReadHandler {
public:
  explicit DumpReadHandler(std::ostream& os, const char* indent = "  ");

  ~DumpReadHandler() override = default;

  bool OnElementStart(Tag tag) override;
  void OnElementEnd(DataElement* data_element) override;

  void OnSequenceStart(DataSequence* data_sequence) override;
  void OnSequenceEnd() override;

  void OnSequenceItemStart(DataElement* data_element) override;
  void OnSequenceItemEnd() override;

private:
  void PrintIndent();

private:
  std::ostream& os_;
  std::string indent_;
  int level_ = 0;
};

}  // namespace dcm

#endif  // DCM_DUMP_READER_HANDLER_H_
