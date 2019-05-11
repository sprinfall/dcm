#ifndef EXAMPLES_PRINT_H_
#define EXAMPLES_PRINT_H_

#include <iosfwd>

namespace dcm {

class DataElement;

}  // namespace dcm

// Print data element to an output stream.
void Print(std::ostream& os, const dcm::DataElement* element);

#endif  // EXAMPLES_PRINT_H_
