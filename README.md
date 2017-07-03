# dcmlite
A lightweight C++ DICOM library.

Currently, only support to read DICOM files.

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
dcmlite::DataSet data_set;
dcmlite::TagsReadHandler read_handler(&data_set);

// Add tags to read.
read_handler.AddTag(dcmlite::Tag(0x0002, 0x0010)).
    AddTag(dcmlite::Tag(0x0010, 0x0010)).
    AddTag(dcmlite::Tag(0x0028, 0x0002));

dcmlite::DicomReader reader(&read_handler);
reader.ReadFile("path/to/some/dcm")

// Get value from data set.
// ...
```
