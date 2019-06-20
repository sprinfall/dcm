#ifndef APPS_PRINT_H_
#define APPS_PRINT_H_

#include <iosfwd>

namespace dcm {
class DataElement;
}

// TODO: Add options to allow to customize the output format.

// Print the value of data element to an output stream.
void PrintValue(std::ostream& os, const dcm::DataElement* element);

// Print data element to an output stream.
void Print(std::ostream& os, const dcm::DataElement* element);

#endif  // APPS_PRINT_H_
