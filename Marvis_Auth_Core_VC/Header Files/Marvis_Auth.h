#ifndef __MARVIS_AUTH_H__
#define __MARVIS_AUTH_H__

#ifdef __cplusplus 
extern "C" {
#endif 

#define MARVIS __declspec(dllimport)

//#define FPS_DEVELOPEMENT_MODE

/** Max string length for device version info */

#define MIS100V2	"MIS100V2"

/*
Maximal size of generated ISO/IEC 19794-2 template (with only one iris view)
*/

typedef struct _MARVIS_DEVICE_INFO { 
char SerialNo[13];
char Firmware[13];
char Make[7];
char Model[12];
int Width;
int Height;
int DPI;
} MARVIS_DEVICE_INFO;

typedef enum _IMAGE_FORMAT {
    /** To get bmp image data */
    BMP = 0,
    /** To get raw image data */
    RAW = 1,
	/** To get K3 image data */
	K3 = 2,
	/** To get K7 image data */
	K7 = 3,
} IMAGE_FORMAT;

/**
 * @brief The _MARVIS_AUTH_LOG_LEVEL_ enum : enum for log levels
 * @note : used in method MARVIS_Auth_EnableLogs to set log Properties
 */
typedef enum _MARVIS_AUTH_LOG_LEVEL_
{
    /** log level 0 - no logs - default */
	MARVIS_AUTH_LOG_LEVEL_OFF = 0,

    /** log level 1 - for error messages */
    MARVIS_AUTH_LOG_LEVEL_ERROR = 1,

}MARVIS_AUTH_LOG_LEVEL;

/**
* @brief The _DEVICE_DETECTION_EVENT enumeration for device status
* @note : used in callback function DEVICE_DETECTION to set status of the device
*/
typedef enum _DEVICE_DETECTION_EVENT
{
	/** For device disconnected */
	EVENT_DISCONNECTED = 0,

	/** For device connected */
	EVENT_CONNECTED = 1,
} DEVICE_DETECTION_EVENT;


typedef struct _DEVICE_LIST {
char Model[12]; //device model
}DEVICE_LIST;

/**
* @brief MARVIS_ENROLL_IMAGE_DATA : Structure for Captured both Iris images parameter.
* @note : Use in callback function to send Captured images data and length for Left and Right Iris
*/
typedef struct _MARVIS_IMAGE_DATA
{
	/** Bmp Image Buffer for Left Iris*/
	unsigned char* m_BitmapImage;
	/** Bmp Image length for Left Iris*/
	int m_BitmapImageLen;
}MARVIS_IMAGE_DATA;

/**
* @brief _MARVIS_IMAGE_QUALITY : Structure for Image Quality.
* @note : Use in complete callback function to send final captured image for single Iris
*/
typedef struct _MARVIS_IMAGE_QUALITY
{
	/** Image Quality */
	int Quality;
	int X;
	int Y;
	int R;
	int pupil_blur;
	int pupil_mean;
	int iris_mean;
	float sclera_mean;

	int iris_diameter;
	int pupil_center_x;
	int pupil_center_y;
	int pupil_diameter;
	int percent_occlusion;
	float sharpness;
#ifdef FPS_DEVELOPEMENT_MODE
	char CaptureTime[50];
#endif
}MARVIS_IMAGE_QUALITY;

typedef enum _MARVIS_AUTH_LIB_ERROR_CODE_ {

	MARVIS_AUTH_E_SUCCESS = 0,							//Success
	MARVIS_AUTH_E_NO_DEVICE = -1307,           //No Device Present
	MARVIS_AUTH_E_INVALIDPARAM = -1101,           //Invalid Parameters
	MARVIS_AUTH_E_NULLPARAM = -1121,           //Null Parameters
	MARVIS_AUTH_E_MEMORY = -1120,           //Memory allocation failed
	MARVIS_AUTH_E_CORRUPT_SERIAL = -1314,           //Serial no currupted
	MARVIS_AUTH_E_UNKNOWN_SENSOR = -1142,           //Unknown Sensor
	MARVIS_AUTH_E_LOAD_FIRMWARE_FAILED = -1317,           //Firmware failed to load
	MARVIS_AUTH_E_NOT_INITIALIZED = -1309,           //Device Not Initialized
	MARVIS_AUTH_E_SYNC_PROBLEM = -1139,           //Sync Problem
	MARVIS_AUTH_E_OTHER = -1122,           //Other Error
	MARVIS_AUTH_E_TIMEOUT = -1140,           //Timeout
	MARVIS_AUTH_E_NOT_GOOD_IMAGE = -1115,           //Input image is not good
	MARVIS_AUTH_E_BLANKIMAGE = -1114,		//Image is blank or contains non-recognizable fingerprint
	MARVIS_AUTH_E_EXTRACTOR_INIT_FAILED = -1116,		//Extractor Library cannot Initialize
	MARVIS_AUTH_E_FILE_IO = -1117,		//Error occured while opening/reading file
	MARVIS_AUTH_E_BAD_LICENSE = -1129,		//Provided license is not valid, or no license was found
	MARVIS_AUTH_E_BAD_FORMAT = -1132,		//Unsupported Format
	MARVIS_AUTH_E_BAD_VALUE = -1133,		//Invalid Value Provided
	MARVIS_AUTH_E_BAD_TEMPLATE = -1135,		//Invalide template or unsupported template format
	MARVIS_AUTH_E_READ_ONLY = -1136,		//Value cannot be modified
	MARVIS_AUTH_E_NOT_DEFINED = -1137,		//Value is not defined
	MARVIS_AUTH_E_NULL_TEMPLATE = -1138,		//Template is NULL (contains no iris)
	MARVIS_AUTH_E_NOT_IMPLEMENTED = -1134,           //Function not implemented
	MARVIS_AUTH_E_BAD_QUALITY = -1318,           //Bad Quality Finger
	MARVIS_AUTH_E_CAPTURING_STOPPED = -1319,           //Capturing stopped
	MARVIS_AUTH_E_LATENT_FINGER = -1320,           //latent finger on device
	MARVIS_AUTH_E_ALREADY_START_STOP = -1321,			//Already start or already stopped
	MARVIS_AUTH_E_INVALID_REQUEST = -1001,		    //Unhandled Exception
	MARVIS_AUTH_E_EVOLUTION_PERIOD_EXPIRED = -1,		    //Unhandled Exception
	MARVIS_AUTH_E_REGISTER_CALLBACK = -2001,		//failed to Register Callback
	MARVIS_AUTH_E_EXCEPTION_OCCURRED = -2002,
	MARVIS_AUTH_E_CAPTURE_STOP = -2003,
	MARVIS_AUTH_E_CALLBACK_NULLPARAM = -2004,
	MARVIS_AUTH_E_MATCH_IMAGE_FAILED = -2005,
	MARVIS_AUTH_E_FULL_FRAME_CORRUPTED = -2006,
	MARVIS_AUTH_E_FINAL_FRAME_CORRUPTED = -2007,
	MARVIS_AUTH_E_CROP_CORRUPTED = -2008,
	MARVIS_AUTH_E_COORDINATE_CORRUPTED = -2009,
	MARVIS_AUTH_E_FIRMWARE_CORRUPTED = -2010,
	MARVIS_AUTH_E_FLIP_DEVICE = -2011,
	MARVIS_AUTH_E_FAKE_IRISFOUND = -2012,
	MARVIS_AUTH_MODEL_FAKE=-2013,
	MARVIS_AUTH_E_ALREADY_INIT=-2014
} MARVIS_AUTH_LIB_ERROR_CODE;

/**
 * @brief MARVIS_Auth_InitDevice: This API will Init related library and device.
 * @param fd[In]: file descriptor for USB in case of Android, for other OS pass its value as -1.
 * @param pcProductName[In]: Product name for which library needs to be initialized.
 * @param ProductNameLen[In]: Product name string length.
 * @param StDeviceInfo[Out]: A structure pointer to DEVICE_INFO which will hold device related information while device initialized successfully.
 * Memory allocation for this structure will be done by user only.
 * @return: 0 for success, other if there is any error.
 */
MARVIS int MARVIS_Auth_InitDevice(int fd, const char* pcProductName, int ProductNameLen,MARVIS_DEVICE_INFO *StDeviceInfo);

/**
 * @brief MARVIS_Auth_IsDeviceConnected: This API will return device connection status.
 * @param fd[In]: file descriptor for USB in case of Android, for Linux pass its value as 0.
 * @param productName[In]: Product name for which library needs to be initialized.
 * @param productNameLen[In]: Product name string length.
 * @return: 0 in the case of device is connected,  other if there is any error.
 */
MARVIS int MARVIS_Auth_IsDeviceConnected(int fd, const char *pcProductName, int ProductNameLen);

/**
 * @brief MARVIS_Auth_UnInitDevice: This API will uninit device and internal parameters along with used libraries.
 * @return : 0 in case of success, other for failed or error.
 */
MARVIS int MARVIS_Auth_UninitDevice();

/**
* @brief DEVICE_DETECTION: Callback function for preview image
* @param DeviceName: Gives the error name of the device.
* @param dvcStatus: Status of the device - Connected/Disconnected from enum _DEVICE_DETECTION_EVENT.
*/
typedef void(*DEVICE_DETECTION)(const char* DeviceName, DEVICE_DETECTION_EVENT dvcStatus);

/**
* @brief MARVIS_Auth_RegisterDetectionCallback: This API will use for Device Auto Detection.
* @param p_DEVICE_ATTACH[Out]: A callback function which will raise on device attachment and detachment and provide details like Device Name and Event name.
* @return : 0 in case of success, other for failed or error.
* @note : It will provide only device status to the user (Connected/Disconnected).
*         This status is not updated in the device.
*         Also, if multiple devices of same vendor ID/Product ID are
attached,
*         and any one of it is disconnected, the status will be shown as connected until
*         there is at least one device connected.
*/
MARVIS int MARVIS_Auth_RegisterDetectionCallback(DEVICE_DETECTION p_DEVICE_ATTACH);

/**
* @brief PREVIEW_CALLBACK: This API will callback when capture preview.
* @param ErrorCode[Out]: 0 in case of success, other for failed or error.
* @param Quality[Out]: This is the quality level once achieved, device will stop capturing.
* @param ImageData[Out]: This gives bitmap Image information and image length.
*						Merrory allocated to rawdata will be de-allocated at library side. User is not required to free memory.
*/

typedef void(*PREVIEW_CALLBACK)(int ErrorCode, MARVIS_IMAGE_QUALITY *QtyInfo, MARVIS_IMAGE_DATA *PreviewImageData);

/**
* @brief CAPTURE_COMPLETE_CALLBACK: This API will callback when capture is completed.
* @param ErrorCode[Out]: 0 in case of success, other for failed or error.
* @param Quality[Out]: This is the quality level once achieved, device will stop capturing.
* @param ImageData[Out]: This gives bitmap Image information and image length.
*						Merrory allocated to rawdata will be de-allocated at library side. User is not required to free memory.
*/

typedef void(*CAPTURE_COMPLETE_CALLBACK)(int ErrorCode, MARVIS_IMAGE_QUALITY *QtyInfo, MARVIS_IMAGE_DATA *ImageData);

/**
 * @brief MARVIS_Auth_StartCapture: This API will start iris capturing.
 * @param Quality[In]: This is the min quality for capture image.
 * @param Timeout[In]: Timeout value at which device will timed out for capturing and capturing will be stopped.
 * @param p_CapturePreviewCallBack[Out]: A call back function which will provide details like error code, quality, iris image raw data for preview and data length, continuously.
 *  Merrory allocated to rawdata will be de-allocated at library side. User is not required to free memory.
 * @param p_CaptureCompleteCallBack[Out]: A call back function which will be triggered when desired quality level achieved. It will contain error code if any, quality, NFIQ for the
 * final image, raw data for iris image and data length.  Merrory allocated to raw data will be de-allocated at library side. User is not required to free memory.
 * int Capture_type = 0 Both Stop(Normal Capture) , 1 both (PWM and Model ) ,2 (only PWM) , 3 (Only Model)
  * @return : 0 in case of success, other for failed or error.
 */
MARVIS int MARVIS_Auth_StartCapture(int in_quality, int Timeout, PREVIEW_CALLBACK p_CapturePreviewCallBack, CAPTURE_COMPLETE_CALLBACK p_CaptureCompleteCallBack, char * emp_id,int Capture_type);

/**
 * @brief MARVIS_Auth_AutoCapture: This API will start iris capturing and will returned only when desired level of quality achieved.
  * @param minQuality[In]: This is the quality [lover-upper level with best of six image] once achieved, device will stop capturing.
 * @param upperThreshQty[In]: This is the quality upperlwvel level once achieved, device will stop capturing.
 * @param in_noOfFrameCash[In]: This is the no of frame cashing between lower and upper threshold, capturing best frame.
 * @param Timeout[In]: Timeout value at which device will timed out for capturing and capturing will be stopped.
 * @param Quality[Out]: This will hold Image frame quality of the captured iris.
 * @param p_CaptureCompleteCallBack[Out]: A call back function which will provide details like error code, quality, iris image raw data for preview and data length, continuously.
 * Merrory allocated to rawdata will be de-allocated at library side. User is not required to free memory.
 * @return : 0 in case of success, other for failed or error.
 */
MARVIS int MARVIS_Auth_AutoCapture_RD(int in_lowerThreshQty, int in_upperThreshQty, int in_noOfFrameCash, int Timeout, int *Quality, PREVIEW_CALLBACK p_MARVIS_AUTH_PREVIEW_CALLBACK, MARVIS_IMAGE_DATA *imageData);

/**
* @brief MARVIS_Auth_AutoCapture: This API is to start Iris images capturing and will returned only when desired level of quality achieved.
* @param timeout[In]: Timeout value at which device will timed out for capturing and capturing will be stopped.
* @param Timeout[In]: Timeout value at which device will timed out for capturing and capturing will be stopped.
* @param Quality[Out]: This will hold Image frame quality of the captured iris.
* @param p_CaptureCompleteCallBack[Out]: A call back function which will provide details like error code, quality, iris image raw data for preview and data length, continuously.
* @note Memory allocated to rawdata will be de-allocated at library side. User is not required to free memory.
* @return : 0 in case of success, -ve for failed or error.
*/
MARVIS int MARVIS_Auth_AutoCapture(int timeout, int *Quality, PREVIEW_CALLBACK p_MARVIS_AUTH_PREVIEW_CALLBACK, MARVIS_IMAGE_DATA *imageData);


/**
 * @brief MARVIS_Auth_StopCapture: This API will stop iris image capturing.
 * @return : 0 in case of success, other for failed or error.
 */
MARVIS int MARVIS_Auth_StopCapture();

/**
 * @brief MARVIS_Auth_GetImage: This API will provide captured image data and its length in requested image format.
 * @param out_pImage[Out]: This parameter will hold pointer to captured image raw data. User must allocate memory for this parameter..
 * @param ImageLen[In/Out]: This parameter will hold captured image data buffer length on input time and in return it will hold image data length.
 * @param Format[In]: Type of image data required, user should specify format as per _IMAGE_FORMAT enum value.
 * @param CompressionRatio[In]: Required compression ratio
 * @return : 0 in case of success, other for failed or error.
 */
MARVIS int MARVIS_Auth_GetImage(unsigned char *out_pimage, int *Imagelen, IMAGE_FORMAT Format, int CompressionRatio);

MARVIS int MARVIS_Auth_MatchIrisData(unsigned char* galleryImg, unsigned char* probImg, int* out_score);

/**
 * @brief MARVIS_Auth_GetErrDescription: This API will return error description for the provided error code.
 * @param errorNo[In]: Error code for which error description is required.
 * @return : 0 in case of success, other for failed or error.
 */
MARVIS const char *MARVIS_Auth_GetErrDescription(int errorNo);

/**
 * @brief MARVIS_Auth_EnableLogs: This API will set log related properties.
 * @param eLogLevel: Required log level. For levels find enum value for MARVIS_AUTH_MBS_LOG_LEVEL.
 * @param pFilePath: File path at where log file will be created. If not provided (NULL) then file with name "M0070Core.log" will be created in current directory.
 */
MARVIS int MARVIS_Auth_EnableLogs(const MARVIS_AUTH_LOG_LEVEL eLogLevel, const char *pFolderPath);

/**
 * @brief MARVIS_Auth_GetVersion: This API will return Library version in passed argument.
 * @param ver[In]: This argument will hold library version detail on success.
 * @return : 0 in case of success, other for failed or error.
 */
MARVIS int MARVIS_Auth_GetVersion(char *ver);

/**
 * @brief MARVIS_Auth_GetDeviceList: This API will return Connected Device List in SDK.
 * @param StDeviceList[Out]:A structure pointer to DEVICE_LIST which will hold device list information on success 
	When structure pointer null then user will get number of connected device count which is used in allocate array structure size.
	User is required to free memory.
 * @param DeviceCnt[Out]: connected device count on success.
 * @return : 0 in case of success, other for failed or error.
 */
MARVIS int MARVIS_Auth_GetDeviceList(DEVICE_LIST *StDeviceList,int *DeviceCnt);

/**
 * @brief MARVIS_Auth_GetSupportedDeviceList: This API will return Supported Device List in SDK.
 * @param StDeviceList[Out]:A structure pointer to DEVICE_LIST which will hold device list information on success 
	When structure pointer null then user will get number of connected device count which is used in allocate array structure size.
	User is required to free memory.
 * @param DeviceCnt[Out]: connected device count on success.
 * @return : 0 in case of success, other for failed or error.
 */
MARVIS int MARVIS_Auth_GetSupportedDeviceList(DEVICE_LIST *StDeviceList,int *DeviceCnt);

#ifdef __cplusplus 
}
#endif 

#endif

