# dcm

A lightweight C++ DICOM library for reading and writing DICOM files.

## Usage

### Read Full Data Set

```cpp
dcm::DataSet data_set;
dcm::FullReadHandler read_handler(&data_set);
dcm::DicomReader reader(&read_handler);
reader.ReadFile("path/to/some/dcm");
```

With the data set, you can get tags.

Read string:
```cpp
std::string transfer_syntax_uid;
if (data_set.GetString(0x00020010, &transfer_syntax_uid)) {
  std::cout << "Transfer Syntax UID: " << transfer_syntax_uid << std::endl;
}

std::string patient_name;
if (data_set.GetString(0x00100010, &patient_name)) {
  std::cout << "Patient Name: " << patient_name << std::endl;
}
```

Read integer:
```cpp
std::uint16_t samples_per_pixel;
if (data_set.GetUint16(0x00280002, &samples_per_pixel)) {
  std::cout << "Samples Per Pixel: " << samples_per_pixel << std::endl;
}
```

### Read Specific Tags

Much less memory allocation thus much faster.

```cpp
dcm::DataSet data_set;
dcm::TagsReadHandler read_handler(&data_set);

// Add tags to read.
read_handler.AddTag(0x00020010).AddTag(0x00100010).AddTag(0x00280002);

dcm::DicomReader reader(&read_handler);
reader.ReadFile("path/to/some/dcm")

// Get value from data set.
// ...
```

## Write

Coming soon...
