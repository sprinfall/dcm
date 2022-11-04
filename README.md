# dcm

NOTE: This project is only for study purpose. Please consider to use DCMTK in your project. 

A lightweight C++ DICOM library for reading and writing DICOM files.

## Usage

### Read DICOM File

```cpp
dcm::DicomFile dicom_file("path/to/some/dcm");
if (!dicom_file.Load()) {
  std::cerr << "Failed to load the file." << std::endl;
  return;
}
```

Read string:
```cpp
std::string transfer_syntax_uid;
if (dicom_file.GetString(dcm::tags::kTransferSyntaxUID, &transfer_syntax_uid)) {
  // ...
}

std::string patient_name;
if (dicom_file.GetString(dcm::tags::kPatientName, &patient_name)) {
  // ...
}
```

Or:
```cpp
auto transfer_syntax_uid = dicom_file.GetString(dcm::tags::kTransferSyntaxUID);
auto patient_name = dicom_file.GetString(dcm::tags::kPatientName);
```

Read integer:
```cpp
std::uint16_t samples_per_pixel;
if (dicom_file.GetUint16(dcm::tags::kSamplesPerPixel, &samples_per_pixel)) {
  // ...
}
```

Or:
```cpp
auto samples_per_pixel = dicom_file.GetUint16(dcm::tags::kSamplesPerPixel, 0);

```
