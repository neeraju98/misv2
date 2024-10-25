// iirIO.h
//
#pragma once
#ifndef __iris_image_record_h
#define __iris_image_record_h

//enum eImageType {
#define IMAGE_TYPE_UNCROPPED          (1) // (0x01)
#define IMAGE_TYPE_VGA                (2) // (0x02)
#define IMAGE_TYPE_CROPPED            (3) // (0x03)
#define IMAGE_TYPE_CROPPED_AND_MASKED (7) // (0x07)
//};

//enum eIrisSubtype {
#define EYE_UNDEF (0)
#define EYE_RIGHT (1)
#define EYE_LEFT  (2)
//};

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;


typedef struct { // 16
  //const static int size = 16;
  uint8  FormatId[4];                    // 4 bytes           0x49495200      // "IIR", "FIR", "FMR"
  uint8  FormatVersion[4];               // 4 bytes           0x30323000
  uint32 RecordLength;                   // 4 bytes           69 to 2^32-1
} IirCommonHeader;

typedef struct { // 45
  //const static int size = 45;
  IirCommonHeader ich;

  uint16 CaptureDeviceId;                 // Capture device ID                Capture device ID assigned by vendor of equipment used to capture image data
                                          //                                  CAPTURE_DEVICE_UNDEF = 0
  uint8  NumberOfEyes;                    // No. of iris biometric subtypes   Number of eyes imaged, 1 or 2.
  uint16 RecordHeaderLength; // = 45;     // Record header length             Currently 45 bytes
  uint16 IrisProperties; // bitfield      // Iris image properties bitfield   1-2: Horizontal orientation
                                          //                                    ORIENTATION_UNDEF = 0
                                          //                                    ORIENTATION_BASE = 1
                                          //                                    ORIENTATION_FLIPPED = 2
                                          //                                  3-4: Vertical orientation
                                          //                                    ORIENTATION_UNDEF = 0
                                          //                                    ORIENTATION_BASE = 1
                                          //                                    ORIENTATION_FLIPPED = 2
                                          //                                  5-6: Scan type (rectilinear only)
                                          //                                    SCAN_TYPE_CORRECTED = 0
                                          //                                    SCAN_TYPE_PROGRESSIVE = 1
                                          //                                    SCAN_TYPE_INTERLACE_FRAME = 2
                                          //                                    SCAN_TYPE_INTERLACE_FIELD = 3
                                          //                                  7: Iris occlusions (polar only)
                                          //                                    IROCC_UNDEF = 0
                                          //                                    IROCC_PROCESSED = 1
                                          //                                  8: Occlusion filling (polar only)
                                          //                                    IROCC_ZEROFILL = 0
                                          //                                    IROC_UNITFILL = 1
                                          //                                  9: Boundary extraction (polar only)
                                          //                                    IRBNDY_UNDEF = 0
                                          //                                    IRBNDY_PROCESSED = 1
  uint16 IrisDiameter;                    // Iris diameter                    Expected iris diameter in pixels (rectilinear only)
  uint16 ImageFormat;                     // Image format                     Image format of data blob (JPEG, raw, etc.):
                                          //                                    IMAGEFORMAT_MONO_RAW = 2 (0x0002)
                                          //                                    IMAGEFORMAT_RGB_RAW = 4 (0x0004)
                                          //                                    IMAGEFORMAT_MONO_JPEG = 6 (0x0006)
                                          //                                    IMAGEFORMAT_RGB_JPEG = 8 (0x0008
                                          //                                    IMAGEFORMAT_MONO_JPEG_LS = 10 (0x000A)
                                          //                                    IMAGEFORMAT_RGB_JPEG_LS = 12 (0x000C)
                                          //                                    IMAGEFORMAT_MONO_JPEG2000 = 14 (0x000E)
                                          //                                    IMAGEFORMAT_RGB_JPEG2000 = 16 (0x0010)
  uint16 Width;                           // Raw image width                  Raw image width, pixels; WIDTH_UNDEF = 0
  uint16 Height;                          // Raw image height                 Raw image height, pixels; HEIGHT_UNDEF = 0
  uint8  IntensityDepth;                  // Intensity depth                  Intensity depth, bits, per colour; INTENSITY_DEPTH_UNDEF = 0
  uint8  ImageTranform;                   // Image transformation             Transformation to polar image (polar only)
                                          //                                    TRANS_UNDEF = 0
                                          //                                    TRANS_STD = 1
  uint8  UniqueDeviceID[16];              // Device unique identifier         A 16 character string uniquely identifying the device or source of the data. This data can be one of:
                                          //                                    Device Serial number, identified by the first character "D"
                                          //                                    Host PC Mac address, identified by the first character "M"
                                          //                                    Host PC processor ID, identified by the first character "P"
                                          //                                    No serial number, identified by all zeros
} IirRecordHeader;

