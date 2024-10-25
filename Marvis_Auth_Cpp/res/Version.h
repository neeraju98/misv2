#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

#ifndef VERSION_MAJOR
#define VERSION_MAJOR               2
#endif

#ifndef VERSION_MINOR
#define VERSION_MINOR               0
#endif

#ifndef VERSION_REVISION
#define VERSION_REVISION            0
#endif

#ifndef VERSION_BUILD
#define VERSION_BUILD               0
#endif

#ifndef VER_FILE_DESCRIPTION_STR
#define VER_FILE_DESCRIPTION_STR    "Marvis Auth Native SDK"
#endif

#define VER_FILE_VERSION            VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD
#define VER_FILE_VERSION_STR        STRINGIZE(VERSION_MAJOR)        \
                                    "." STRINGIZE(VERSION_MINOR)    \
                                    "." STRINGIZE(VERSION_REVISION) \
                                    "." STRINGIZE(VERSION_BUILD)    \

#ifndef VER_PRODUCTNAME_STR
#define VER_PRODUCTNAME_STR         "Marvis_Auth_Cpp"
#endif // VER_PRODUCTNAME_STR


#define VER_PRODUCT_VERSION         VER_FILE_VERSION
#define VER_PRODUCT_VERSION_STR     VER_FILE_VERSION_STR

#ifndef VER_ORIGINAL_FILENAME_STR
#define VER_ORIGINAL_FILENAME_STR   "Marvis_Auth_Cpp.exe"
#endif

#define VER_INTERNAL_NAME_STR       VER_ORIGINAL_FILENAME_STR

#define VER_COPYRIGHT_STR           "(c) Mantra Softech (India) Pvt. Ltd. All rights reserved."

#ifdef _DEBUG
#define VER_VER_DEBUG             VS_FF_DEBUG
#else
#define VER_VER_DEBUG             0
#endif

#define VER_FILEOS                  VOS_NT_WINDOWS32
#define VER_FILEFLAGS               VER_VER_DEBUG
#define VER_FILETYPE                VFT_APP
