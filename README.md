# dcmlite
A lightweight C++ DICOM library.

It's at the very beginning of the development. Don't try to use it in production.

## Read

### Read Full Data Set

```cpp
dcmlite::DataSet data_set;
dcmlite::FullReadHandler read_handler(&data_set);
dcmlite::DicomReader reader(&read_handler);
reader.ReadFile("path/to/some/dcm");
```

With the data set, you can get tags.

Read string:
```cpp
std::string transfer_syntax_uid;
if (data_set.GetString(dcmlite::Tag(0x0002, 0x0010), &transfer_syntax_uid)) {
  std::cout << "Transfer Syntax UID: " << transfer_syntax_uid << std::endl;
}

std::string patient_name;
if (data_set.GetString(dcmlite::Tag(0x0010, 0x0010), &patient_name)) {
  std::cout << "Patient Name: " << patient_name << std::endl;
}
```

Read integer:
```cpp
std::uint16_t samples_per_pixel;
if (data_set.GetUint16(dcmlite::Tag(0x0028, 0x0002), &samples_per_pixel)) {
  std::cout << "Samples Per Pixel: " << samples_per_pixel << std::endl;
}
```

### Read Specific Tags

Much less memory allcation. Much faster.

```cpp
dcmlite::Tag tag1(0x0002, 0x0010);
dcmlite::Tag tag2(0x0010, 0x0010);
dcmlite::Tag tag3(0x0028, 0x0002);

dcmlite::TagsReader tags_reader;
tags_reader.AddTag(tag1).AddTag(tag2).AddTag(tag3);
tags_reader.ReadFile("path/to/some/dcm")

dcmlite::DataElement* element1 = tags_reader.GetElement(tag1);
if (element1 != NULL) {
  std::cout << *element1 << std::endl;
}

// ...
```