typedef struct { // 3
  //const static int size = 3;
  uint8  BiometricSubtype;                // Biometric subtype      Biometric subtype identifier
                                          //                          EYE_UNDEF = 0 (0x00)
                                          //                          EYE_RIGHT = 1 (0x01)
                                          //                          EYE_LEFT = 2 (0x02)
  uint16 NumberOfImages;                  // No. images             Number of iris images for this biometric subtype, 1-65,535
} IirSubtypeHeader;

typedef struct { // 11
  //const static int size = 11;
  uint16 ImageNumber;                     // Image number           Image sequence number, 1  No. Images
  uint8  Quality;                         // Quality                Image quality value
  uint16 RotationAngle;                   // Rotation angle of eye  Rotation angle = (signed short) round (65536*angle/360) modulo 65536
                                          //                        ROT_ANGLE_UNDEF = 0xFFFF
                                          //                        Where angle is measured in degrees from horizontal
                                          //                        Used only for rectilinear images. For polar images entry shall be ROT_ANGLE_UNDEF
  uint16 RotationUncertainty;             // Rotation uncertainty   Rotation uncertainty = (unsigned short) round (65536*uncertainty/180) Where 0 ?? uncertainty < 180
                                          //                        ROT_UNCERTAIN_UNDEF = 0xFFFF
                                          //                        Where uncertainty is measured in degrees and is the absolute value of maximum error
  uint32 ImageLength;                     // Image length           Size of image data, bytes, 0 - 4294967295
} IirImageHeader;

//enum eImageFormat2005 {
#define Iir2005_IMAGEFORMAT_MONO_RAW      (2)  // (0x0002)
#define Iir2005_IMAGEFORMAT_RGB_RAW       (4)  // (0x0004)
#define Iir2005_IMAGEFORMAT_MONO_JPEG     (6)  // (0x0006)
#define Iir2005_IMAGEFORMAT_RGB_JPEG      (8)  // (0x0008)
#define Iir2005_IMAGEFORMAT_MONO_JPEG_LS  (10) // (0x000A)
#define Iir2005_IMAGEFORMAT_RGB_JPEG_LS   (12) // (0x000C)
#define Iir2005_IMAGEFORMAT_MONO_JPEG2000 (14) // (0x000E)
#define Iir2005_IMAGEFORMAT_RGB_JPEG2000  (16) // (0x0010)
//};

const static int IirHeader2005_size = 59;
typedef struct { // 59
    IirRecordHeader  irh;
    IirSubtypeHeader ish;
    IirImageHeader   iih;
} IirHeader2005;

typedef struct {

  IirHeader2005 iirh;
  uint8* p;
} Iir2005;

typedef struct { // 16
  //const static int size = 16;
  IirCommonHeader ich;
  uint16 NumberOfIrises;                 // 2 bytes           1 ... 65 535
  uint8  CertificationFlag;              // 1 byte            0x00
  uint8  NumberOfEyes;                   // 1 byte            0, 1, 2
} IirGeneralHeader;

