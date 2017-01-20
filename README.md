# dcmlite
A lightweight C++ DICOM library.

It's at the very beginning of the development. Don't try to use it in production.

## Read File

```cpp
dcmlite::DataSet data_set;
data_set.LoadFile("path/to/some/dcm");
```

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

Example output:
```
Transfer Syntax UID: 1.2.840.10008.1.2.1
Patient Name : Gu^Adam
Samples Per Pixel : 1
```

