#include "apps/print.h"

#include <iostream>
#include <sstream>

#include "dcm/data_element.h"

void PrintValue(std::ostream& os, const dcm::DataElement* element) {
  using namespace dcm;

  if (element->tag() == dcm::tags::kSeqDelimatation ||
      element->tag() == dcm::tags::kSeqItemDelimatation ||
      element->tag() == dcm::tags::kSeqItemPrefix) {
    return;
  }

  switch (element->vr().code()) {
    case VR::AT:  // Attribute Tag
      os << "<TODO>";
      break;

    case VR::OD:
    case VR::OF:
    case VR::OL:
    case VR::OW:
      os << "<TODO>";
      break;

    case VR::AE:  // Application Entity
    case VR::AS:  // Age String
    case VR::CS:  // Code String
    case VR::DS:  // Decimal String
    case VR::SH:  // Short String
    case VR::LO:  // Long String
    case VR::ST:  // Short Text
    case VR::LT:  // Long Text
    case VR::UT:  // Unlimited Text
    case VR::IS:  // Integer String
    case VR::UC:  // Unlimited Characters
    case VR::UI:  // UID
    case VR::UR:  // URI/URL
    case VR::DA:  // Date
    case VR::TM:  // Time
    case VR::DT:  // Date Time
    case VR::PN:  // Person Name
    {
      // TODO: Set max length to display.
      std::string str;
      element->GetString(&str);
      os << str;
    }
    break;

    case VR::US:  // Unsigned Short
    {
      std::uint16_t value = 0;
      element->GetUint16(&value);
      os << value;
    }
    break;

    case VR::SS:  // Signed Short
    {
      std::int16_t value = 0;
      element->GetInt16(&value);
      os << value;
    }
    break;

    case VR::UL:  // Unsigned Long
    {
      std::uint32_t value = 0;
      element->GetUint32(&value);
      os << value;
    }
    break;

    case VR::SL:  // Signed Long
    {
      std::int32_t value = 0;
      element->GetInt32(&value);
      os << value;
    }
    break;

    case VR::FL:  // Floating Point Single
    {
      float32_t value = 0.0;
      element->GetFloat32(&value);
      os << value;
    }
    break;

    case VR::FD:  // Floating Point Double
    {
      float64_t value = 0.0;
      element->GetFloat64(&value);
      os << value;
    }
    break;

    default:
      os << "<TODO>";
      break;
  }
}

void Print(std::ostream& os, const dcm::DataElement* element) {
  element->tag().Print(os);

  const char* TAB = "\t";

  auto vr = element->vr();

  os << TAB << vr.byte1() << vr.byte2() << TAB;

  if (element->length() != dcm::kUndefinedLength) {
    os << element->length();
  } else {
    os << "-1";
  }

  os << TAB;

  PrintValue(os, element);
}