const static int IirRepresentationHeader_size = 52;
typedef struct { // 52 (53 ?) ...
  uint32 RepresentationLength;            // 4 bytes          53 to (( 2^32 ?1) 16)
  uint8  CaptureDateAndTime[9];           // 9 bytes          See ISO/IEC 19794-1, clause 12.3.2; EXAMPLE: Thursday 17:35:20 December 15, 2005 is encoded as : 0x07 D50C 0F11 2314 FFFF
  uint8  CaptureDeviceTechnology;         // 1 byte           0 (0x00 ): Unknown or Unspecified
                                          //                  1 (0x01 ): CMOS/CCD
  uint16 CaptureDeviceVendorID;           // 2 bytes          0x0000 (Unspecified) or Registered Value (IBIA or otherwise)
  uint16 CaptureDeviceTypeID;             // 2 bytes          0x0000 (Unspecified) or Registered Value (IBIA or otherwise)
  uint8  QualityBlock[1];                 // 1 to n bytes     See ISO/IEC 19794-1
  uint16 RepresentationNumber;            // 2 bytes          1 to ... number of representations < 65 536
  uint8  EyeLabel;                        // 1 byte           SUBJECT_EYE_LABEL_UNDEF = 0 (0x00)
                                          //                  SUBJECT_EYE_LABEL_RIGHT = 1 (0x01)
                                          //                  SUBJECT_EYE_LABEL_LEFT = 2 (0x02)
  uint8  ImageType;                       // 1 byte           IMAGE_TYPE_UNCROPPED = 1 (0x01)
                                          //                  IMAGE_TYPE_VGA = 2 (0x02)
                                          //                  IMAGE_TYPE_CROPPED = 3 (0x03)
                                          //                  IMAGE_TYPE_CROPPED_AND_MASKED = 7 (0x07)
  uint8  ImageFormat;                     // 1 byte           IMAGEFORMAT_MONO_RAW = 2 (0x02)
                                          //                  IMAGEFORMAT_MONO_JPEG2000 = 10 (0x0A)
                                          //                  IMAGEFORMAT_MONO_PNG = 14 (0x0E)
  uint8  IrisProperties;                  // 1 byte           Bits 1-2, i.e. least significant bits:  // Horizontal orientation
                                          //                    ORIENTATION_UNDEF = 0
                                          //                    HORZ_ORIENTATION_BASE = 1
                                          //                    HORZ_ORIENTATION_FLIPPED = 2
                                          //                  Bits 3-4:                               // Vertical orientation
                                          //                    ORIENTATION_UNDEF = 0
                                          //                    VERT_ORIENTATION_BASE = 1
                                          //                    VERT_ORIENTATION_FLIPPED = 2
                                          //                  Bits 5-6: 0,0                           // Reserved by SC 37 for future use; default to 0,0
                                          //                  Bits 7-8:                               // Compression history
                                          //                    PREVIOUS_COMPRESSION_UNDEF = 0
                                          //                    PREVIOUS_COMPRESSION_LOSSLESS_OR_NONE = 1
                                          //                    PREVIOUS_COMPRESSION_LOSSY = 2
  uint16 Width;                           // 2 bytes          > 0
  uint16 Height;                          // 2 bytes          > 0
  uint8  BitDepth;                        // 1 byte           At least 8
  uint16 Range;                           // 2 bytes          2 to (2^16)
                                          //                  RANGE_UNASSIGNED = 0
                                          //                  RANGE_FAILED = 1
                                          //                  RANGE_OVERFLOW = 2^16
  uint16 RollAngle;                       // 2 bytes          0 to 65 534; ROLL_ANGLE_UNDEF = 65 535
  uint16 RollUncertainty;                 // 2 bytes          0 to 65 534; ROLL_UNCERTAIN_UNDEF = 65 535
  uint16 IrisCentreX_Min;                 // 2 bytes          1 to 65 535; COORDINATE_UNDEF = 0
  uint16 irisCentreX_Max;                 // 2 bytes          1 to 65 535; COORDINATE_UNDEF = 0
  uint16 IrisCentreY_Min;                 // 2 bytes          1 to 65 535; COORDINATE_UNDEF = 0
  uint16 IrisCentreY_Max;                 // 2 bytes          1 to 65 535; COORDINATE_UNDEF = 0
  uint16 IrisDiameterMin;                 // 2 bytes          1 to 65 535; COORDINATE_UNDEF = 0
  uint16 IrisDiameterMax;                 // 2 bytes          1 to 65 535; COORDINATE_UNDEF = 0
  uint32 ImageLength;                     // 4 bytes          1 to 4 294 967 226
} IirRepresentationHeader;

//enum eImageFormat2011 {
#define Iir2011_IMAGEFORMAT_MONO_RAW       (2)  // (0x02)
#define Iir2011_IMAGEFORMAT_MONO_JPEG2000  (10) // (0x0A)
#define Iir2011_IMAGEFORMAT_MONO_PNG       (14) // (0x0E)
//};

const static int IirHeader2011_size = 68;
typedef struct { // 68
    IirGeneralHeader  igh;
    IirRepresentationHeader irh;
} IirHeader2011;

typedef struct {
  IirHeader2011 iirh;
  uint8* p;
} Iir2011;

//enum eIirType {
#define UNDEFINED (0)
#define NOT_IIR   (1)
#define IIR_2005  (2)
#define IIR_2011  (3)
//};

typedef struct {
  IirCommonHeader ich;
  uint16 IirType; // uint8 ? // eIirType

// Iir2011
// IirGeneralHeader
  uint16 NumberOfIrises;
  uint8  CertificationFlag;
  uint8  NumberOfEyes;

// IirRepresentationHeader
  uint32 RepresentationLength;
  uint8  CaptureDateAndTime[9];
  uint8  CaptureDeviceTechnology;
  uint16 CaptureDeviceVendorID;
  uint16 CaptureDeviceTypeID;
  uint8  QualityBlock[1];
  uint16 RepresentationNumber;
  uint8  EyeLabel;
  uint8  ImageType;       // Kind 1, 2, 3, 7
  uint16 ImageFormat;     // uint8 ??
  uint16 IrisProperties;  // uint8 ??
  uint16 Width;
  uint16 Height;
  uint8  BitDepth;
  uint16 Range;
  uint16 RollAngle;
  uint16 RollUncertainty;
  uint16 IrisCentreX_Min;
  uint16 irisCentreX_Max;
  uint16 IrisCentreY_Min;
  uint16 IrisCentreY_Max;
  uint16 IrisDiameterMin;
  uint16 IrisDiameterMax;
  uint32 ImageLength;

  // Iir2005
// IirRecordHeader
  uint16 CaptureDeviceId;
//  uint8  NumberOfEyes;
  uint16 RecordHeaderLength;
//  uint16 IrisProperties;
//  uint16 IrisDiameter; //  IrisDiameterMin; // IrisDiameterMax;
//  uint16 ImageFormat;
//  uint16 Width;
//  uint16 Height;
//  uint8  IntensityDepth; //  uint8  BitDepth;
  uint8  ImageTranform;
  uint8  UniqueDeviceID[16];

// IirSubtypeHeader
//  uint8  BiometricSubtype;  //  uint8  EyeLabel;
  uint16 NumberOfImages;

// IirImageHeader
  uint16 ImageNumber;
  uint8  Quality;
//  uint16 RotationAngle;         //  uint16 RollAngle;
//  uint16 RotationUncertainty;   //  uint16 RollUncertainty;
//  uint32 ImageLength;

  int numcmpts;
  int width;
  int height;
  int depth;
  int fmtid;
  int rawsize;
  char fmtname[10];
} IirInfo;

#ifdef __cplusplus
extern "C" {
#endif

int iirUnpack(const uint8* buf, uint32 n, IirInfo * ii, uint32 nSizeReq, uint8* pout, uint32* nSizeOut);
int iirPack(const uint8* buf, uint32 nWidth, uint32 nHeight, uint32 nBpp, uint32 nSize, const IirInfo* ii, char *fmtname, uint32 nSizeReq, uint8* pout, uint32* nSizeOut);

#ifdef __cplusplus
}  /* end of extern "C" */
#endif /* __cplusplus */

#endif // __iris_image_record_h
