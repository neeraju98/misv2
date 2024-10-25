#include "..\Header Files\CommonCore.h"
#include <chrono>
#include <ctime>
#define BITMAP_FILE_HEADER_LENGTH   14      //Length in Bytes
#define BITMAP_IMAGE_HEADER_LENGTH  40      //Length in Bytes
#define BITMAP_COLOR_PALETTE_LENGTH 1024    //Length in Bytes
#define BITMAP_TOTAL_LENGTH         (BITMAP_FILE_HEADER_LENGTH + BITMAP_IMAGE_HEADER_LENGTH + BITMAP_COLOR_PALETTE_LENGTH)
#define MIS100V2_FW "MOPv6.0"
#define NO_OF_IRIS					1
#define REQUIRED_QUALITY                        90
//#define MINIMUM_CAPTURE_ROTATION                6
#define MIN_PUPIL_MEAN                          0 //2
#define MAX_PUPIL_MEAN                          115
#define IRIS_HEADER_LENGTH			68
#define m_stricmp(s1, s2) stricmp(s1, s2)
#define M_PI 3.14
//#define PWM_FW		//for set pwm iris logic
int prev_blur_value = 0;
int white_count_FULL = 0;
int black_count_FULL = 0;
time_t tmStartTime, tmEndTime;
double elapsed_seconds = 0;
#pragma region Variable Declaration

int maximum = 0; double textureFeature = 0.0;
int minimum = 0;
int First = 0;
bool IRLed_Status = true;
bool IRLed_On = false;
bool IRLed_First = false;
bool ISComplete = false;
bool IsPreviewHide = false;


int pre_mean = 0;
double prev_std = 0.0;
int hideframecounter = 0;
bool IsDeviceInit = false;
bool IsAutoCap = false;

bool IsTimeOutAutoCap = false;
bool IsAutoCapComplete = false;

volatile bool IsGetQtyInCap = false;
static bool IsGetQtyInCapTemp = false;

unsigned long CapStartTime = 0;
//unsigned long CompleteCapTime = 0;
//bool IsFirstFrame = false;
int ExceptionLog = 0;
char m_szVendorID[VENDORID_LENGTH];
char m_szProductID[PRODUCTID_LENGTH];
int nTimeOutInMilliSeconds = 10;
int nMinQty = 40;
int nReqQty = 95;
int nMinCapRotation = 6;
int gPreviewErrorCode = 0;

bool IsStopCaptureRunning = false;
bool IsStopButton = false;
int CapturedQty = 0;
bool IsCheckDeviceThread = false;

COMM_EVT_CALLBACK m_cbCommEventCallback;

HANDLE  m_hAutoStopThreadHandle;
HANDLE  m_hCaptureCompleteThreadHandle;
HANDLE  m_hCapturePreviewThreadHandle;
HANDLE  m_hImageProcessThreadHandle;
HANDLE  m_hAutoDeviceDetectThreadHandle;
PREVIEW_CALLBACK m_PREVIEW_CALLBACK = NULL;
CAPTURE_COMPLETE_CALLBACK m_CAPTURE_COMPLETE_CALLBACK = NULL;

DEVICE_DETECTION m_DEVICE_DETECTION = NULL;

// FPS
int m_NoOfFrameCount;
float m_fFPS;
LARGE_INTEGER FpsStartTime;
LARGE_INTEGER FpsEndtime;
LARGE_INTEGER FpsframeFrequency;

string g_executionTimeInfo;
int m_FrameCount;
//
int arr1[4] = { 2, 3, 4, 5 };
int arr2[3] = { 17, 13, 10 };
int pwmchangecount = 0;

COMM_DUTY_CYCLE_LEVEL lastIRLED;

/**************************  IPL Lib**************************/

int DeviceErrorCode = 0;
bool isDataBufferFilled = false;
FPC_IRIS_EYE_INFO pEyeInfo;
#define MAX_SKIP_FRAME 5
int gCounter = 0;

int MinQtyCounter = 0;
bool IsIrisLedOn = false;
int MinQtyLedOn = 5;
int pwmcounter = 0;
int prev_pwmValue = 0;
int highPwm = 0;
int midPwm = 0;
int lowPwm = 0;
int skipFrame = 0;
int counter = 0;
int tempcount = 0;
double starttime = 0;
double endtime = 0;
bool updown = false;
int high_meanvalue = 0;
int mid_meanvalue = 0;
int low_meanvalue = 0;
float max_high_texture ;
float min_high_texture;
float max_low_texture;
float min_low_texture;

struct imgProperty {
	int irValue;
	int mean;
	int sd;
	int blur;
	double texture;
};
vector <imgProperty> irPropety;
int lowpwmhighmean = 0;
char emp_id[2048];

static bool gPreviousDeviceStatus[4] = { false };
/************************** END IPL Lib**************************/

#pragma endregion

#pragma region Function Declaration

void ImagePreviewCallback(void *pParam);
DWORD WINAPI ImageProcessThreadFull(LPVOID lpParam);

void ImageProcessThread(LPVOID pArgs_);
void freeIPLLibVariables();
vector<string> split(string s, string delimiter);

#pragma endregion

#pragma region Structure Declaration
typedef enum _MID_FINGER_AUTH_DEVICE_STATUS {
	/** Device is connected successfully */
	DEVICE_CONNECTED = 1,

	/** Device is initialized successfully */
	DEVICE_INITIALIZED = 2,

	/** Image capturing started */
	DEVICE_STREAMING_STARTED = 3,

	/** Image capturing stopped */
	DEVICE_STREAMING_STOPPED = 4,

	/** Device uninit */
	DEVICE_UNINITIALIZED = 5,

	/** Device is not connected, Reinsert and then restart application */
	DEVICE_DISCONNECTED = 6,
} MID_FINGER_AUTH_DEVICE_STATUS;

typedef enum _COMPRESSION_TYPE {
	COMPRESSION_NONE = 0,
	COMPRESSION_WSQ = 1,
	COMPRESSION_JPEG2000 = 2
}   COMPRESSION_TYPE;

typedef enum _MFG_LIB_ERR {
	/** Invalid Product Id in MFG data process */
	MFG_INVALID_PRODUCT_ID = -3001,
	/** Invalid Vendor Id in MFG data process */
	MFG_INVALID_VENDOR_ID = -3002,
	/** Invalid Device Handle in MFG data process */
	MFG_INVALID_DEV_HNDL = -3003,
	/** CRC failed in MFG data process */
	MFG_CRC_FAILED = -3004,
	/** CRC Type in MFG data process */
	MFG_CRC_TYPE_FAILED = -3005,
	/** Invalid patameter in MFG data process */
	MFG_INVLD_PARAM = -3006,
	/** key set failed in MFG data process */
	MFG_KEY_SET_FAILED = -3007,
	/** Encryption failed in MFG data process */
	MFG_ENC_FAILED = -3008,
	/** decryption failed in MFG data process */
	MFG_DEC_FAILED = -3009,
	/** Device not connected in MFG data process */
	MFG_DEV_NOT_CONNECTED = -3010,
	/** Corrupted serial number in MFG data process */
	MFG_CORRUPTED_SR_NO = -3011,
	/** Default serial number in MFG data process */
	MFG_DEFAULT_SR_NO = -3012
} MFG_LIB_ERR;

typedef struct _COMPLETE_FRAME_INFO
{
	unsigned char  *Final_Frame = NULL;

	MARVIS_IMAGE_DATA pBitmap;
	MARVIS_IMAGE_QUALITY pQtyInfo;
}COMPLETE_FRAME_INFO;

struct _IPL_INFO
{
	int DPI = 500;
	int FULL_FRAME_WIDTH = 1280;
	int FULL_FRAME_HEIGHT = 720;
	int CROP_WIDTH = 3412;
	int CROP_HEIGHT = 2340;
	int CROP_X_COORDINATE = 204;
	int CROP_Y_COORDINATE = 210;
	int FINAL_FRAME_WIDTH = 640;
	int FINAL_FRAME_HEIGHT = 480;
	bool Invert = false;
	int RotateFinalImage = 0;

	unsigned char serialno[10];
	unsigned char Firmware_Version[12];

	unsigned char *RawImageBuffer = NULL;
	int RawImageLen = 0;

	unsigned char  *Final_Frame = NULL;
	unsigned char *Cropped_Frame = NULL;
	unsigned char *Pers_Frame = NULL;
	unsigned char  *back_frame = NULL;
	unsigned char  *bmpFrame = NULL;
	int nImageLengthWithBitmapHeader = 0;
	double *secondPersArray = NULL;
	double *firstPersArray = NULL;




	MARVIS_IMAGE_DATA pBitmapImage;
	MARVIS_IMAGE_QUALITY pQtyInfo;

	COMPLETE_FRAME_INFO pCompleteFrame;


	int Focus = 0;
	int Area = 0;
	int Blur = 0;


}g_ipl_info[1];

//unsigned char  *Blur_Frame = NULL;
//unsigned char  *LC_Crp_Frame = NULL;
unsigned char  *Prev_Frame_Preview = NULL;

float m_gSimilarityScore[3];

int gSimilarityThreshold = 85;// 70;

typedef struct _features_entry {
	char *id;
	uint8_t *des_features;
	struct _features_entry *next;
} features_entry;

static MID_FINGER_AUTH_DEVICE_STATUS deviceStatus = DEVICE_DISCONNECTED;

#pragma endregion

#pragma region User Function
void trim(char * s) {
	char * p = s;
	int l = strlen(p);

	while (isspace(p[l - 1])) p[--l] = 0;
	while (*p && isspace(*p)) ++p, --l;

	memmove(s, p, l + 1);
}
bool ValidateDeviceFirmware(const char *pVendorID, const char *pProductID, const char* Firmware)
{
	char szOutputString[256] = { 0 };
	try
	{
		if ((strcmp(pVendorID, MIS100V2_VID_STR) == 0 && strcmp(pProductID, MIS100V2_PID_STR) == 0) && strcmp(Firmware, MIS100V2_FW) == 0)
		{
			return true;
		}
	}

	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
	}
	return false;

}
int validateMfgData(int t_iris_index)
{
	char szOutputString[256] = { 0 };
	int retValue = MARVIS_AUTH_E_NO_DEVICE;
	try
	{

		{
			if (strlen((const char *)g_ipl_info[t_iris_index].serialno) >= 6)
			{
				retValue = MARVIS_AUTH_E_SUCCESS;
			}
			else
			{
				return MARVIS_AUTH_E_CORRUPT_SERIAL;
			}

			if (g_ipl_info[t_iris_index].FULL_FRAME_WIDTH < 100 || g_ipl_info[t_iris_index].FULL_FRAME_HEIGHT < 100)
			{
				return MARVIS_AUTH_E_FULL_FRAME_CORRUPTED;
			}

			if (g_ipl_info[t_iris_index].FINAL_FRAME_WIDTH < 100 || g_ipl_info[t_iris_index].FINAL_FRAME_HEIGHT < 100)
			{
				return MARVIS_AUTH_E_FINAL_FRAME_CORRUPTED;
			}

			if (g_ipl_info[t_iris_index].CROP_WIDTH < 100 || g_ipl_info[t_iris_index].CROP_HEIGHT < 100)
			{
				return MARVIS_AUTH_E_CROP_CORRUPTED;
			}

			if (g_ipl_info[t_iris_index].CROP_X_COORDINATE > 600 || g_ipl_info[t_iris_index].CROP_Y_COORDINATE > 600)
			{
				return MARVIS_AUTH_E_COORDINATE_CORRUPTED;
			}

			if (strlen((const char *)g_ipl_info[t_iris_index].Firmware_Version) >= 8)
			{
				retValue = MARVIS_AUTH_E_SUCCESS;
			}
			else
			{
				return MARVIS_AUTH_E_FIRMWARE_CORRUPTED;
			}

			if (g_ipl_info[t_iris_index].DPI < 300)
			{
				g_ipl_info[t_iris_index].DPI = 565;
			}

			retValue = MARVIS_AUTH_E_SUCCESS;
		}
	}

	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}

	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}

	return retValue;
}
void WriteFile(const char *format, ...)
{
	int Write = 1;
	if (Write == 0)
	{
		FILE *fp = NULL;
		errno_t err = 0;
		err = fopen_s(&fp, "test.txt", "a+");
		if (err == 0)
		{
			va_list argptr;
			va_start(argptr, format);
			vfprintf(fp, format, argptr);
			va_end(argptr);
			fclose(fp);
		}
	}
}
void ClearData()
{
	isDataBufferFilled = false;
	IsDeviceInit = false;
	IsAutoCap = false;
	IsGetQtyInCap = false;
	IsGetQtyInCapTemp = false;
	DeviceErrorCode = MARVIS_AUTH_E_CAPTURE_STOP;
	IsAutoCapComplete = true;

	IsTimeOutAutoCap = false;
	ExceptionLog = 0;
	nTimeOutInMilliSeconds = 10000;
	nMinQty = 40;
	CapStartTime = 0;
	IsStopButton = false;
	deviceStatus = DEVICE_UNINITIALIZED;
	gCounter = 0;

	MinQtyCounter = 0;
	IsIrisLedOn = false;

}
void ResetStartCaptureData()
{
	IsAutoCap = false;
	IsStopButton = false;
	IsGetQtyInCap = false;
	IsAutoCapComplete = false;
	IsGetQtyInCapTemp = false;
	DeviceErrorCode = 0;
	IsTimeOutAutoCap = false;

	memset(g_ipl_info[0].pBitmapImage.m_BitmapImage, 255, g_ipl_info[0].nImageLengthWithBitmapHeader);

	g_ipl_info[0].pQtyInfo.Quality = 0;
	g_ipl_info[0].pQtyInfo.X = 0;
	g_ipl_info[0].pQtyInfo.Y = 0;
	g_ipl_info[0].pQtyInfo.R = 0;

	memset(g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImage, 255, g_ipl_info[0].nImageLengthWithBitmapHeader);

	g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality = 0;
	g_ipl_info[0].pCompleteFrame.pQtyInfo.X = 0;
	g_ipl_info[0].pCompleteFrame.pQtyInfo.Y = 0;
	g_ipl_info[0].pCompleteFrame.pQtyInfo.R = 0;

	MinQtyCounter = 0;
	IsIrisLedOn = false;
	gCounter = 0;
	m_FrameCount = 0;


	maximum = 0;
	minimum = 0;
	double mean;
	int blur_value = 0;
}
void WriteAllText1(const char *name, char *data, int datalength)
{
	int WriteFile = 1;
	if (WriteFile == 0)
	{
		FILE *file = NULL;
		errno_t err = 0;
		err = fopen_s(&file, name, "wb");
		if (err == 0)
		{
			fwrite(data, 1, datalength, file);
			fclose(file);
		}
	}
}
void freeIPLLibVariables()
{
	for (int l_i = 0; l_i < NO_OF_IRIS; l_i++)
	{
		if (g_ipl_info[l_i].RawImageBuffer)
		{
			free(g_ipl_info[l_i].RawImageBuffer);
			g_ipl_info[l_i].RawImageBuffer = NULL;
		}

		if (g_ipl_info[l_i].Final_Frame)
		{
			free(g_ipl_info[l_i].Final_Frame);
			g_ipl_info[l_i].Final_Frame = NULL;
		}

		if (g_ipl_info[l_i].pCompleteFrame.Final_Frame)
		{
			free(g_ipl_info[l_i].pCompleteFrame.Final_Frame);
			g_ipl_info[l_i].pCompleteFrame.Final_Frame = NULL;
		}

		if (g_ipl_info[l_i].firstPersArray)
		{
			free(g_ipl_info[l_i].firstPersArray);
			g_ipl_info[l_i].firstPersArray = NULL;
		}

		if (g_ipl_info[l_i].secondPersArray)
		{
			free(g_ipl_info[l_i].secondPersArray);
			g_ipl_info[l_i].secondPersArray = NULL;
		}

		if (g_ipl_info[l_i].back_frame)
		{
			free(g_ipl_info[l_i].back_frame);
			g_ipl_info[l_i].back_frame = NULL;
		}

		if (g_ipl_info[l_i].Cropped_Frame)
		{
			free(g_ipl_info[l_i].Cropped_Frame);
			g_ipl_info[l_i].Cropped_Frame = NULL;
		}

		if (g_ipl_info[l_i].Pers_Frame)
		{
			free(g_ipl_info[l_i].Pers_Frame);
			g_ipl_info[l_i].Pers_Frame = NULL;
		}

		if (g_ipl_info[l_i].pBitmapImage.m_BitmapImage)
		{
			free(g_ipl_info[l_i].pBitmapImage.m_BitmapImage);
			g_ipl_info[l_i].pBitmapImage.m_BitmapImage = NULL;
		}

		if (g_ipl_info[l_i].pCompleteFrame.pBitmap.m_BitmapImage)
		{
			free(g_ipl_info[l_i].pCompleteFrame.pBitmap.m_BitmapImage);
			g_ipl_info[l_i].pCompleteFrame.pBitmap.m_BitmapImage = NULL;
		}
	}
}

int GetIplParseData()
{
	int retValue = MARVIS_AUTH_E_SUCCESS;

	char szOutputString[256] = { 0 };

	for (int l_i = 0; l_i < NO_OF_IRIS; l_i++)
	{
		g_ipl_info[l_i].nImageLengthWithBitmapHeader = BITMAP_FILE_HEADER_LENGTH + BITMAP_IMAGE_HEADER_LENGTH + BITMAP_COLOR_PALETTE_LENGTH + (g_ipl_info[l_i].FINAL_FRAME_WIDTH * g_ipl_info[l_i].FINAL_FRAME_HEIGHT);

		g_ipl_info[l_i].RawImageBuffer = (unsigned char *)malloc(g_ipl_info[l_i].FULL_FRAME_WIDTH * g_ipl_info[l_i].FULL_FRAME_HEIGHT);
		if (g_ipl_info[l_i].RawImageBuffer == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(RawImageBuffer).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			std::memset(g_ipl_info[l_i].RawImageBuffer, 0, g_ipl_info[l_i].FULL_FRAME_WIDTH * g_ipl_info[l_i].FULL_FRAME_HEIGHT);
		}

		g_ipl_info[l_i].pBitmapImage.m_BitmapImage = (unsigned char *)malloc(g_ipl_info[l_i].nImageLengthWithBitmapHeader);
		if (g_ipl_info[l_i].pBitmapImage.m_BitmapImage == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(pBitmapImage).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			std::memset(g_ipl_info[l_i].pBitmapImage.m_BitmapImage, 0, g_ipl_info[l_i].nImageLengthWithBitmapHeader);
		}

		g_ipl_info[l_i].pCompleteFrame.pBitmap.m_BitmapImage = (unsigned char *)malloc(g_ipl_info[l_i].nImageLengthWithBitmapHeader);
		if (g_ipl_info[l_i].pCompleteFrame.pBitmap.m_BitmapImage == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(pBitmapImage).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			std::memset(g_ipl_info[l_i].pCompleteFrame.pBitmap.m_BitmapImage, 0, g_ipl_info[l_i].nImageLengthWithBitmapHeader);
		}

		g_ipl_info[l_i].firstPersArray = (double*)malloc(g_ipl_info[l_i].FULL_FRAME_WIDTH * g_ipl_info[l_i].FULL_FRAME_HEIGHT* sizeof(double));
		if (g_ipl_info[l_i].firstPersArray == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(firstPersArray).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			memset(g_ipl_info[l_i].firstPersArray, 0, g_ipl_info[l_i].FULL_FRAME_WIDTH * g_ipl_info[l_i].FULL_FRAME_HEIGHT* sizeof(double));
		}

		g_ipl_info[l_i].secondPersArray = (double*)malloc(g_ipl_info[l_i].FULL_FRAME_WIDTH *g_ipl_info[l_i].FULL_FRAME_HEIGHT* sizeof(double));
		if (g_ipl_info[l_i].secondPersArray == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(secondPersArray).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			memset(g_ipl_info[l_i].secondPersArray, 0, g_ipl_info[l_i].FULL_FRAME_WIDTH *g_ipl_info[l_i].FULL_FRAME_HEIGHT* sizeof(double));
		}

		g_ipl_info[l_i].back_frame = (unsigned char *)malloc(g_ipl_info[l_i].FULL_FRAME_WIDTH *g_ipl_info[l_i].FULL_FRAME_HEIGHT);
		if (g_ipl_info[l_i].back_frame == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(backFrame).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			memset(g_ipl_info[l_i].back_frame, 0, g_ipl_info[l_i].FULL_FRAME_WIDTH *g_ipl_info[l_i].FULL_FRAME_HEIGHT);
		}

		g_ipl_info[l_i].Cropped_Frame = (unsigned char *)malloc((g_ipl_info[l_i].CROP_WIDTH + 20) *(g_ipl_info[l_i].CROP_HEIGHT + 20));
		if (g_ipl_info[l_i].Cropped_Frame == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(croppedFrame).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			memset(g_ipl_info[l_i].Cropped_Frame, 0, (g_ipl_info[l_i].CROP_WIDTH + 20) *(g_ipl_info[l_i].CROP_HEIGHT + 20));
		}

		g_ipl_info[l_i].Pers_Frame = (unsigned char *)calloc(g_ipl_info[l_i].FULL_FRAME_WIDTH * g_ipl_info[l_i].FULL_FRAME_HEIGHT, 1);
		if (g_ipl_info[l_i].Pers_Frame == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(persFrame).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			memset(g_ipl_info[l_i].Pers_Frame, 0, g_ipl_info[l_i].FULL_FRAME_WIDTH * g_ipl_info[l_i].FULL_FRAME_HEIGHT);
		}

		g_ipl_info[l_i].Final_Frame = (unsigned char *)malloc(g_ipl_info[l_i].FINAL_FRAME_WIDTH * g_ipl_info[l_i].FINAL_FRAME_HEIGHT);

		if (g_ipl_info[l_i].Final_Frame == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(finalFrame).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			std::memset(g_ipl_info[l_i].Final_Frame, 0, g_ipl_info[l_i].FINAL_FRAME_WIDTH * g_ipl_info[l_i].FINAL_FRAME_HEIGHT);
		}

		g_ipl_info[l_i].pCompleteFrame.Final_Frame = (unsigned char *)malloc(g_ipl_info[l_i].FINAL_FRAME_WIDTH * g_ipl_info[l_i].FINAL_FRAME_HEIGHT);

		if (g_ipl_info[l_i].pCompleteFrame.Final_Frame == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(finalFrame).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{
			std::memset(g_ipl_info[l_i].pCompleteFrame.Final_Frame, 0, g_ipl_info[l_i].FINAL_FRAME_WIDTH * g_ipl_info[l_i].FINAL_FRAME_HEIGHT);
		}

		Prev_Frame_Preview = (unsigned char *)malloc(g_ipl_info[0].FINAL_FRAME_WIDTH * g_ipl_info[0].FINAL_FRAME_HEIGHT);
		if (Prev_Frame_Preview == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to allocate memory(Prev_Frame_Preview).",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_MEMORY);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_MEMORY;
		}
		else
		{

			memset(Prev_Frame_Preview, 0, g_ipl_info[0].FINAL_FRAME_WIDTH * g_ipl_info[0].FINAL_FRAME_HEIGHT);
		}

	}

	return retValue;
}

// for string delimiter
vector<string> split(string s, string delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

int GetDeviceVidPid(const char *pcProductName)
{
	int retVal = MARVIS_AUTH_E_SUCCESS;
	string strProductName(pcProductName);
	std::transform(strProductName.begin(), strProductName.end(), strProductName.begin(), ::toupper);
	if (strProductName.compare(MIS100V2) == 0)
	{
		strcpy_s(m_szVendorID, VENDOR_ID_LENGTH, MIS100V2_VID_STR);	///< MIS100V2 Vendor ID. 
		strcpy_s(m_szProductID, PRODUCT_ID_LENGTH, MIS100V2_PID_STR);	///< MIS100V2 Product ID.
	}
	else
	{
		retVal = MARVIS_AUTH_E_INVALIDPARAM;
	}

	return retVal;
}

int GetConnectedDeviceList()
{
	char szOutputString[256] = { 0 };
	int retValue = MARVIS_AUTH_E_NO_DEVICE;

	string DeviceName[1] = { MIS100V2 };

	try
	{
		int j = 0;
		for (int i = 0; i <= 3; i++)
		{
			retValue = GetDeviceVidPid((char*)DeviceName[i].c_str());
			if (MARVIS_AUTH_E_SUCCESS == retValue)
			{
				::Sleep(200);
				retValue = CommIsDeviceConnected(-1, m_szVendorID, m_szProductID);
				if (MARVIS_AUTH_E_SUCCESS == retValue)
				{
					if (m_DEVICE_DETECTION)
					{
						m_DEVICE_DETECTION(DeviceName[i].c_str(), EVENT_CONNECTED);
					}
				}
			}
		}
	}
	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	return MARVIS_AUTH_E_SUCCESS;
}

void Auto_UninitDevice()
{
	if (!IsAutoCap)
	{
		if (m_CAPTURE_COMPLETE_CALLBACK)
		{
			(m_CAPTURE_COMPLETE_CALLBACK)(MARVIS_AUTH_E_NO_DEVICE, 0, 0);
		}
	}
	MARVIS_Auth_UninitDevice();
}

int SetDeviceInfo(MARVIS_DEVICE_INFO *StDeviceInfo, int t_iris_index)
{
	char szOutputString[256] = { 0 };
	int retValue = MARVIS_AUTH_E_SUCCESS;

	string FirmWare = split((char*)g_ipl_info[t_iris_index].Firmware_Version, ".")[0] + "." + split((char*)g_ipl_info[t_iris_index].Firmware_Version, ".")[1];

	memcpy(StDeviceInfo->Firmware, FirmWare.c_str(), FirmWare.length());
	StDeviceInfo->Firmware[FirmWare.length()] = 0;

	/*memcpy(StDeviceInfo->Firmware, g_ipl_info[t_iris_index].Firmware_Version, strlen((char*)g_ipl_info[t_iris_index].Firmware_Version));
	StDeviceInfo->Firmware[strlen((char*)g_ipl_info[t_iris_index].Firmware_Version)] = 0;*/

	memcpy(StDeviceInfo->Make, "Mantra", strlen("Mantra"));
	StDeviceInfo->Make[strlen("Mantra")] = 0;

	//string strModelName(ssDevMfgData.m_Model);
	string strModelName(MIS100V2);
	strModelName.erase(remove(strModelName.begin(), strModelName.end(), ' '), strModelName.end());

	memcpy(StDeviceInfo->Model, strModelName.c_str(), strModelName.length());
	StDeviceInfo->Model[strModelName.length()] = 0;

	memcpy(StDeviceInfo->SerialNo, g_ipl_info[t_iris_index].serialno, strlen((char*)g_ipl_info[t_iris_index].serialno));
	StDeviceInfo->SerialNo[strlen((char*)g_ipl_info[t_iris_index].serialno)] = 0;

	StDeviceInfo->Height = g_ipl_info[t_iris_index].FINAL_FRAME_HEIGHT;
	StDeviceInfo->Width = g_ipl_info[t_iris_index].FINAL_FRAME_WIDTH;

	return retValue;
}

unsigned char *DeviceHexStringToGivenNoOfParts(const std::string &hexString, int nParts)
{
	size_t numberOfChars = hexString.length();

	size_t nProcessingStringLength = (numberOfChars + 1) / 2;
	unsigned char *pProcessingString = (unsigned char *)calloc(nProcessingStringLength, sizeof(unsigned char));
	unsigned char *pDevidexBytes = (unsigned char *)calloc(nParts, sizeof(unsigned char));

	int nOffset = 0;
	if ((numberOfChars % 2) == 1)
	{
		std::string strTemp = hexString.substr(0, 1);
		strTemp += "";
		pProcessingString[0] = (unsigned char)strtol(strTemp.c_str(), NULL, 16);
		nOffset = 1;
	}

	for (size_t i = 0; i < (numberOfChars / 2); i++)
		pProcessingString[i + nOffset] = (unsigned char)strtol(hexString.substr((i * 2) + nOffset, 2).c_str(), NULL, 16);

	memcpy(pDevidexBytes + nParts - nProcessingStringLength, pProcessingString, nProcessingStringLength);
	return pDevidexBytes;
}
int CreateISOImageIris_11(unsigned char *pWsqImage, unsigned char *pIso_19794_4_Image, int nWsqImageSize, int nImageHeight, int nImageWidth)
{
	int  returnValue = 0;
	if (!pWsqImage || !pIso_19794_4_Image)
		return -1;

	int Length = 0;

	//Format Identifier - 4 Bytes
	pIso_19794_4_Image[0] = 0x49;
	pIso_19794_4_Image[1] = 0x49;
	pIso_19794_4_Image[2] = 0x52;
	pIso_19794_4_Image[3] = 0x00;

	//Version No. - 4 Bytes
	pIso_19794_4_Image[4] = 0x30;
	pIso_19794_4_Image[5] = 0x32;
	pIso_19794_4_Image[6] = 0x30;
	pIso_19794_4_Image[7] = 0x00;

	int nImageSize = nWsqImageSize + IRIS_HEADER_LENGTH;
	std::stringstream ssRecordLengthStream;
	ssRecordLengthStream << std::hex << nImageSize;
	unsigned char *pRecordLengthInBytes = DeviceHexStringToGivenNoOfParts(ssRecordLengthStream.str(), 4);
	//Record Length - 6 Bytes
	memcpy(pIso_19794_4_Image + 8, pRecordLengthInBytes, 4);
	//free(pRecordLengthInBytes);

	//Device Id - 2 Bytes
	pIso_19794_4_Image[12] = 0x00;
	pIso_19794_4_Image[13] = 0x01;

	//CertFlag - 1 Bytes
	pIso_19794_4_Image[14] = 0x00;

	//NoOFDistPalm - 1 Bytes
	pIso_19794_4_Image[15] = 0x01;

	//=================//

	int LenImage = nWsqImageSize + IRIS_HEADER_LENGTH - 16;
	std::stringstream ssLengthStream;
	ssLengthStream << std::hex << LenImage;
	pRecordLengthInBytes = DeviceHexStringToGivenNoOfParts(ssLengthStream.str(), 4);
	memcpy(pIso_19794_4_Image + 16, pRecordLengthInBytes, 4);

	time_t t = time(0);
	struct tm* lt = localtime(&t);
	char time_str[15];
	sprintf(time_str, "%04d%02d%02d%02d%02d%02d",
		lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday,
		lt->tm_hour, lt->tm_min, lt->tm_sec);

	int dtyear = atoi(string(time_str).substr(0, 4).c_str());
	std::stringstream ssdtyearStream;
	ssdtyearStream << std::hex << dtyear;
	pRecordLengthInBytes = DeviceHexStringToGivenNoOfParts(ssdtyearStream.str(), 2);
	memcpy(pIso_19794_4_Image + 20, pRecordLengthInBytes, 2);

	int month = atoi(string(time_str).substr(4, 2).c_str());
	int day = atoi(string(time_str).substr(6, 2).c_str());
	int Hour = atoi(string(time_str).substr(8, 2).c_str());
	int min = atoi(string(time_str).substr(10, 2).c_str());
	int sec = atoi(string(time_str).substr(12, 2).c_str());

	pIso_19794_4_Image[22] = month; //datetime[0];
	pIso_19794_4_Image[23] = day;
	pIso_19794_4_Image[24] = Hour;
	pIso_19794_4_Image[25] = min;
	pIso_19794_4_Image[26] = sec;
	pIso_19794_4_Image[27] = 0;
	pIso_19794_4_Image[28] = 0;

	//=========//\=====

	//Device Tec Iden - 1 Bytes
	//06
	pIso_19794_4_Image[29] = 0x01;

	//Device Vendor Iden - 2 Bytes
	//0000
	pIso_19794_4_Image[30] = 0x00; pIso_19794_4_Image[31] = 0x00;

	//Device Type Iden - 2 Bytes
	//0000
	pIso_19794_4_Image[32] = 0x00; pIso_19794_4_Image[33] = 0x00;

	//Qlt Blk Cnt - 1 Bytes
	//00
	pIso_19794_4_Image[34] = 0x00;

	//FingerPalmPosition - 1 Bytes
	//00
	pIso_19794_4_Image[35] = 0x00;

	//represent Util - 1 Bytes
	//00
	pIso_19794_4_Image[36] = 0x01;

	//Scal util Num - 1 Bytes
	//01
	pIso_19794_4_Image[37] = 0x00;

	//DPI Hor - 2 Bytes
	//01F4
	pIso_19794_4_Image[38] = 0x01; pIso_19794_4_Image[39] = 0x0A;

	//DPI Ver - 2 Bytes
	//01F4
	pIso_19794_4_Image[40] = 0x00;

	std::stringstream ssnImageWidthStream;
	ssnImageWidthStream << std::hex << nImageWidth;
	pRecordLengthInBytes = DeviceHexStringToGivenNoOfParts(ssnImageWidthStream.str(), 2);
	memcpy(pIso_19794_4_Image + 41, pRecordLengthInBytes, 2);

	std::stringstream ssnImageHeightStream;
	ssnImageHeightStream << std::hex << nImageHeight;
	pRecordLengthInBytes = DeviceHexStringToGivenNoOfParts(ssnImageHeightStream.str(), 2);
	memcpy(pIso_19794_4_Image + 43, pRecordLengthInBytes, 2);

	//Bit Depth	1 Byte
	pIso_19794_4_Image[45] = 8;
	//Range	2 Bytes
	pIso_19794_4_Image[46] = 0;
	pIso_19794_4_Image[47] = 0;
	//Roll Angle	2 Bytes
	pIso_19794_4_Image[48] = 0;
	pIso_19794_4_Image[49] = 0;
	//Roll Angle Uncertainity	2 Bytes
	pIso_19794_4_Image[50] = 0;
	pIso_19794_4_Image[51] = 0;
	//IRIS Center Smallest X	2 Bytes
	pIso_19794_4_Image[52] = 0;
	pIso_19794_4_Image[53] = 0;
	//IRIS Center Largest X	2 Bytes
	pIso_19794_4_Image[54] = 0;
	pIso_19794_4_Image[55] = 0;
	//IRIS Center Smallest Y	2 Bytes
	pIso_19794_4_Image[56] = 0;
	pIso_19794_4_Image[57] = 0;
	//IRIS Center Largest Y	2 Bytes
	pIso_19794_4_Image[58] = 0;
	pIso_19794_4_Image[59] = 0;
	//IRIS Diameter Smallest	2 Bytes
	pIso_19794_4_Image[60] = 0;
	pIso_19794_4_Image[61] = 0;
	//IRIS Diameter largest	2 Bytes
	pIso_19794_4_Image[62] = 0;
	pIso_19794_4_Image[63] = 0;

	std::stringstream ssnImageLenStream;
	ssnImageLenStream << std::hex << nWsqImageSize;
	pRecordLengthInBytes = DeviceHexStringToGivenNoOfParts(ssnImageLenStream.str(), 4);
	memcpy(pIso_19794_4_Image + 64, pRecordLengthInBytes, 4);

	memcpy(pIso_19794_4_Image + 68, pWsqImage, nWsqImageSize);
	returnValue = MARVIS_AUTH_E_SUCCESS;
	return returnValue;
}

int ire_kind7(uint8_t *pixels, int width, int height, unsigned char *Jp2Isoimage, int &t_width, int &t_height, int &t_final_size, bool t_K7) {

	int retValue = 0;
	try
	{
		size_t max_features_size, working_set_size;

		ire3_settings settings = iris_settings;

		//ire3_settings settings = { sizeof(ire3_settings), 200, 6 };

		auto rc = ire3_get_max_features_size(&max_features_size);
		rc = ire3_get_extraction_working_set_size(&working_set_size, &settings);
		std::vector<uint8_t> featurev(max_features_size);
		uint8_t *features = featurev.data();
		std::vector<uint8_t> working_setv(working_set_size);
		uint8_t *working_set = working_setv.data();

#define A_WIDTH 320
#define A_HEIGHT 240

		uint8_t kind7[A_WIDTH * A_HEIGHT];
		uint8_t kind3[A_WIDTH * A_HEIGHT];
		ire3_eye_info eye_info = { 0 };
		ire3_out_images images;
		images.out_image_width = A_WIDTH;
		images.out_image_height = A_HEIGHT;
		images.kind7_pixels = kind7;
		images.kind3_pixels = kind3;

		retValue = ire3_extract_features_with_images(pixels, width, height, features, max_features_size,
			NULL, &eye_info, sizeof(eye_info), working_set,
			working_set_size, &settings, &images);

		if (retValue != MARVIS_AUTH_E_SUCCESS)
		{
			retValue = MARVIS_AUTH_E_BAD_TEMPLATE;
			return retValue;
		}

		WriteAllText1("kind3.raw", (char*)kind3, A_WIDTH * A_HEIGHT);
		WriteAllText1("kind7.raw", (char*)kind7, A_WIDTH * A_HEIGHT);

		t_width = images.out_image_width;
		t_height = images.out_image_height;
		t_final_size = 3500;

		unsigned char* K7Jp2Data = (unsigned char*)calloc(A_WIDTH * A_HEIGHT, sizeof(unsigned char));
		int K7FinalSize = 0;

		if (t_K7 == true)
		{
			retValue = jpeg2k_encode_mem(kind7, A_WIDTH, A_HEIGHT, K7Jp2Data, &K7FinalSize, 23);
		}

		else
		{
			retValue = jpeg2k_encode_mem(kind3, A_WIDTH, A_HEIGHT, K7Jp2Data, &K7FinalSize, 23);
		}
		K7Jp2Data[K7FinalSize] = 0;

		if (retValue != MARVIS_AUTH_E_SUCCESS)
		{
			free(K7Jp2Data);
			retValue = MARVIS_AUTH_E_BAD_TEMPLATE;
			return retValue;
		}


		WriteAllText1("K7Dev.jp2", (char*)K7Jp2Data, K7FinalSize);

		unsigned char* K7Jp2ISOData = (unsigned char*)calloc(K7FinalSize + 68, sizeof(unsigned char));

		retValue = CreateISOImageIris_11(K7Jp2Data, K7Jp2ISOData, K7FinalSize, A_HEIGHT, A_WIDTH);

		if (retValue != MARVIS_AUTH_E_SUCCESS)
		{
			free(K7Jp2Data);
			free(K7Jp2ISOData);
			retValue = MARVIS_AUTH_E_BAD_TEMPLATE;
			return retValue;
		}

		K7FinalSize = K7FinalSize + 68;

		memcpy(Jp2Isoimage, K7Jp2ISOData, K7FinalSize); //without IIR only jp2  + 68
		t_final_size = K7FinalSize;

		WriteAllText1("K7iso.iso", (char*)K7Jp2ISOData, K7FinalSize);

		free(K7Jp2Data);
		free(K7Jp2ISOData);
	}
	catch (exception &ex)
	{
		char szOutputString[256] = { 0 };
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] : %s", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
	}
	catch (...)
	{
		retValue = -1;
	}

	return retValue;
}

unsigned char * rotate180(unsigned char *in_img, int in_width, int in_height)
{
	if (in_img == NULL)
		return NULL;

	unsigned char *out = (unsigned char *)calloc(in_width*in_height + 1, sizeof(unsigned char));

	memcpy(out, in_img, in_width * in_height);

	for (size_t i = 0; i < in_height; i++)
	{
		for (size_t j = 0; j < in_width / 2; j++)
		{
			int firstLoc = (i * in_width) + j;
			int secondLoc = ((i + 1) * in_width) - j - 1;

			unsigned char chVal = out[firstLoc];
			out[firstLoc] = out[secondLoc];
			out[secondLoc] = chVal;
		}
	}

	return out;
}

float MARVIS_Enroll_GetLeftFPS()
{
	QueryPerformanceCounter(&FpsEndtime);

	double dTimeDifference = (double)(FpsEndtime.QuadPart - FpsStartTime.QuadPart) / double(FpsframeFrequency.QuadPart);

	if (dTimeDifference >= 1.0f)
	{
		m_fFPS = (float)m_NoOfFrameCount;

		char szOutputString[1024] = "";
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line# [%d] - FPS is [%0.3f].\n",
			__FUNCTION__, __LINE__, m_fFPS);


		//OutputDebugStringA(szOutputString);
		m_NoOfFrameCount = 0;
		QueryPerformanceFrequency(&FpsframeFrequency);
		QueryPerformanceCounter(&FpsStartTime);
	}
	return m_fFPS;

}

string GetIrisFunctionInfo()
{
	return g_executionTimeInfo;
}

int  MARVIS_Enroll_SetIRLed(int m_pupil_mean)
{
	int tempRet;

	/*CommSetIRLed(COMM_DUTY_CYCLE_LEVEL2);
	return 0;*/

	if (m_pupil_mean <= 40 && lastIRLED != COMM_DUTY_CYCLE_LEVEL15) {
		tempRet = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL15);
		lastIRLED = COMM_DUTY_CYCLE_LEVEL15;
	}
	else if ((m_pupil_mean > 40 && m_pupil_mean <= 50) && lastIRLED != COMM_DUTY_CYCLE_LEVEL12) {
		tempRet = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL12);
		lastIRLED = COMM_DUTY_CYCLE_LEVEL12;
	}
	else if ((m_pupil_mean > 50 && m_pupil_mean <= 60) && lastIRLED != COMM_DUTY_CYCLE_LEVEL10) {
		tempRet = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL10);
		lastIRLED = COMM_DUTY_CYCLE_LEVEL10;
	}
	else if ((m_pupil_mean > 60 && m_pupil_mean <= 70) && lastIRLED != COMM_DUTY_CYCLE_LEVEL8) {
		tempRet = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL8);
		lastIRLED = COMM_DUTY_CYCLE_LEVEL8;
	}
	else if ((m_pupil_mean > 70 && m_pupil_mean <= 80) && lastIRLED != COMM_DUTY_CYCLE_LEVEL6) {
		tempRet = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL6);
		lastIRLED = COMM_DUTY_CYCLE_LEVEL6;
	}
	else if ((m_pupil_mean > 80 && m_pupil_mean <= 90) && lastIRLED != COMM_DUTY_CYCLE_LEVEL4) {
		tempRet = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL4);
		lastIRLED = COMM_DUTY_CYCLE_LEVEL4;
	}
	else if ((m_pupil_mean > 90 && m_pupil_mean <= 100) && lastIRLED != COMM_DUTY_CYCLE_LEVEL2) {
		tempRet = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL2);
		lastIRLED = COMM_DUTY_CYCLE_LEVEL2;
	}
	else if ((m_pupil_mean > 100) && lastIRLED != COMM_DUTY_CYCLE_OFF) {
		tempRet = CommSetIRLed(COMM_DUTY_CYCLE_OFF);
		lastIRLED = COMM_DUTY_CYCLE_OFF;
	}

	return tempRet;
}

#pragma endregion

#pragma region Match Template Function

static int compare_one_probe(int dummy_id, std::string id_, unsigned char* img, int width, int height, const features_entry *features_list, size_t max_features_size, int size_feature_list,
	size_t des_features_size, size_t working_set_size, const ire3_settings *settings, int *out_score) {

	char szOutputString[256] = { 0 };
	try
	{
		const char *id = id_.c_str();

		auto working_set = std::make_unique<uint8_t[]>(working_set_size);
		auto features = std::make_unique<uint8_t[]>(max_features_size);
		if (!features) {
			//perror("malloc");
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] memory allocation  Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);
			return MARVIS_AUTH_E_MEMORY;
		}
		memset(features.get(), 0, max_features_size);

		auto des_features = std::make_unique<uint8_t[]>(des_features_size);
		if (!des_features) {
			//perror("malloc");
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] memory allocation Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return MARVIS_AUTH_E_MEMORY;
		}


		//create 16-bit array to store scores for binary file output
		//auto scores = (size_feature_list > 0) ? std::make_unique<uint16_t[]>(size_feature_list * sizeof(uint16_t)) : NULL;

		if (img == NULL) {
			//fprintf(stderr, "failed: read_pgm %s\n", ifile);
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] image not found Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return MARVIS_AUTH_E_INVALIDPARAM;
		}

		ire3_eye_info einfo;
		int rc;
		rc = ire3_extract_features(img, width, height, features.get(), max_features_size, NULL, &einfo, sizeof(einfo), working_set.get(), working_set_size, settings);

		if (IRE3_FAILED(rc) || (rc == IRE3_STATUS_FAIL)) {
			//fprintf(stderr, "failed: feature extraction on the file %s rc = 0%08x, continuing to the next one\n", ifile, rc);
			//fprintf(ef, "failed: feature extraction on the file %s rc = 0%08x\n", ifile, rc);
			/*if (IRE3_FAILED(rc) || (rc == IRE3_STATUS_FAIL && use_extract_fail == 0)) {
			return;
			}*/
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] feature extraction Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);
			return MARVIS_AUTH_E_MATCH_IMAGE_FAILED;
		}

		/*free(img);
		img = NULL;*/

		rc = ire3_deserialize_features(features.get(), max_features_size, des_features.get(), des_features_size);

		if (IRE3_FAILED(rc)) {
			//fprintf(stderr, "failed: de-serialize features on the file %s rc = 0%08x, continuing to the next one\n", ifile, rc);
			//fprintf(ef, "failed: de-serialize features on the file %s rc = 0%08x\n", ifile, rc);
			// empty features will result in 0 score for generalized DET
			memset(des_features.get(), 0, des_features_size);
		}

		/*if (scores != NULL) {
			memset(scores.get(), 0, size_feature_list * sizeof(uint16_t));
			}*/

		int i = 0;
		for (const features_entry *fe = features_list; fe; fe = fe->next, i++) {
			int score = 0;

			rc = ire3_compare(des_features.get(), fe->des_features, settings, out_score);
			if (rc) {
				//fprintf(stderr, "failed: ire3_compare rc = 0%08x\n", rc);
				_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
					"%s - Line#[%d] ire3_compare Fail.", __FUNCTION__, __LINE__);
				LOG_ERROR(szOutputString);

				return MARVIS_AUTH_E_MATCH_IMAGE_FAILED;
			}


		}
		//if (scores != NULL && size_feature_list > 0) {

		//	_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
		//		"%s - Line#[%d] score or feature list null or zero Fail.", __FUNCTION__, __LINE__);
		//	LOG_ERROR(szOutputString);

		//	return MARVIS_AUTH_E_MATCH_IMAGE_FAILED;
		//	/*if (fwrite(scores.get(), sizeof(uint16_t), size_feature_list, lcf) != size_feature_list) {
		//	fprintf(stderr, "failed: writing to binary file scores");
		//	return;
		//	}*/
		//}
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] exception Fail.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);

		return MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}

	return MARVIS_AUTH_E_SUCCESS;
}

static features_entry *new_features_entry(char *id, size_t des_features_size) {
	features_entry *fe = NULL;

	fe = (features_entry *)calloc(1, sizeof(features_entry));
	if (!fe) {
		perror("calloc");
		goto cleanup;
	}

	fe->des_features = (uint8_t *)malloc(des_features_size);
	if (!fe->des_features) {
		perror("malloc");
		goto cleanup;
	}

	fe->id = strdup(id);
	if (!fe->id) {
		perror("strdup");
		goto cleanup;
	}

	fe->next = NULL;
	return fe;

cleanup:

	if (fe) {
		if (fe->id)
			free(fe->id);

		if (fe->des_features)
			free(fe->des_features);
	}
	return NULL;
}

unsigned int enroll_extract_features(unsigned char* enroll_Data, int width, int height, features_entry **features_list, size_t max_features_size,
	size_t des_features_size, uint8_t *working_set, size_t working_set_size, int *cnt, const ire3_settings *settings) {

	char id[256], ifile0[256], *p1, *p2, *ifile1;

	int rc, retval = 0;
	int index = 0;
	features_entry *prev_fe = NULL, *fe = NULL;
	uint8_t *features;
	ire3_eye_info einfo;

	*cnt = 0;
	//vector<string> a_img;

	if (enroll_Data == NULL)
		return -1;

	features = (uint8_t *)malloc(max_features_size);
	if (!features) {
		return MARVIS_AUTH_E_MEMORY;
	}

	while (index < 1) {
		//char *ifile = (char *)in_img[index].c_str();

		std::string s = std::to_string(index);
		char const *pchar = s.c_str();  //use char const* as target type

		strcpy(id, pchar);

		int already_extracted = 0;
		uint8_t *img = NULL;

		fe = *features_list;
		while (fe != NULL) {
			// already already_extracted?
			if (m_stricmp(id, fe->id) == 0) {
				already_extracted = 1;
				break;
			}
			fe = fe->next;
		}

		if (already_extracted == 1) {
			//fprintf(stderr, "already extracted %s, skipping new image\n", id);
			index++;
			continue;
		}


		if (enroll_Data == NULL)
		{
			//cout << "Image not found." << endl;

			index++;
			continue;
		}

		/*if (img == NULL) {
		index++;
		continue;
		}*/

		memset(features, 0, max_features_size);
		rc = ire3_extract_features(enroll_Data, width, height, features, max_features_size, NULL, &einfo, sizeof(einfo), working_set, working_set_size, settings);

		if (IRE3_FAILED(rc)) {
			//fprintf(stderr, "failed: ire3_extract_features on the file %s rc = 0%08x\n", ifile, rc);
			goto loop_end1;
		}

		if (rc == IRE3_STATUS_FAIL) {
			//fprintf(stderr, "failed: feature extraction on the file %s rc = 0%08x, continuing to the next one\n", ifile, rc);
			//fprintf(ef, "failed: feature extraction on the file %s rc = 0%08x\n", ifile, rc);
			/*if (use_extract_fail == 0) {
			goto loop_end3;
			}*/

			goto loop_end3;
		}

		fe = new_features_entry(id, des_features_size);
		if (!fe) {
			//fprintf(stderr, "failed: new_features_entry on file %s\n", ifile);
			retval = IRE3_ERR_OUTOFMEMORY;
			goto loop_end3;
		}

	loop_des_ftr:
		rc = ire3_deserialize_features(features, max_features_size, fe->des_features, des_features_size);
		if (IRE3_FAILED(rc)) {
			//fprintf(stderr, "failed: de-serialize features on the file %s rc = 0%08x, continuing to the next one\n", ifile, rc);
			//fprintf(ef, "failed: de-serialize features on the file %s rc = 0%08x\n", ifile, rc);
			// empty features will result in 0 score for generalized DET
			memset(fe->des_features, 0, des_features_size);
			goto loop_end1;
		}
	loop_add_ftr:

		if (*features_list == NULL) {
			*features_list = fe;
		}

		if (prev_fe != NULL) {
			prev_fe->next = fe;
		}
		prev_fe = fe;
		(*cnt)++;
		//	a_img.push_back(in_img[index]);
		goto loop_end3;

	loop_end1:
		if (fe) {
			free(fe);
		}
	loop_end3:
		if (img) {
			free(img);
		}

		index++;
	}


	free(features);
	return retval;
}

features_entry * EnrollIris_Single(unsigned char* enroll_Data, int in_width, int in_height)
{
	char szOutputString[256] = { 0 };
	try
	{
		int num_features_entries, rc;

		features_entry *features_list = NULL;
		ire3_settings settings = iris_settings; // { sizeof(settings), 200, 4 };

		uint8_t *working_set = NULL;
		size_t working_set_size, max_features_size, des_features_size;

		rc = ire3_get_max_features_size(&max_features_size);
		if (rc) {
			//fprintf(stderr, "failed: ire3_get_max_features_size rc = 0%08x\n", rc);

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ire3_get_max_features_size Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return NULL;
		}

		rc = ire3_get_extraction_working_set_size(&working_set_size, &settings);
		if (rc) {
			//fprintf(stderr, "failed: ire3_get_extraction_working_set_size rc = 0%08x\n", rc);

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ire3_get_extraction_working_set_size Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return NULL;
		}

		rc = ire3_get_deserialized_features_size(&des_features_size);
		if (rc) {
			//fprintf(stderr, "failed: ire3_get_deserialized_features_size rc = 0%08x\n", rc);

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ire3_get_deserialized_features_size Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return NULL;
		}

		working_set = (uint8_t *)malloc(working_set_size);
		if (!working_set) {

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] memory allication Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);
			return NULL;
		}

		//Enroll features
		rc = enroll_extract_features(enroll_Data, in_width, in_height, &features_list, max_features_size, des_features_size, working_set, working_set_size,
			&num_features_entries, &settings);
		if (rc) {
			//fprintf(stderr, "failed: extract_features rc = 0%08x\n", rc);

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] enroll_extract_features fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return NULL;
		}

		printf("created %d templates\n", num_features_entries);

		free(working_set);
		return features_list;

	}
	catch (...)
	{
		return NULL;
	}
}

static unsigned int match_features(unsigned char* imgMatch, int width, int height, features_entry *features_list, size_t max_features_size,
	size_t des_features_size, uint8_t *working_set, size_t working_set_size, const ire3_settings *settings, int* out_score) {

	int retval = IRE3_STATUS_OK;
	int size_feature_list = 0;
	int index = 0;

	for (features_entry *fe = features_list; fe; fe = fe->next) {
		size_feature_list++;
	}

	retval = compare_one_probe(0, to_string(index), imgMatch, width, height, features_list, max_features_size, size_feature_list,
		des_features_size, working_set_size, settings, out_score);

	return retval;
}

int  MatchIris(features_entry *enroll_features_list, unsigned char* imgMatch, int in_width, int in_height, int* out_score)
{
	char szOutputString[256] = { 0 };
	int ret = MARVIS_AUTH_E_MATCH_IMAGE_FAILED;
	try
	{
		int num_features_entries, rc;

		features_entry *features_list = enroll_features_list;
		ire3_settings settings = iris_settings; //{ sizeof(settings), 200, 4 };

		uint8_t *working_set = NULL;
		size_t working_set_size, max_features_size, des_features_size;

		rc = ire3_get_max_features_size(&max_features_size);
		if (rc) {
			//fprintf(stderr, "failed: ire3_get_max_features_size rc = 0%08x\n", rc);

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ire3_get_max_features_size Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return MARVIS_AUTH_E_MATCH_IMAGE_FAILED;
		}

		rc = ire3_get_extraction_working_set_size(&working_set_size, &settings);
		if (rc) {
			//fprintf(stderr, "failed: ire3_get_extraction_working_set_size rc = 0%08x\n", rc);
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ire3_get_extraction_working_set_size Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return MARVIS_AUTH_E_MATCH_IMAGE_FAILED;
		}

		rc = ire3_get_deserialized_features_size(&des_features_size);
		if (rc) {
			//fprintf(stderr, "failed: ire3_get_deserialized_features_size rc = 0%08x\n", rc);

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ire3_get_deserialized_features_size Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return MARVIS_AUTH_E_MATCH_IMAGE_FAILED;
		}

		working_set = (uint8_t *)malloc(working_set_size);
		if (!working_set) {
			//perror("malloc");
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] memory allication Fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return MARVIS_AUTH_E_MEMORY;
		}

		///Match Image

		rc = match_features(imgMatch, in_width, in_height, features_list, max_features_size, des_features_size, working_set, working_set_size,
			&settings, out_score);
		if (rc) {
			//fprintf(stderr, "failed: compare rc = 0%08x\n", rc);

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] match_features fail.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);

			return MARVIS_AUTH_E_MATCH_IMAGE_FAILED;
		}

		if (working_set != NULL)
		{
			free(working_set);
			working_set = NULL;
		}

		ret = MARVIS_AUTH_E_SUCCESS;
	}
	catch (...)
	{
		ret = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	return ret;
}


#pragma endregion


#pragma region Fake_Check
bool ReadFile1(const char* filePath, unsigned char*& buffer, long* fileSize)
{
	FILE* file = fopen(filePath, "rb");
	if (!file) {
		//printf("Failed to open file: %s\n", CT2A(filePath));
		return false;
	}

	fseek(file, 0, SEEK_END);
	long Size = ftell(file);
	fseek(file, 0, SEEK_SET);


	buffer = new unsigned char[Size];


	size_t bytesRead = fread(buffer, 1, Size, file);
	if (bytesRead != Size) {
		//printf("Failed to read file: %s\n", CT2A(filePath));
		delete[] buffer;
		buffer = nullptr;
		std::fclose(file);
		return false;
	}

	std::fclose(file);
	*fileSize = bytesRead;
	return true;
}

float GetAngle(int x1, int y1, int x2, int y2)
{
	float angle = atan2(y1 - y2, x1 - x2);

	return angle * 180 / M_PI;
}

void FilterCreationGaussian(unsigned char* in_image, int w, int h)
{
	// initialising standard deviation to 1.0
	double sigma = 1.0;
	double r, s = 2.0 * sigma * sigma;

	// sum is for normalization
	double sum = 0.0;

	for (int i = 2; i < h - 2; ++i)
	{
		for (int j = 2; j < w - 2; ++j)
		{
			sum = 0.0;
			// generating 5x5 kernel
			for (int h1 = -2; h1 <= 2; h1++) {
				for (int w1 = -2; w1 <= 2; w1++) {
					//r = sqrt(h1 * h1 + w1 * w1);
					//GKernel[x + 2][y + 2] = (exp(-(r * r) / s)) / (M_PI * s);
					//in_image[((i + h1) * w) + (j + w1)] = (exp(-(r * r) / s)) / (M_PI * s);
					sum += in_image[((i + h1) * w) + (j + w1)];
				}
			}
			in_image[(i * w) + j] = sum / 25;
		}
	}

	// normalising the Kernel
	/*for (int i = 0; i < 5; ++i)
	for (int j = 0; j < 5; ++j)
	GKernel[i][j] /= sum;*/
}

int GetLacrimalCaruncleMean(unsigned char *in_rawImgOrg, int rawImgWidth, int rawImgHeight,
	int iris_center_x, int iris_center_y, int iris_center_r, int pupil_center_r,
	unsigned char *out_crpImg, int &crpWidth, int &crpHeight, float &out_mean)
{
	try
	{
		unsigned char *in_rawImg = (unsigned char*)calloc((640 * 480) + 10, sizeof(unsigned char));

		memcpy(in_rawImg, in_rawImgOrg, 640 * 480);

		//FilterCreationGaussianLeft(in_rawImg, 640, 480);

		//Eye Left Side
		int LT_X = (640 - iris_center_x) + iris_center_r + 10;// +(iris_center_r / 3);
		int LT_Y = iris_center_y - iris_center_r;

		int RT_X = LT_X + iris_center_r + (iris_center_r / 2) + (iris_center_r / 3);
		int RT_Y = iris_center_y + iris_center_r + (iris_center_r / 2);

		if (LT_X <= 0)
			LT_X = 0;

		if (RT_X > rawImgWidth - 1)
			RT_X = rawImgWidth;

		if (LT_Y <= 0)
			LT_Y = 0;

		if (LT_Y > rawImgHeight - 1)
			LT_Y = rawImgHeight;

		if (RT_Y <= 0)
			RT_Y = 0;

		if (RT_Y > rawImgHeight - 1)
			RT_Y = rawImgHeight;

		int crpSumVal = 0;
		int crpW = (RT_X - LT_X);
		int crpH = (RT_Y - LT_Y);
		float lc_crpMean = 0;

		crpWidth = crpW;
		crpHeight = crpH;

		if (out_crpImg == NULL)
			return 0;

		//out_crpImg = (unsigned char *)calloc((crpW*crpH) + 1, sizeof(unsigned char));

		for (size_t i = LT_Y; i < RT_Y; i++)
		{
			for (size_t j = LT_X; j < RT_X; j++)
			{
				//in_rawImg[(i * 640) + j] = 255;

				int pixVal = in_rawImg[(i * 640) + j];

				if (i == LT_Y || i == RT_Y - 1 || j == LT_X || j == RT_X - 1)
					in_rawImg[(i * 640) + j] = 0;

				out_crpImg[((i - LT_Y) * crpW) + (j - LT_X)] = pixVal;

				crpSumVal += pixVal;
			}
		}

		lc_crpMean = crpSumVal / (crpH*crpW);

		/*if (lc_crpMean<150 || lc_crpMean>225)
		retVal = false;*/

		if (in_rawImg != NULL)
		{
			free(in_rawImg);
			in_rawImg = NULL;
		}
		out_mean = lc_crpMean;
	}
	catch (...)
	{
		int a = 0;
	}
	return 0;
}

float CircleDistance(int x1, int y1, int x2, int y2)
{
	// Calculating distance
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}

bool FindLacrimalCaruncle(unsigned char *in_crpImg, int crpWidth, int crpHeight, int iris_center_x, int iris_center_y,
	int iris_center_r, int pupil_center_r, int iris_mean, int pupil_mean, float crpMean)
{
	bool retVal = false;

	int pixVal = 0;
	int pixVal1 = 0;
	int crpSumVal = 0;
	int crpW = crpWidth;
	int crpH = crpHeight;

	if (crpWidth < 150)
		return false;

	int TotalCrpArea = 0;
	int BlackPixCount = 0;

	//find black to white x,y
	vector<vector<pair<pair<int, int>, pair<int, int>>>> TotlalLineListPt;
	vector<pair<pair<int, int>, pair<int, int>>> linelistPt; //x,dist, black-y, white-y
	vector <float> vecAcngle;

	int multiplelineCnt = 0;

	int validAngleCnt = 0;
	int InValidLengthCnt = 0;


	for (size_t i = 0; i < crpH; i++)
	{//y
		for (size_t j = 0; j < crpW; j++)
		{//x
			pixVal = in_crpImg[(i * crpW) + j];

			int offset = 30;

			if (i <= crpH - 100)
				offset = 30;
			else
				offset = 5;

			if (crpMean > 150 && pixVal > crpMean - offset)
				pixVal = 255;
			else
			{
				pixVal = 0;
				//BlackPixCount++;
			}


			float circleDist = CircleDistance(crpW - 50, 0, j, i);

			if (circleDist <= 100)
				pixVal = 255;

			circleDist = CircleDistance(70, 0, j, i);

			if (circleDist <= 50)
				pixVal = 255;

			in_crpImg[(i * crpW) + j] = pixVal;

			if (i <= crpH - 100 && j <= 25)
			{
				if (pixVal == 0)
				{
					BlackPixCount++;
				}
				TotalCrpArea++;
			}
		}
	}

	for (size_t i = 0; i < crpW; i += 10)
	{//x
		for (size_t j = 1; j < crpH; j++)
		{//y
			pixVal = in_crpImg[((j)* crpW) + (i)];
			pixVal1 = in_crpImg[(((j - 1)) * crpW) + (i)];

			//crpImg[((j - LT_Y) * crpW) + (i - LT_X)] = 0;

			if (pixVal == 0 /*&& pixVal1 == 255*/)
			{
				size_t k = 0;
				bool IsWhiteFound = false;
				for (k = j; k < crpH; k++)
				{//y
					pixVal = in_crpImg[((k)* crpW) + (i)];

					if (pixVal == 255)
					{
						IsWhiteFound = true;
						break;
					}
					else
					{
						//crpImg[((k - LT_Y) * crpW) + (i - LT_X)] = 255;
					}
				}

				if (IsWhiteFound)
				{
					int x0 = (i);
					int y0 = (j);
					int y1 = (k);

					//if (y1 - y0 >= 20 && y1-y0 <= 90)
					if (y1 - y0 >= 30)
					{
						if (linelistPt.size() > 0)
						{
							int	lastLoc = linelistPt.size() - 1;

							if (abs(y0 - linelistPt[lastLoc].second.second) <= 30)
							{
								linelistPt[lastLoc].first.second = y1 - linelistPt[lastLoc].second.first;
								linelistPt[lastLoc].second.second = y1;
							}
							else
								linelistPt.push_back(make_pair(make_pair(x0, y1 - y0), make_pair(y0, y1)));
						}
						else
							linelistPt.push_back(make_pair(make_pair(x0, y1 - y0), make_pair(y0, y1)));
					}
				}

				j = k + 1;
			}
		}

		if (linelistPt.size() > 0)
		{
			if (linelistPt.size() >= 2 && linelistPt.size() <= 5)
			{
				multiplelineCnt++;
			}
			TotlalLineListPt.push_back(linelistPt);
		}

		linelistPt.clear();
	}

	if (TotlalLineListPt.size() > 1)
	{
		for (size_t i0 = 0; i0 < TotlalLineListPt.size() - 1; i0++)
		{
			//if (TotlalLineListPt[i0].size() >= 2 && TotlalLineListPt[i0].size() <= 5)
			{
				for (size_t j0 = 0; j0 < 1/*TotlalLineListPt[i0].size() - 1*/; j0++)
				{
					if (TotlalLineListPt[i0][j0].first.second >= 90)
						InValidLengthCnt++;
					else
					{
						int x1 = TotlalLineListPt[i0][j0].first.first;
						int y1 = TotlalLineListPt[i0][j0].second.first;

						int x2 = TotlalLineListPt[i0 + 1][j0].first.first;
						int y2 = TotlalLineListPt[i0 + 1][j0].second.first;

						float angDegree = GetAngle(x1, y1, x2, y2);

						if (angDegree <= -115 && angDegree >= -190 || (angDegree >= 178 && angDegree <= 182))
							validAngleCnt++;

						vecAcngle.push_back(angDegree);

						for (size_t j = TotlalLineListPt[i0][j0].second.first; j < TotlalLineListPt[i0][j0].second.second; j++)
						{
							in_crpImg[(j * crpW) + TotlalLineListPt[i0][j0].first.first] = 255;
						}
					}
				}
			}
		}
	}

	//bool retVal = false;

	if (validAngleCnt >= 6 && InValidLengthCnt <= 4 &&
		validAngleCnt > InValidLengthCnt && abs(validAngleCnt - InValidLengthCnt) >= 5)
		retVal = true;
	else
		retVal = false;

	bool IsSaveImage = false;
	if (IsSaveImage)
	{

#ifdef LC_CRP_THRESH_IMG_SAVE
		string sName = "TestImages\\lc-crp-thresh-L" + to_string(m_NoOfFrameCountL) + "_" + to_string((int)crpMean)
			+ "_" + to_string(validAngleCnt) + "_" + to_string(InValidLengthCnt) + "_" + to_string((int)BlackPixCount) + "_" + to_string((int)TotalCrpArea) + ".bmp";

		int pad = ((crpW % 4) == 0) ? 0 : (4 - (crpW % 4));
		//crpW += pad;

		int tImgLen = (crpW + pad) * crpH;
		unsigned char *crpBmp = (unsigned char *)calloc(tImgLen + 1078 + 1, sizeof(unsigned char));
		try
		{
			iplConvertRawToBmp(in_crpImg, crpBmp, crpH, crpW, 0);

			//tImgLen = (crpW - pad)* crpH;
			WriteAllText1(sName.c_str(), (char *)crpBmp, tImgLen + 1078);
			free(crpBmp);
		}
		catch (...)
		{
		}
#endif
	}

	return retVal;
}



void GetIrisInfoLeft(unsigned char *in_rawImg, int w, int h, int LT_X, int LT_Y, int RT_X, int RT_Y,
	int iris_center_x, int iris_center_y, int iris_center_r, int pupil_center_r,
	int BrightMeanThreshold, int outEyeMean, int inEyeMean,
	float &out_BrightPercent, float &out_OutEyePercent, float &out_InEyePercent)
{
	int pixVal = 0;

	int BrightMeanCnt = 0;
	int BrightMeanTotal = 0;

	int OutEyeCnt = 0;
	int OutEyeTotal = 0;

	int InEyeCnt = 0;
	int InEyeTotal = 0;

	int outEyeBorderOffsetPix = 30;
	int outEyeBorderOffsetPixWidth = 10;

	for (size_t i = LT_Y; i < RT_Y; i++)
	{
		for (size_t j = LT_X; j < RT_X; j++)
		{
			//in_rawImg[(i * 640) + j] = 0;

			pixVal = in_rawImg[(i * 640) + j];

			float circleDist = CircleDistance((640 - iris_center_x), iris_center_y, j, i);
			//float pupilDist = CircleDistanceLeft((640 - iris_center_x), iris_center_y, j, i);

			if (circleDist >= (iris_center_r + 10))
			{//OUTER EYE PART
				//in_rawImg[(i * 640) + j] = 0;

				if (pixVal < (outEyeMean + 10))
				{
					OutEyeCnt++;

					in_rawImg[(i * 640) + j] = 0;
				}

				OutEyeTotal++;

			}
			else if ((circleDist >= (iris_center_r - outEyeBorderOffsetPix) && (circleDist < (iris_center_r - 10)) &&
				(j >(640 - iris_center_x) - pupil_center_r - outEyeBorderOffsetPixWidth) &&
				(j < (640 - iris_center_x) + pupil_center_r + outEyeBorderOffsetPixWidth)))
			{
				//in_rawImg[(i * 640) + j] = 0;

				if (pixVal >= (inEyeMean + (inEyeMean / 3)) && (pixVal < BrightMeanThreshold + 10))
				{
					OutEyeCnt++;

					in_rawImg[(i * 640) + j] = 0;
				}

				OutEyeTotal++;
			}
			else if (circleDist <= (iris_center_r - 15))
			{//INNER EYE PART
				if (circleDist >= (pupil_center_r + 5))
				{
					if (pixVal < (inEyeMean - 10))
					{
						InEyeCnt++;

						in_rawImg[(i * 640) + j] = 255;
					}

					InEyeTotal++;

					if (pixVal > BrightMeanThreshold)
					{
						BrightMeanCnt++;

						in_rawImg[(i * 640) + j] = 0;
					}
					BrightMeanTotal++;
				}
			}

		}
	}

	OutEyeTotal /= 1.5;
	if (BrightMeanTotal <= 0)
		BrightMeanTotal = 1;
	if (OutEyeTotal <= 0)
		OutEyeTotal = 1;
	if (InEyeTotal <= 0)
		InEyeTotal = 1;

	out_BrightPercent = BrightMeanCnt * 100 / BrightMeanTotal;
	out_OutEyePercent = OutEyeCnt;// *100 / OutEyeTotal;
	out_InEyePercent = InEyeCnt * 100 / InEyeTotal;


}

bool FindUpperEyeLid_Inside(unsigned char *in_bmpImg, int bmpLen, unsigned char *in_rawImgOrg, int iris_center_x, int iris_center_y,
	int iris_center_r, int pupil_center_r, int iris_mean, int pupil_mean)
{
	unsigned char *in_rawImg = (unsigned char*)calloc((640 * 480) + 10, sizeof(unsigned char));

	memcpy(in_rawImg, in_rawImgOrg, 640 * 480);

	//FilterCreationGaussianLeft(in_rawImg, 640, 480);

	//Eye Lower Part
	int LT_X = (640 - iris_center_x) - iris_center_r - 30;// (iris_center_r / 2);
	int LT_Y = iris_center_y;

	int RT_X = (640 - iris_center_x) + iris_center_r + 30;// (iris_center_r / 2);
	int RT_Y = iris_center_y + iris_center_r + iris_center_r;

	if (LT_X <= 0)
		LT_X = 0;

	if (RT_X > 639)
		RT_X = 640;

	if (LT_Y <= 0)
		LT_Y = 0;

	if (LT_Y > 379)
		LT_Y = 480;

	if (RT_Y <= 0)
		RT_Y = 0;

	if (RT_Y > 479)
		RT_Y = 480;


	if (iris_mean > 85)
		iris_mean -= 15;

	int BrightMean = 190;
	int outEyeMean = iris_mean + (iris_mean / 3);

	//if (iris_mean > 85)
	//{
	//	outEyeMean = iris_mean + (iris_mean / 2);
	//}
	//else if (iris_mean > 60)
	//{
	//	outEyeMean = iris_mean + iris_mean;
	//}
	//else// if (iris_mean > 40)
	//{
	//	outEyeMean = iris_mean + iris_mean + (iris_mean / 2);
	//}

	if (iris_mean > 125)
	{
		outEyeMean = iris_mean + 25;// (iris_mean / 2);
	}
	else if (iris_mean > 85)
	{
		outEyeMean = iris_mean + (iris_mean / 2);
	}
	/*else if (iris_mean > 60)
	{
	outEyeMean = iris_mean + iris_mean;
	}*/
	else if (iris_mean < 40)
	{
		outEyeMean = iris_mean + (iris_mean / 2) + (iris_mean / 3);
	}

	float out_BrightPercent[2] = { 0 };
	float out_OutEyePercent[2] = { 0 };
	float out_InEyePercent[2] = { 0 };


	GetIrisInfoLeft(in_rawImg, 640, 480, LT_X, LT_Y, RT_X, RT_Y, iris_center_x, iris_center_y, iris_center_r, pupil_center_r,
		BrightMean, outEyeMean, iris_mean, out_BrightPercent[1], out_OutEyePercent[1], out_InEyePercent[1]);



	bool retVal = false;

	string f = "Flip";

	//if (out_OutEyePercent[0] > out_OutEyePercent[1] && out_InEyePercent[0] < 25 && out_InEyePercent[1] < 25)
	//if (abs(out_OutEyePercent[0] - out_OutEyePercent[1]) > 2 && out_OutEyePercent[0] >= out_OutEyePercent[1] && out_InEyePercent[1] < 35)
	if (out_BrightPercent[1] == 0 && out_OutEyePercent[1] == 0 && out_InEyePercent[1] == 0)
		retVal = true;
	else if (out_OutEyePercent[1] <= 3000 && out_InEyePercent[1] <= 15)
	{
		if (out_BrightPercent[1] <= 10)
		{
			retVal = false;
			f = "Ok";
		}
		else
			retVal = true;
	}
	else
		retVal = true;

	bool IsSaveImage = false;
	if (IsSaveImage)
	{
		/*string sName = "TestImages\\FlipEyeLid-L_" + to_string(m_NoOfFrameCountL) + "_" + f + "_" +
			to_string((int)out_BrightPercent[1]) + "_" + to_string((int)out_OutEyePercent[1]) + "_"
			+ to_string((int)out_InEyePercent[1]) + "_" + to_string(iris_mean) + "_" + to_string(pupil_mean) + ".bmp";*/

		int tImgLen = 640 * 480;
		unsigned char *fullImg = (unsigned char *)calloc(tImgLen + 1078 + 1, sizeof(unsigned char));
		iplConvertRawToBmp(in_rawImg, fullImg, g_ipl_info[0].FINAL_FRAME_HEIGHT, g_ipl_info[0].FINAL_FRAME_WIDTH, 0);
		//WriteAllText1(sName.c_str(), (char *)fullImg, tImgLen + 1078);

		free(fullImg);
	}
	return retVal;

}

bool FindPupilCenter(unsigned char *in_rawImg, int ImgWidth, int ImgHeight,
	int iris_center_x, int iris_center_y, int pupil_center_r, int pupil_mean)
{
	bool IsPupilCenterLoc = false;

	int pixVal = 0;
	int BrightMeanThreshold = 0;
	int BrightMeanCnt = 0;
	int BrightMeanTotal = 0;

	if (pupil_center_r <= 10)
		pupil_center_r = 40;

	int LT_X = (640 - iris_center_x) - pupil_center_r + 10;
	int LT_Y = iris_center_y - pupil_center_r;

	int RT_X = (640 - iris_center_x) + pupil_center_r - 10;
	int RT_Y = iris_center_y + pupil_center_r;

	if (pupil_mean < 60)
		BrightMeanThreshold = 180;
	else
	{
		BrightMeanThreshold = 240 - pupil_mean;
	}

	BrightMeanThreshold = pupil_mean + 50;

	for (size_t i = LT_Y; i < RT_Y; i++)
	{
		for (size_t j = LT_X; j < RT_X; j++)
		{
			//in_rawImg[(i * 640) + j] = 255;

			if (j >((640 - iris_center_x) - pupil_center_r) && j < ((640 - iris_center_x) + pupil_center_r))
			{
				if (i >(iris_center_y - (pupil_center_r / 2)) && i < (iris_center_y + (pupil_center_r / 2)))
				{
					pixVal = in_rawImg[(i * 640) + j];
					//in_rawImg[(i * 640) + j] = 255;

					if (pixVal > BrightMeanThreshold)
					{
						BrightMeanCnt++;

						in_rawImg[(i * 640) + j] = 0;
					}
					BrightMeanTotal++;
				}
			}
		}
	}

	if (BrightMeanCnt > 220)
		IsPupilCenterLoc = true;

	bool IsSaveImage = false;
	if (IsSaveImage)
	{
		/*string sName = "TestImages\\PupilCenter-L_" + to_string(m_NoOfFrameCountL) + "_" +
			to_string(BrightMeanCnt) + "_" + to_string(pupil_mean) + ".bmp";*/

		int tImgLen = 640 * 480;
		unsigned char *fullImg = (unsigned char *)calloc(tImgLen + 1078 + 1, sizeof(unsigned char));
		iplConvertRawToBmp(in_rawImg, fullImg, g_ipl_info[0].FINAL_FRAME_HEIGHT, g_ipl_info[0].FINAL_FRAME_WIDTH, 0);
		//WriteAllText1(sName.c_str(), (char *)fullImg, tImgLen + 1078);

		free(fullImg);
	}
	return IsPupilCenterLoc;
}

bool GetFinalSpoofResult(char* prevImg, int w, int h)
{
#ifndef NORMAL_DETECT
	return true;
#elif FLIP_DETECT
	bool IsValid = false;
	int retValue = 0;
	string TestLog = "";

	bool out_IsLive[2] = { 0 };
	float out_score[2] = { 0 };


	bool tmep_out_IsLive[2] = { 0 };
	float temp_out_score[2] = { 0 };

	retValue = MSSD_CheckLiveness(gModelBuffer, gModelBufferLen, true, gLabelBuffer, (char)gLabelBufferLen,
		(char*)prevImg, w, h, tmep_out_IsLive, temp_out_score);

	if (0 == retValue)
	{
		//Currently form model
		//0:Live
		//1:Fake
		//2:Digital


		//Actual - want
		//0:Fake
		//1:Live
		//2:Digital

		if (tmep_out_IsLive[1] == true && temp_out_score[0] <= temp_out_score[1])
		{
			out_score[0] = temp_out_score[1];
			out_IsLive[0] = true;
			TestLog += to_string(2) + ",";

			IsValid = false;

		}
		else
		{
			if (temp_out_score[0] * 100 > 90)
			{
				out_score[0] = temp_out_score[0];
				out_IsLive[1] = true;
				TestLog += to_string(1) + ",";
			}
			else
			{
				out_score[0] = temp_out_score[1];
				out_IsLive[0] = true;
				TestLog += to_string(2) + ",";

				IsValid = false;
			}

			IsValid = true;
		}

		if (out_score[0] <= out_score[1])
		{
			out_IsLive[0] = out_IsLive[1];
			out_score[0] = out_score[1];
		}
	}
	else
	{
		TestLog += to_string(0) + ",";
	}

	return IsValid;
#endif
}

float CircleDistanceLeft(int x1, int y1, int x2, int y2)
{
	// Calculating distance
	return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) * 1.0);
}

int  Circledata(unsigned char *in_rawImg, int ImgWidth, int ImgHeight,
	int iris_center_x, int iris_center_y, int pupil_center_r, unsigned char* outdata, int *out_black_count)
{
	int white_pixel_count = 0;
	int black_pixel_count = 0;
	int StartX = iris_center_x - pupil_center_r;
	int StartY = iris_center_y - pupil_center_r;
	int width = pupil_center_r * 2;
	int height = pupil_center_r * 2;

	int pupil_y = iris_center_y - pupil_center_r ;
	int pupil_height = pupil_center_r*2;

	int  k = 0;
	int outlen = pupil_center_r * 2;
	memset(outdata, 0, outlen*outlen);
	for (int i = StartY; i < height + StartY; i++)
	{
		for (int j = StartX; j < StartX + width; j++)
		{
			int l = (i * 640) + j;

			float circleDist = CircleDistanceLeft(iris_center_x, iris_center_y, j, i);
			if ((circleDist < (pupil_height / 2)) && (i >= pupil_y) && (i <= pupil_y + pupil_height))
			{
				
			
				outdata[k] = in_rawImg[l];
				if (outdata[k] >= 240)
					white_pixel_count++;
				if (outdata[k] <=25)
					black_pixel_count++;
				k++;
				in_rawImg[l] = 0;
			}
		}
	}

	*out_black_count = black_pixel_count;
	//WriteAllText1("111.raw", (char*)outdata, outlen * outlen);
	return white_pixel_count;
}

int Getblur(unsigned char* in_inputImageData, int in_Width, int in_Height, double* out_meanValue, double* variance)
{
	double sum = 0;                                                               
	int imageSize = in_Width * in_Height;
	for (int i = 0; i < imageSize; ++i)
	{
		sum += in_inputImageData[i];
	}
	*out_meanValue = sum / imageSize;

	double sumSquaredDiff = 0;
	for (int i = 0; i < imageSize; ++i) {
		double diff = in_inputImageData[i] - *out_meanValue;
		sumSquaredDiff += diff * diff;
	}
	*variance = sumSquaredDiff / imageSize;
	//*out_stddevvalue = sqrt(variance);

	return 0;
}


// Function to apply Laplacian filter and calculate its variance
//double CalculateLaplacianVariance(unsigned char* imageData, int width, int height) {
//	std::vector<double> laplacianImage(width * height, 0.0);
//	double sum = 0.0;
//	double sumSquared = 0.0;
//	int index;
//
//	const int laplacianKernel[3][3] = {
//		{ 0, -1, 0 },
//		{ -1, 4, -1 },
//		{ 0, -1, 0 }
//	};
//
//	for (int y = 1; y < height - 1; ++y) {
//		for (int x = 1; x < width - 1; ++x) {
//			double laplacianValue = 0.0;
//			for (int ky = -1; ky <= 1; ++ky) {
//				for (int kx = -1; kx <= 1; ++kx) {
//					index = (y + ky) * width + (x + kx);
//					laplacianValue += imageData[index] * laplacianKernel[ky + 1][kx + 1];
//				}
//			}
//			laplacianImage[y * width + x] = laplacianValue;
//			sum += laplacianValue;
//			sumSquared += laplacianValue * laplacianValue;
//		}
//	}
//
//	double mean = sum / (width * height);
//	double variance = (sumSquared / (width * height)) - (mean * mean);
//	return variance;
//}
//
//// Updated Getblur function to include normalized blur score calculation
//int Getblur(unsigned char* in_inputImageData, int in_Width, int in_Height, double* out_meanValue, double* out_blurScore) {
//	// Calculate mean
//	double sum = 0;
//	int imageSize = in_Width * in_Height;
//	for (int i = 0; i < imageSize; ++i) {
//		sum += in_inputImageData[i];
//	}
//	*out_meanValue = sum / imageSize;
//
//	// Calculate variance
//	double sumSquaredDiff = 0;
//	for (int i = 0; i < imageSize; ++i) {
//		double diff = in_inputImageData[i] - *out_meanValue;
//		sumSquaredDiff += diff * diff;
//	}
//	double pixelVariance = sumSquaredDiff / imageSize;
//
//	// Calculate Laplacian variance
//	double laplacianVariance = CalculateLaplacianVariance(in_inputImageData, in_Width, in_Height);
//
//	// Normalize Laplacian variance
//	double laplacianVarianceNormalized = laplacianVariance / (in_Width * in_Height); // Normalize to image size or use other scaling
//
//	// Combine using weighted sum
//	double weightPixelVariance = 0.5; // Weight for pixel intensity variance
//	double weightLaplacianVariance = 0.5; // Weight for Laplacian variance
//	*out_blurScore = (weightPixelVariance * pixelVariance) + (weightLaplacianVariance * laplacianVarianceNormalized);
//
//	return 0;
//}

double CalculateTextureFeature(unsigned char* imageData, int width, int height) {
	
	int count = 0;

	for (int y = 1; y < height - 1; ++y) 
	{
		for (int x = 1; x < width - 1; ++x) 
		{
			unsigned char center = imageData[y * width + x];
			unsigned char neighbors[8] = {
				imageData[(y - 1) * width + (x - 1)], imageData[(y - 1) * width + x], imageData[(y - 1) * width + (x + 1)],
				imageData[y * width + (x - 1)], /*center*/ imageData[y * width + (x + 1)],
				imageData[(y + 1) * width + (x - 1)], imageData[(y + 1) * width + x], imageData[(y + 1) * width + (x + 1)]
			};

			unsigned char lbpValue = 0;
			for (int i = 0; i < 8; ++i) {
				lbpValue |= (neighbors[i] >= center) << i;
			}
			textureFeature += lbpValue;
			++count;
		}
	}

	if (count > 0) {
		textureFeature /= count;
	}

	return textureFeature;
}

bool GetSpoofStatus(unsigned char* in_inputImageData, int in_Width, int in_Height, double* out_meanValue, double* out_stddev, double *out_Blur)
{
	bool IsSpoofStatus = false;
	double meanValue = 0.0;
	double blurValue = 0.0;
	double stdValue = 0.0;
	textureFeature = 0;
	
	/*int arr1[4] = { 2, 3, 4, 5 };
	int arr2[3] = { 17, 13, 10 };
	pwmchangecount = arr1[rand() % 4];*/
	imgProperty stcimgeproperty;
	int retvalue = Getblur(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, &meanValue, &blurValue);
	retvalue = GetMeanAndStddev(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, &meanValue, &stdValue);

	
	textureFeature = CalculateTextureFeature(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT);
	
	//const double TEXTURE_THRESHOLD = 20.0;

	
	stcimgeproperty.irValue = prev_pwmValue;
	stcimgeproperty.blur = blurValue;
	stcimgeproperty.mean = meanValue;
	stcimgeproperty.sd = stdValue;
	stcimgeproperty.texture = textureFeature;
	irPropety.push_back(stcimgeproperty);
	

	if ((prev_pwmValue == COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL17) /*&& (blurValue >= 3000 && (stdValue >= 55 && stdValue <= 75) && (meanValue > 180 && meanValue <220))*/)
	{
		skipFrame++;
		highPwm++;
		if (high_meanvalue < irPropety[irPropety.size() - 1].mean)
			high_meanvalue = irPropety[irPropety.size() - 1].mean;
		if (max_high_texture < irPropety[irPropety.size() - 1].texture)
		{
			/*if (min_high_texture == 0)
			{
				min_high_texture = max_high_texture;
			}*/
			max_high_texture = irPropety[irPropety.size() - 1].texture;
		}

		if ((min_high_texture > irPropety[irPropety.size() - 1].texture) || min_high_texture == 0)
			min_high_texture = irPropety[irPropety.size() - 1].texture;


	}


	//if ((prev_pwmValue == COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL13) /*&& (blurValue >= 1700 && (stdValue >= 40 && stdValue <= 65) && (meanValue > 85 && meanValue <150))*/)
	//{
	//	skipFrame++;
	//	midPwm++;
	//	if (mid_meanvalue < irPropety[irPropety.size() - 1].mean)
	//		mid_meanvalue = irPropety[irPropety.size() - 1].mean;
	//}

	if ((prev_pwmValue == COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL11) /*&& (blurValue >= 1700 && (stdValue >= 40 && stdValue <= 65) && (meanValue > 85 && meanValue <150))*/)
	{
		skipFrame++;
		lowPwm++;
		if (low_meanvalue < irPropety[irPropety.size() - 1].mean)
			low_meanvalue = irPropety[irPropety.size() - 1].mean;
		if (max_low_texture < irPropety[irPropety.size() - 1].texture)
		{
			/*if (min_low_texture == 0)
			{
				min_low_texture = max_low_texture;
			}*/
			max_low_texture = irPropety[irPropety.size() - 1].texture;
		}

		if ((min_low_texture > irPropety[irPropety.size() - 1].texture) || min_low_texture == 0)
			min_low_texture = irPropety[irPropety.size() - 1].texture;
	}

	

	

	/*pwmcounter++;
	if (pwmcounter > 7)
	{
		retvalue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL17);
	}
	else*/ 
	if ((skipFrame > 1) && (highPwm >=2) && (!IsSpoofStatus))
	{
		if (max_high_texture - min_high_texture > 10)
		{
			skipFrame = 0;
			FILE* fptr1 = NULL;
			fopen_s(&fptr1, "Vector_Mean.csv", "ab");
			for (int i = 0; i < irPropety.size(); i++)
			{
				fprintf(fptr1, "%s,%d,%d,%d,%d,%lf,%s,%lf,%lf,%lf,%lf\n", emp_id, irPropety[i].irValue, irPropety[i].mean, irPropety[i].sd, irPropety[i].blur, irPropety[i].texture, "timeout", max_high_texture, min_high_texture, max_low_texture, min_low_texture);
			}
			fprintf(fptr1, "\n");
			fclose(fptr1);
		}
		else
		{
			retvalue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL11);
			prev_pwmValue = COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL11;
			//pwmchangecount = arr1[rand() % 4];
			::Sleep(200);
			skipFrame = 0;
			highPwm = 0;
		}

		
	}
	/*if ((skipFrame > 2) && (midPwm >pwmchangecount) && (!IsSpoofStatus))
	{
		retvalue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL10);
		prev_pwmValue = COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL10;
		pwmchangecount = arr1[rand() % 4];
		::Sleep(200);
		skipFrame = 0;
		highPwm = 0;
		midPwm = 0;

	}*/
	
	if ((skipFrame > 1) && (lowPwm >=2))
	{
		//if (((irPropety[0].mean - irPropety[irPropety.size() - 1].mean) >= 40) /*&& ((irPropety[0].sd / 2) > irPropety[irPropety.size() - 1].sd)*/ && ((irPropety[irPropety.size() - 1].blur > 500) && ((irPropety[irPropety.size() - 1].blur < 7000))))
        //if (((high_meanvalue > mid_meanvalue) && (mid_meanvalue > low_meanvalue)) && ((irPropety[0].mean - irPropety[irPropety.size() - 1].mean) >= 40) && ((irPropety[irPropety.size() - 1].blur > 500) && ((irPropety[irPropety.size() - 1].blur < 7000))) && textureFeature<190)
		if (max_low_texture - min_low_texture > 10)
		{
			skipFrame = 0; FILE* fptr1 = NULL;
			fopen_s(&fptr1, "Vector_Mean.csv", "ab");
			for (int i = 0; i < irPropety.size(); i++)
			{
				fprintf(fptr1, "%s,%d,%d,%d,%d,%lf,%s,%lf,%lf,%lf,%lf\n", emp_id, irPropety[i].irValue, irPropety[i].mean, irPropety[i].sd, irPropety[i].blur, irPropety[i].texture, "timeout", max_high_texture, min_high_texture, max_low_texture, min_low_texture);
			}
			fprintf(fptr1, "\n");
			fclose(fptr1);
		}
		else if((irPropety[0].texture - irPropety[irPropety.size() - 1].texture)>30 && ((high_meanvalue > low_meanvalue))/* && ((irPropety[0].mean - irPropety[irPropety.size() - 1].mean) >= 40)*/)
		{
		
 			lowpwmhighmean = 0;
			updown = true;
			retvalue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL17);
			::Sleep(200);
			prev_pwmValue = COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL17;
			highPwm = 0;
			midPwm = 0;
			skipFrame = 0;

			FILE* fptr1 = NULL;
			fopen_s(&fptr1, "Vector_Mean.csv", "ab");
			for (int i = 0; i < irPropety.size(); i++)
			{
				fprintf(fptr1, "%s,%d,%d,%d,%d,%lf,%s,%lf,%lf,%lf,%lf\n", emp_id, irPropety[i].irValue, irPropety[i].mean, irPropety[i].sd, irPropety[i].blur, irPropety[i].texture,"Captured",max_high_texture,min_high_texture,max_low_texture,min_low_texture);
			}
			fprintf(fptr1, "\n");
			fclose(fptr1);
		}
		else
		{	
			lowpwmhighmean++;
		}
	
	}
	if (lowpwmhighmean >= 3 && lowpwmhighmean < 6)
	{
		if (((high_meanvalue > low_meanvalue)) && ((irPropety[0].mean - irPropety[irPropety.size() - 1].mean) >= 40))
		{
			
			lowpwmhighmean = 0;
			updown = true;
			retvalue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL17);
			::Sleep(200);
			prev_pwmValue = COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL17;
			highPwm = 0;
			midPwm = 0;
			skipFrame = 0;

			FILE* fptr1 = NULL;
			fopen_s(&fptr1, "Vector_Mean.csv", "ab");
			for (int i = 0; i < irPropety.size(); i++)
			{
				fprintf(fptr1, "%s,%d,%d,%d,%d,%lf,%s,%lf,%lf,%lf,%lf\n", emp_id, irPropety[i].irValue, irPropety[i].mean, irPropety[i].sd, irPropety[i].blur, irPropety[i].texture, "Captured", max_high_texture, min_high_texture, max_low_texture, min_low_texture);
			}
			fprintf(fptr1, "\n");
			fclose(fptr1);
		}
	}
	if (lowpwmhighmean >= 6)
	{
		
		FILE* fptr1 = NULL;
		fopen_s(&fptr1, "Vector_Mean.csv", "ab");
		for (int i = 0; i < irPropety.size(); i++)
		{
			fprintf(fptr1, "%s,%d,%d,%d,%d,%lf,%s,%lf,%lf,%lf,%lf\n", emp_id, irPropety[i].irValue, irPropety[i].mean, irPropety[i].sd, irPropety[i].blur, irPropety[i].texture, "Not Captured",max_high_texture, min_high_texture, max_low_texture, min_low_texture);
		}
		fprintf(fptr1, "\n");
		fclose(fptr1);
		
	}
	/*if (updown)
		IsSpoofStatus = true;
	*/

	*out_meanValue = meanValue;
	*out_stddev = stdValue;
	*out_Blur = blurValue;
	return IsSpoofStatus;
}

#pragma endregion

#pragma region Thread And Callback Function

DWORD WINAPI StopCapThread(LPVOID lpParam)
{
	int retValue = MARVIS_Auth_StopCapture();

	if (MARVIS_AUTH_E_SUCCESS != retValue)
	{
		IsGetQtyInCapTemp = false;

		(m_CAPTURE_COMPLETE_CALLBACK)(-1, 0, 0);

		/*stringstream s;
		s << "[ImageProcessThread]: CommStopCapture Fail With Error Code : " <<
		retValue;
		LOG_ERROR(s.str());*/
	}
	else
	{
		DeviceErrorCode = 0;
	}
	return 0;
}

void WaitForAutoStopThread()
{
	if (m_hImageProcessThreadHandle)
	{
		switch (WaitForSingleObject(m_hImageProcessThreadHandle, 0))
		{
		case WAIT_OBJECT_0:
		{
							  break;
		}

		case WAIT_TIMEOUT:
		{
							 //TerminateThread(m_hImageProcessThreadHandle, 0);
							 break;
		}

		default:
		{
				   TerminateThread(m_hImageProcessThreadHandle, 0);
				   break;
		}
		}

		CloseHandle(m_hImageProcessThreadHandle);
		m_hImageProcessThreadHandle = NULL;
	}

	if (m_hAutoStopThreadHandle)
	{
		switch (WaitForSingleObject(m_hAutoStopThreadHandle, 0))
		{
		case WAIT_OBJECT_0:
		{
							  break;
		}

		case WAIT_TIMEOUT:
		{
							 //TerminateThread(m_hAutoStopThreadHandle, 0);
							 break;
		}

		default:
		{
				   TerminateThread(m_hAutoStopThreadHandle, 0);
				   break;
		}
		}
		CloseHandle(m_hAutoStopThreadHandle);
		m_hAutoStopThreadHandle = NULL;
	}
	if (m_hCapturePreviewThreadHandle)
	{
		switch (WaitForSingleObject(m_hCapturePreviewThreadHandle, 0))
		{
		case WAIT_OBJECT_0:
		{
							  break;
		}

		case WAIT_TIMEOUT:
		{
							 //TerminateThread(m_hAutoStopThreadHandle, 0);
							 break;
		}

		default:
		{
				   TerminateThread(m_hCapturePreviewThreadHandle, 0);
				   break;
		}
		}
		CloseHandle(m_hCapturePreviewThreadHandle);
		m_hCapturePreviewThreadHandle = NULL;
	}

	if (m_hCaptureCompleteThreadHandle)
	{
		switch (WaitForSingleObject(m_hCaptureCompleteThreadHandle, 0))
		{
		case WAIT_OBJECT_0:
		{
							  break;
		}

		case WAIT_TIMEOUT:
		{
							 //TerminateThread(m_hAutoStopThreadHandle, 0);
							 break;
		}

		default:
		{
				   TerminateThread(m_hCaptureCompleteThreadHandle, 0);
				   break;
		}
		}
		CloseHandle(m_hCaptureCompleteThreadHandle);
		m_hCaptureCompleteThreadHandle = NULL;
	}

	//if (m_hCaptureStopThreadHandle)
	//{
	//	switch (WaitForSingleObject(m_hCaptureStopThreadHandle, 1000))
	//	{
	//	case WAIT_OBJECT_0:
	//	{
	//						  break;
	//	}
	//
	//	case WAIT_TIMEOUT:
	//	{
	//						 //TerminateThread(m_hAutoStopThreadHandle, 0);
	//						 break;
	//	}
	//
	//	default:
	//	{
	//			   TerminateThread(m_hCaptureStopThreadHandle, 0);
	//			   break;
	//	}
	//	}
	//	CloseHandle(m_hCaptureStopThreadHandle);
	//	m_hCaptureStopThreadHandle = NULL;
	//}
}

DWORD WINAPI AutoStopCapThread(LPVOID lpParam)
{
	int retValue = MARVIS_AUTH_E_SUCCESS;
	//while (GetTickCount() - CaptureStartTime <= nTimeOutInMilliSeconds && !IsStopButton && deviceStatus != DEVICE_STREAMING_STOPPED )
	while (GetTickCount() - CapStartTime <= nTimeOutInMilliSeconds && !IsStopButton && deviceStatus == DEVICE_STREAMING_STARTED)
	{
		::Sleep(10);
	}

	if (!IsDeviceInit)
	{
		DeviceErrorCode = MARVIS_AUTH_E_CAPTURE_STOP;
	}
	else if (IsStopButton == false && !IsGetQtyInCap && DeviceErrorCode == MARVIS_AUTH_E_SUCCESS && IsDeviceInit)
	{
		IsTimeOutAutoCap = true;
		//m_hCaptureCompleteThreadHandle = ::CreateThread(NULL, 0, &StopCapThread, 0, 0, NULL);
		//::Sleep(100);
		retValue = MARVIS_Auth_StopCapture();
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			LOG_ERROR("CommStopCapture Fail In AutoStopCapThread ");
		}
		if (!IsAutoCap && IsDeviceInit)
		{
			(m_CAPTURE_COMPLETE_CALLBACK)(MARVIS_AUTH_E_TIMEOUT, 0, 0);
		}
		IsAutoCapComplete = true;
	}

	return retValue;
}

DWORD WINAPI CompleteCapThread(LPVOID lpParam)
{
	int retValue = MARVIS_Auth_StopCapture();

	if (MARVIS_AUTH_E_SUCCESS != retValue)
	{
		IsGetQtyInCapTemp = false;
		(m_CAPTURE_COMPLETE_CALLBACK)(-1, 0, 0);
		/*stringstream s;
		s << "[ImageProcessThread]: CommStopCapture Fail With Error Code : " <<
		retValue;
		LOG_ERROR(s.str());*/
	}
	else
	{
		if (m_CAPTURE_COMPLETE_CALLBACK)
		{
			(m_CAPTURE_COMPLETE_CALLBACK)(0, &g_ipl_info[0].pCompleteFrame.pQtyInfo, &g_ipl_info[0].pCompleteFrame.pBitmap);
		}
	}
	return 0;
}

DWORD WINAPI CapturePreviewThread(LPVOID lpParam)
{
	if (m_PREVIEW_CALLBACK)
	{
		(m_PREVIEW_CALLBACK)(gPreviewErrorCode, &g_ipl_info[0].pQtyInfo, &g_ipl_info[0].pBitmapImage);
	}
	return 0;
}

void ImagePreviewCallback(void *pParam)
{
	//We have got callback from worker thread.
	//We should NEVER update GUI directly from Worker Thread.
	//So, we will preserve these parameters and post them to GUI thread to take relevant action.
	COMM_IMAGE_PREVIEW_PARAMS *pImagePreviewParams = reinterpret_cast<COMM_IMAGE_PREVIEW_PARAMS*> (pParam);

	if (pImagePreviewParams)
	{
		if (pImagePreviewParams->pCapturedImage)
		{
			if (isDataBufferFilled == false)
			{
				std::memset(g_ipl_info[0].RawImageBuffer, 0, g_ipl_info[0].FULL_FRAME_WIDTH * g_ipl_info[0].FULL_FRAME_HEIGHT);
				std::memcpy(g_ipl_info[0].RawImageBuffer, pImagePreviewParams->pCapturedImage, g_ipl_info[0].FULL_FRAME_WIDTH * g_ipl_info[0].FULL_FRAME_HEIGHT);

				g_ipl_info[0].RawImageLen = pImagePreviewParams->nImageBytes;

				isDataBufferFilled = true;
				//m_hImageProcessThreadHandle = ::CreateThread(NULL, 0, &ImageProcessThreadFull, 0, 0, NULL);

				/*unsigned char filePathname[2048] = { '\0' };
				sprintf((char*)&filePathname[0], "%d_fullFrame.raw", counter++);
				WriteAllText1((const char*)filePathname, (char*)g_ipl_info[0].RawImageBuffer, g_ipl_info[0].FULL_FRAME_WIDTH * g_ipl_info[0].FULL_FRAME_HEIGHT);
*/

				ImageProcessThreadFull(NULL);
			}
		}
	}
}

DWORD WINAPI ImageProcessThreadFull(LPVOID lpParam)
{
	
	bool IsSimilarity = false;
	int ErrorCode = 0;
	int retValue = MARVIS_AUTH_E_SUCCESS;
	bool IsSimilarFrame = false;
	try
	{
		//if (IsDeviceInit && isDataBufferFilled && g_ipl_info[0].RawImageLen == g_ipl_info[0].FULL_FRAME_WIDTH *g_ipl_info[0].FULL_FRAME_HEIGHT && ((g_ipl_info[0].RawImageBuffer != NULL && (g_ipl_info[0].RawImageBuffer[0] != '\0'))) && deviceStatus != DEVICE_STREAMING_STOPPED)
		if (IsDeviceInit && isDataBufferFilled && deviceStatus != DEVICE_STREAMING_STOPPED)
		{
			//CommSetVISLed(0);
			m_NoOfFrameCount++;
			gPreviewErrorCode = 0;
#ifdef FPS_DEVELOPEMENT_MODE
			unsigned long capStart = GetTickCount();

			m_NoOfFrameCount++;

			//iplReadImage(g_ipl_info[0].RawImageBuffer, "IRIS.bmp", g_ipl_info[0].FULL_FRAME_WIDTH, g_ipl_info[0].FULL_FRAME_HEIGHT, 0);
			//unsigned char *rt = rotate180(g_ipl_info[0].RawImageBuffer, g_ipl_info[0].FULL_FRAME_WIDTH, g_ipl_info[0].FULL_FRAME_HEIGHT);
#endif

			retValue = iplProcessRawImage(g_ipl_info[0].RawImageBuffer, g_ipl_info[0].Final_Frame, g_ipl_info[0].FULL_FRAME_WIDTH, g_ipl_info[0].FULL_FRAME_HEIGHT,
				g_ipl_info[0].CROP_WIDTH, g_ipl_info[0].CROP_HEIGHT, g_ipl_info[0].CROP_X_COORDINATE, g_ipl_info[0].CROP_Y_COORDINATE,
				g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, g_ipl_info[0].Invert, g_ipl_info[0].RotateFinalImage);

			/*unsigned char filePathname[2048] = { '\0' };
			sprintf((char*)&filePathname[0], "%d_FUlllFrame_%d.raw", filecounter++,tempcount);
			WriteAllText1((const char*)filePathname, (char*)g_ipl_info[0].RawImageBuffer, g_ipl_info[0].FULL_FRAME_WIDTH*g_ipl_info[0].FULL_FRAME_HEIGHT);
*/
			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				isDataBufferFilled = false;
				/*stringstream s;
				s << "[ImageProcessThread]: iplProcessRawImage Fail With Error Code : " << retValue;
				LOG_ERROR(s.str());*/
				return ErrorCode;
			}

			retValue = MEngin_GetImageSimilarityScore(g_ipl_info[0].Final_Frame, Prev_Frame_Preview, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, 20, m_gSimilarityScore[m_NoOfFrameCount % 2]);
			if (retValue != 0)
			{
				/*_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
				MARVIS_AUTH_E_ALREADY_START_STOP, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_ALREADY_START_STOP));
				LOG_ERROR(szOutputString);*/
				return -1;
			}
			else
			{
				if (m_gSimilarityScore[0] > gSimilarityThreshold && m_gSimilarityScore[1] > gSimilarityThreshold /*&& m_gSimilarityScore[2] > gSimilarityThreshold*/)
				{
					IsSimilarity = true;
				}
				else
				{
					IsSimilarity = false;
				}

			}


			long len1 = 0;
			//ReadFile1("RAW.raw", g_ipl_info[0].Final_Frame, &len1);
			retValue = iplIRISGetEyeInfo(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, g_ipl_info[0].DPI, 4, 1, pEyeInfo);

			g_ipl_info[0].pQtyInfo.Quality = pEyeInfo.iris_quality;
			if (g_ipl_info[0].pQtyInfo.Quality > 0)
			{
				g_ipl_info[0].pQtyInfo.X = pEyeInfo.pupil_center_x;
				g_ipl_info[0].pQtyInfo.Y = pEyeInfo.pupil_center_y;
				g_ipl_info[0].pQtyInfo.R = (pEyeInfo.iris_diameter / 2);
				//g_ipl_info[0].pQtyInfo.R = (pEyeInfo.pupil_diameter / 2);
			}
			else
			{
				g_ipl_info[0].pQtyInfo.X = 0;
				g_ipl_info[0].pQtyInfo.Y = 0;
				g_ipl_info[0].pQtyInfo.R = 0;
			}

			double mean = 0;
			double stddev = 0;
			double blur = 0;

			if (prev_pwmValue == COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL5)
			{
				IsSimilarity = true;
				g_ipl_info[0].pQtyInfo.Quality = 10;
			}

			if (updown)
				ISComplete = true;

			if (IsSimilarity && (g_ipl_info[0].pQtyInfo.Quality > 0) && !updown && lowpwmhighmean < 6)
			{
				ISComplete = GetSpoofStatus(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, &mean, &stddev, &blur);
			}
			else
			{
				skipFrame = 0;
			}
			
			//unsigned char* out_data = (unsigned char*)malloc(g_ipl_info[0].FINAL_FRAME_WIDTH* g_ipl_info[0].FINAL_FRAME_HEIGHT);
			//int black_count = 0;
		
				
				//int white_count = Circledata(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT,
					//g_ipl_info[0].pQtyInfo.X, g_ipl_info[0].pQtyInfo.Y, g_ipl_info[0].pQtyInfo.R, out_data, &black_count);
				//g_ipl_info[0].pQtyInfo.R = (pEyeInfo.pupil_diameter / 2);

			if (lowpwmhighmean >= 6)
			{
				//g_ipl_info[0].pQtyInfo.Quality = 0;
				m_hCaptureCompleteThreadHandle = ::CreateThread(NULL, 0, &StopCapThread, 0, 0, NULL);
				IsGetQtyInCap = false;
				DeviceErrorCode = MARVIS_AUTH_E_FAKE_IRISFOUND;
				(m_CAPTURE_COMPLETE_CALLBACK)(DeviceErrorCode, 0, 0);
				IsAutoCapComplete = true;
				gCounter = 0;
				isDataBufferFilled = false;
				return DeviceErrorCode;
			}

			g_ipl_info[0].pQtyInfo.percent_occlusion = /*pEyeInfo.percent_occlusion;*/m_NoOfFrameCount;
			g_ipl_info[0].pQtyInfo.iris_mean = pEyeInfo.iris_mean;
			g_ipl_info[0].pQtyInfo.pupil_mean = pEyeInfo.pupil_mean;
			g_ipl_info[0].pQtyInfo.pupil_blur = blur;
			g_ipl_info[0].pQtyInfo.iris_diameter = stddev;// white_count;
			g_ipl_info[0].pQtyInfo.pupil_diameter = mean;// black_count;
			g_ipl_info[0].pQtyInfo.sharpness = pEyeInfo.sharpness;

#ifdef PWM_FW
			if (pEyeInfo.iris_quality > MinQtyLedOn && IsIrisLedOn == false)
			{
				MinQtyCounter = 0;
				CommSetVISLed(1);
				IsIrisLedOn = true;
			}
			else if (pEyeInfo.iris_quality < MinQtyLedOn && IsIrisLedOn == true)
			{
				MinQtyCounter++;

				if (MinQtyCounter > 10)
				{
					CommSetVISLed(0);
					MinQtyCounter = 0;
					IsIrisLedOn = false;
				}
			}
#endif 

#ifdef PWM_FW
			MARVIS_Enroll_SetIRLed(pEyeInfo.iris_mean);
#endif 

#ifdef FPS_DEVELOPEMENT_MODE
			++m_FrameCount;

			float calFPS = 1000.0 / ((GetTickCount() - CapStartTime) / m_FrameCount);

			sprintf(g_ipl_info[0].pQtyInfo.CaptureTime, "%d, %ld, %.2f", m_FrameCount, (GetTickCount() - CapStartTime), calFPS);
#endif
			memset(g_ipl_info[0].pBitmapImage.m_BitmapImage, 255, g_ipl_info[0].nImageLengthWithBitmapHeader);
				
			retValue = iplConvertRawToBmp(g_ipl_info[0].Final_Frame, g_ipl_info[0].pBitmapImage.m_BitmapImage, g_ipl_info[0].FINAL_FRAME_HEIGHT, g_ipl_info[0].FINAL_FRAME_WIDTH, 0);
	
			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				isDataBufferFilled = false;
				/*stringstream s;
				s << "[ImageProcessThread]: iplConvertRawToBmp Fail With Error Code : " << retValue;
				LOG_ERROR(s.str());*/
				return ErrorCode;
			}



			g_ipl_info[0].pBitmapImage.m_BitmapImageLen = g_ipl_info[0].nImageLengthWithBitmapHeader;

			/////////////////////////////////////////////
			FILE* fp = NULL;
			fp = fopen("frames.raw", "ab");

			if (fp == NULL) 
			{
				std::cerr << "Error opening file: " << std::endl;
				return 1; // Exit with error
			}
			fwrite(g_ipl_info[0].Final_Frame, (g_ipl_info[0].FINAL_FRAME_HEIGHT*g_ipl_info[0].FINAL_FRAME_WIDTH), 1, fp);
			fclose(fp);
			
			///////////////////////////////////////////////////


			if (IsGetQtyInCapTemp == false && g_ipl_info[0].pBitmapImage.m_BitmapImage != NULL && deviceStatus != DEVICE_STREAMING_STOPPED)
			{
				//if (!IsPreviewHide)
					m_hCapturePreviewThreadHandle = ::CreateThread(NULL, 0, &CapturePreviewThread, 0, 0, NULL);
				//else
				//{
				//	if ((counter++) >= 2)
				//	{
				//		IsPreviewHide = false;
				//		//ISComplete = true;
				//		counter = 0;
				//	}
				//}

					
			}

			if (gCounter <= nMinCapRotation  && IsGetQtyInCapTemp == false
				/*&& IsSimilarFrame */&& ISComplete)
			{

#ifdef PWM_FW
				if (pEyeInfo.iris_quality >= nReqQty && pEyeInfo.iris_mean >= MIN_PUPIL_MEAN && pEyeInfo.iris_mean <= MAX_PUPIL_MEAN) //if (QualityL >= nReqQty)				
#else
				if (pEyeInfo.iris_quality >= nReqQty)
#endif
				{
					// Here we have to stop the capture and store the image
					if (g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality <= pEyeInfo.iris_quality)
					{
						std::memcpy(g_ipl_info[0].pCompleteFrame.Final_Frame, g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH * g_ipl_info[0].FINAL_FRAME_HEIGHT);

						std::memcpy(g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImage, g_ipl_info[0].pBitmapImage.m_BitmapImage, g_ipl_info[0].nImageLengthWithBitmapHeader);
						g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImageLen = g_ipl_info[0].nImageLengthWithBitmapHeader;

						g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality = pEyeInfo.iris_quality;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.X = pEyeInfo.iris_center_x;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.Y = pEyeInfo.iris_center_y;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.R = (pEyeInfo.iris_diameter / 2);

					}

					gCounter += 10; //Only check with 2 frame

				}
#ifdef PWM_FW
				else if ((pEyeInfo.iris_quality >= nMinQty && pEyeInfo.iris_quality <= nReqQty) &&
					pEyeInfo.iris_mean >= MIN_PUPIL_MEAN && pEyeInfo.iris_mean <= MAX_PUPIL_MEAN) //else if ((QualityL >= nMinQty && QualityL <= nReqQty))//
#else
				else if ((pEyeInfo.iris_quality >= nMinQty && pEyeInfo.iris_quality <= nReqQty))
#endif

				{
					// Here we will take 5 images and compare which is better then sends the best one
					// to user.

					if (g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality <= pEyeInfo.iris_quality)
					{
						std::memcpy(g_ipl_info[0].pCompleteFrame.Final_Frame, g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH * g_ipl_info[0].FINAL_FRAME_HEIGHT);

						std::memcpy(g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImage, g_ipl_info[0].pBitmapImage.m_BitmapImage, g_ipl_info[0].nImageLengthWithBitmapHeader);
						g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImageLen = g_ipl_info[0].nImageLengthWithBitmapHeader;

						g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality = pEyeInfo.iris_quality;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.X = pEyeInfo.iris_center_x;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.Y = pEyeInfo.iris_center_y;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.R = (pEyeInfo.iris_diameter / 2);

					}

					gCounter += 1;
				}
			}


				//if (nMinQty <= g_ipl_info[0].pQtyInfo.Quality && !IsGetQtyInCap  &&  g_ipl_info[0].pBitmapImage.m_BitmapImage != NULL &&  deviceStatus != DEVICE_STREAMING_STOPPED)
			if (gCounter >= nMinCapRotation && !IsGetQtyInCap &&  g_ipl_info[0].pBitmapImage.m_BitmapImage != NULL && deviceStatus != DEVICE_STREAMING_STOPPED)
			{
				if (ISComplete)
				{
					/*if (GetFinalSpoofResult((char *)g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT))
					{*/

					IsGetQtyInCap = true;

					/*retValue = MARVIS_Auth_StopCapture();
					if (MARVIS_AUTH_E_SUCCESS != retValue)
					{
					stringstream s;
					s << "[ImageProcessThread]: CommStopCapture Fail With Error Code : " <<
					retValue;
					LOG_ERROR(s.str());
					}*/
					IsGetQtyInCapTemp = true;
					if (!IsAutoCap && IsDeviceInit && m_CAPTURE_COMPLETE_CALLBACK)
					{
						//(m_CAPTURE_COMPLETE_CALLBACK)(ErrorCode, &g_ipl_info[0].pQtyInfo, &g_ipl_info[0].pBitmapImage);
						//(m_CAPTURE_COMPLETE_CALLBACK)(ErrorCode, &g_completeFrame.pQtyInfo, &g_completeFrame.pBitmap);

						//CompleteCapTime = GetTickCount();
						//sprintf(g_ipl_info[0].pCompleteFrame.pQtyInfo.CaptureTime, "%ld",(GetTickCount() - CaptureStartTime));

#ifdef FPS_DEVELOPEMENT_MODE
						float calFPS = 1000.0 / ((GetTickCount() - CapStartTime) / m_FrameCount);

						sprintf(g_ipl_info[0].pCompleteFrame.pQtyInfo.CaptureTime, "%d, %ld, %.2f", m_FrameCount, (GetTickCount() - CapStartTime), calFPS);
#endif

						m_hCaptureCompleteThreadHandle = ::CreateThread(NULL, 0, &CompleteCapThread, 0, 0, NULL);
					}
					else
					{
						m_hCaptureCompleteThreadHandle = ::CreateThread(NULL, 0, &StopCapThread, 0, 0, NULL);


						std::memcpy(g_ipl_info[0].pBitmapImage.m_BitmapImage, g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImage, g_ipl_info[0].nImageLengthWithBitmapHeader);
						g_ipl_info[0].pBitmapImage.m_BitmapImageLen = g_ipl_info[0].nImageLengthWithBitmapHeader;

						g_ipl_info[0].pQtyInfo.Quality = g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality;
						g_ipl_info[0].pQtyInfo.X = g_ipl_info[0].pCompleteFrame.pQtyInfo.X;
						g_ipl_info[0].pQtyInfo.Y = g_ipl_info[0].pCompleteFrame.pQtyInfo.Y;
						g_ipl_info[0].pQtyInfo.R = g_ipl_info[0].pCompleteFrame.pQtyInfo.R;


						m_hCapturePreviewThreadHandle = ::CreateThread(NULL, 0, &CapturePreviewThread, 0, 0, NULL);
					}
					IsGetQtyInCap = false;
					DeviceErrorCode = 0;
					IsAutoCapComplete = true;
					gCounter = 0;
						
				}
				else
				{
					gCounter = gCounter >= 10 ? 0 : gCounter--;
					g_ipl_info[0].pQtyInfo.Quality = 0;
				}
			}
				//}
			
			isDataBufferFilled = false;
		}
	}
	catch (const std::exception &/*ex */)
	{
		isDataBufferFilled = false;
		return ErrorCode;
	}
	catch (...)
	{
		isDataBufferFilled = false;
		return ErrorCode;
	}
	return ErrorCode;
}

DWORD WINAPI ImageProcessThreadFull1(LPVOID lpParam)
{

	bool IsSimilarity = false;
	int ErrorCode = 0;
	int retValue = MARVIS_AUTH_E_SUCCESS;
	bool IsSimilarFrame = false;
	int filecounter = 0;
	try
	{
		//if (IsDeviceInit && isDataBufferFilled && g_ipl_info[0].RawImageLen == g_ipl_info[0].FULL_FRAME_WIDTH *g_ipl_info[0].FULL_FRAME_HEIGHT && ((g_ipl_info[0].RawImageBuffer != NULL && (g_ipl_info[0].RawImageBuffer[0] != '\0'))) && deviceStatus != DEVICE_STREAMING_STOPPED)
		if (IsDeviceInit && isDataBufferFilled && deviceStatus != DEVICE_STREAMING_STOPPED)
		{
			//CommSetVISLed(0);
			m_NoOfFrameCount++;
			gPreviewErrorCode = 0;
#ifdef FPS_DEVELOPEMENT_MODE
			unsigned long capStart = GetTickCount();

			m_NoOfFrameCount++;

			//iplReadImage(g_ipl_info[0].RawImageBuffer, "IRIS.bmp", g_ipl_info[0].FULL_FRAME_WIDTH, g_ipl_info[0].FULL_FRAME_HEIGHT, 0);
			//unsigned char *rt = rotate180(g_ipl_info[0].RawImageBuffer, g_ipl_info[0].FULL_FRAME_WIDTH, g_ipl_info[0].FULL_FRAME_HEIGHT);
#endif

			retValue = iplProcessRawImage(g_ipl_info[0].RawImageBuffer, g_ipl_info[0].Final_Frame, g_ipl_info[0].FULL_FRAME_WIDTH, g_ipl_info[0].FULL_FRAME_HEIGHT,
				g_ipl_info[0].CROP_WIDTH, g_ipl_info[0].CROP_HEIGHT, g_ipl_info[0].CROP_X_COORDINATE, g_ipl_info[0].CROP_Y_COORDINATE,
				g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, g_ipl_info[0].Invert, g_ipl_info[0].RotateFinalImage);

			/*unsigned char filePathname[2048] = { '\0' };
			sprintf((char*)&filePathname[0], "%d_FUlllFrame_%d.raw", filecounter++,tempcount);
			WriteAllText1((const char*)filePathname, (char*)g_ipl_info[0].RawImageBuffer, g_ipl_info[0].FULL_FRAME_WIDTH*g_ipl_info[0].FULL_FRAME_HEIGHT);
			*/
			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				isDataBufferFilled = false;
				/*stringstream s;
				s << "[ImageProcessThread]: iplProcessRawImage Fail With Error Code : " << retValue;
				LOG_ERROR(s.str());*/
				return ErrorCode;
			}

			retValue = MEngin_GetImageSimilarityScore(g_ipl_info[0].Final_Frame, Prev_Frame_Preview, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, 20, m_gSimilarityScore[m_NoOfFrameCount % 2]);
			if (retValue != 0)
			{
				/*_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
				MARVIS_AUTH_E_ALREADY_START_STOP, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_ALREADY_START_STOP));
				LOG_ERROR(szOutputString);*/
				return -1;
			}
			else
			{
				if (m_gSimilarityScore[0] > gSimilarityThreshold && m_gSimilarityScore[1] > gSimilarityThreshold)
				{
					IsSimilarity = true;
				}
				else
				{
					IsSimilarity = false;
				}

			}


			long len1 = 0;
			//ReadFile1("RAW.raw", g_ipl_info[0].Final_Frame, &len1);
			retValue = iplIRISGetEyeInfo(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, g_ipl_info[0].DPI, 4, 1, pEyeInfo);

			g_ipl_info[0].pQtyInfo.Quality = pEyeInfo.iris_quality;
			if (g_ipl_info[0].pQtyInfo.Quality > 0)
			{
				g_ipl_info[0].pQtyInfo.X = pEyeInfo.pupil_center_x;
				g_ipl_info[0].pQtyInfo.Y = pEyeInfo.pupil_center_y;
				g_ipl_info[0].pQtyInfo.R = (pEyeInfo.pupil_diameter / 2);
			}
			else
			{
				g_ipl_info[0].pQtyInfo.X = 0;
				g_ipl_info[0].pQtyInfo.Y = 0;
				g_ipl_info[0].pQtyInfo.R = 0;
			}



			int outlen = (pEyeInfo.iris_diameter);
			unsigned char* out_data = (unsigned char*)malloc(g_ipl_info[0].FINAL_FRAME_WIDTH* g_ipl_info[0].FINAL_FRAME_HEIGHT);

			double mean;
			double blur_value = 0;

			//float mean_Final = getMean_FromImage(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT);
			int retvalue = Getblur(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, &mean, &blur_value);

			// get mean and std
			double meanValue = 0.0;
			double stddevvalue = 0.0;
			retvalue = GetMeanAndStddev(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, &meanValue, &stddevvalue);
			//if (IsSimilarity)
			if (IRLed_On)
			{

				//if (abs(pre_mean - mean) > 50)
				if ((stddevvalue < 2) && (mean<2))
				{
					int b = 10;
					IRLed_On = true;

					IsPreviewHide = true;
					//isDataBufferFilled = false;
					//return 0;
					//continue capture
				}
				else
				{
					//filecounter++;
					//IsSimilarity = false;
					if (filecounter > 5)
					{
						ISComplete = false;
						/*m_hCaptureCompleteThreadHandle = ::CreateThread(NULL, 0, &StopCapThread, 0, 0, NULL);
						IsGetQtyInCap = false;
						DeviceErrorCode = MARVIS_AUTH_E_FAKE_IRISFOUND;
						(m_CAPTURE_COMPLETE_CALLBACK)(DeviceErrorCode, 0, 0);
						IsAutoCapComplete = true;
						gCounter = 0;
						isDataBufferFilled = false;
						return DeviceErrorCode;*/
					}
					//filecounter++;
				}
			}
			if (!IRLed_Status)
				endtime = GetTickCount();

			double duration = endtime - starttime;
			if (duration > 10000)
			{
				IRLed_Status = true;
				endtime = 0;
			}
			if (IsSimilarity && (g_ipl_info[0].pQtyInfo.Quality > 100)){
				if (IRLed_Status)
				{
					pre_mean = mean;
					prev_std = stddevvalue;
					retValue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL5);
					//::Sleep(200);
					starttime = GetTickCount();
					IRLed_Status = false;
					isDataBufferFilled = false;
					::Sleep(5000);
					//IsPreviewHide = true;
					//hideframecounter++;
					IRLed_On = true;
					IRLed_First = true;
					//counter++;
					tempcount++;
					filecounter = 0;
					return -1;
				}
			}

			if (IRLed_On && IRLed_First)
			{

				IRLed_First = false;
				retValue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL15);
				//IRLed_On = false;
				isDataBufferFilled = false;
				return -1;
			}


			int black_count = 0;
			int white_count = Circledata(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT,
			g_ipl_info[0].pQtyInfo.X, g_ipl_info[0].pQtyInfo.Y, g_ipl_info[0].pQtyInfo.R, out_data, &black_count);


			/*if ((white_count > 300) || ((black_count < 2000) && (pEyeInfo.pupil_mean <= 30)) || ((black_count > 500) && (pEyeInfo.pupil_mean > 30)) || !IsSimilarity || (pEyeInfo.pupil_mean <= 10))
			{
			g_ipl_info[0].pQtyInfo.Quality = 0;
			}*/


			/*if ((pEyeInfo.pupil_mean <= 30) && (mean >= 180))
			g_ipl_info[0].pQtyInfo.Quality = 0;*/

			//if (mean < 100)   //Condition 1
			//{
			//	g_ipl_info[0].pQtyInfo.Quality = 0;
			//}
			//if (blur_value > 9000)
			//	g_ipl_info[0].pQtyInfo.Quality = 0;

			//if ((mean >= 150 && mean  < 180) && (blur_value > 6800))
			//{
			//	g_ipl_info[0].pQtyInfo.Quality = 0;
			//}
			//else if ((mean >= 180 && mean <= 200) && (blur_value > 5800))
			//{
			//	g_ipl_info[0].pQtyInfo.Quality = 0;
			//}
			//else if ((mean > 200 && mean <= 210) && (blur_value > 4800))
			//{
			//	g_ipl_info[0].pQtyInfo.Quality = 0;
			//}
			//else if ((mean >= 210 /*&& mean <= 210*/) && (blur_value > 4300))
			//{
			//	g_ipl_info[0].pQtyInfo.Quality = 0;
			//}
			//
			g_ipl_info[0].pQtyInfo.percent_occlusion = pEyeInfo.percent_occlusion;//m_gSimilarityScore[m_NoOfFrameCount % 2];
			g_ipl_info[0].pQtyInfo.iris_mean = pEyeInfo.iris_mean;
			g_ipl_info[0].pQtyInfo.pupil_mean = pEyeInfo.pupil_mean;
			g_ipl_info[0].pQtyInfo.pupil_blur = blur_value;
			g_ipl_info[0].pQtyInfo.iris_diameter = stddevvalue;// white_count;
			g_ipl_info[0].pQtyInfo.pupil_diameter = mean;// black_count;
			g_ipl_info[0].pQtyInfo.sharpness = pEyeInfo.sharpness;
			white_count_FULL = 0;
			black_count_FULL = 0;
			if (g_ipl_info[0].pQtyInfo.Quality > 0 && (g_ipl_info[0].pQtyInfo.sharpness < 65))
			{

				IsSimilarFrame = true;

				//pEyeInfo.pupil_diameter = blur_value;

				//if (First == 0){
				//	maximum = pEyeInfo.pupil_diameter;
				//	//minimum = pEyeInfo.pupil_diameter;
				//	//prev_blur_value = blur_value;
				//	First = 1;
				//}
				//else
				//{
				//	if (pEyeInfo.pupil_diameter > maximum)
				//	{
				//		maximum = pEyeInfo.pupil_diameter;
				//	}
				//	else if (pEyeInfo.pupil_diameter < minimum || minimum == 0)
				//	{
				//		minimum = pEyeInfo.pupil_diameter;
				//	}
				//	if ((maximum != 0) && (minimum != 0) && g_ipl_info[0].pQtyInfo.sharpness < 65 /*&& PupilDiaMeanDiff*//* && (abs(mean_box2 - mean_box1) > 30)*/)
				//	{
				//		g_ipl_info[0].pQtyInfo.sclera_mean = maximum - minimum;//abs(prev_blur_value - blur_value);
				//		if ((maximum - minimum) >= 2)
				//		{
				//			IsSimilarFrame = true;
				//		}
				//		else
				//			IsSimilarFrame = false;
				//		
				//	}
				//	//prev_blur_value = blur_value;
				//	g_ipl_info[0].pQtyInfo.sclera_mean = maximum - minimum;
				//}
			}
			else
			{
				IsSimilarFrame = false;
				g_ipl_info[0].pQtyInfo.sclera_mean = 0;
				maximum = 0;
				minimum = 0;
				//prev_blur_value = blur_value;
				First = 0;
				////double mean;
				////int blur_value = 0;
				////int retvalue = Getblur(out_data, pEyeInfo.iris_diameter, pEyeInfo.iris_diameter, &mean, &blur_value);
				//int retvalue = Getblur(g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, &mean, &blur_value);
				//
				//g_ipl_info[0].pQtyInfo.sclera_mean = black_count;


			}

			//free(out_data);


#ifdef PWM_FW
			if (pEyeInfo.iris_quality > MinQtyLedOn && IsIrisLedOn == false)
			{
				MinQtyCounter = 0;
				CommSetVISLed(1);
				IsIrisLedOn = true;
			}
			else if (pEyeInfo.iris_quality < MinQtyLedOn && IsIrisLedOn == true)
			{
				MinQtyCounter++;

				if (MinQtyCounter > 10)
				{
					CommSetVISLed(0);
					MinQtyCounter = 0;
					IsIrisLedOn = false;
				}
			}
#endif 

#ifdef PWM_FW
			MARVIS_Enroll_SetIRLed(pEyeInfo.iris_mean);
#endif 

#ifdef FPS_DEVELOPEMENT_MODE
			++m_FrameCount;

			float calFPS = 1000.0 / ((GetTickCount() - CapStartTime) / m_FrameCount);

			sprintf(g_ipl_info[0].pQtyInfo.CaptureTime, "%d, %ld, %.2f", m_FrameCount, (GetTickCount() - CapStartTime), calFPS);
#endif
			memset(g_ipl_info[0].pBitmapImage.m_BitmapImage, 255, g_ipl_info[0].nImageLengthWithBitmapHeader);

			retValue = iplConvertRawToBmp(g_ipl_info[0].Final_Frame, g_ipl_info[0].pBitmapImage.m_BitmapImage, g_ipl_info[0].FINAL_FRAME_HEIGHT, g_ipl_info[0].FINAL_FRAME_WIDTH, 0);

			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				isDataBufferFilled = false;
				/*stringstream s;
				s << "[ImageProcessThread]: iplConvertRawToBmp Fail With Error Code : " << retValue;
				LOG_ERROR(s.str());*/
				return ErrorCode;
			}



			g_ipl_info[0].pBitmapImage.m_BitmapImageLen = g_ipl_info[0].nImageLengthWithBitmapHeader;
			if (IsGetQtyInCapTemp == false && g_ipl_info[0].pBitmapImage.m_BitmapImage != NULL && deviceStatus != DEVICE_STREAMING_STOPPED)
			{
				if (!IsPreviewHide)
					m_hCapturePreviewThreadHandle = ::CreateThread(NULL, 0, &CapturePreviewThread, 0, 0, NULL);
				else
				{
					if ((counter++) >= 2)
					{
						IsPreviewHide = false;
						//ISComplete = true;
						counter = 0;
					}
				}


			}

			if (gCounter <= nMinCapRotation  && IsGetQtyInCapTemp == false
				&& IsSimilarFrame && ISComplete)
			{

#ifdef PWM_FW
				if (pEyeInfo.iris_quality >= nReqQty && pEyeInfo.iris_mean >= MIN_PUPIL_MEAN && pEyeInfo.iris_mean <= MAX_PUPIL_MEAN) //if (QualityL >= nReqQty)				
#else
				if (pEyeInfo.iris_quality >= nReqQty)
#endif
				{
					// Here we have to stop the capture and store the image
					if (g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality <= pEyeInfo.iris_quality)
					{
						std::memcpy(g_ipl_info[0].pCompleteFrame.Final_Frame, g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH * g_ipl_info[0].FINAL_FRAME_HEIGHT);

						std::memcpy(g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImage, g_ipl_info[0].pBitmapImage.m_BitmapImage, g_ipl_info[0].nImageLengthWithBitmapHeader);
						g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImageLen = g_ipl_info[0].nImageLengthWithBitmapHeader;

						g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality = pEyeInfo.iris_quality;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.X = pEyeInfo.iris_center_x;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.Y = pEyeInfo.iris_center_y;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.R = (pEyeInfo.iris_diameter / 2);

					}

					gCounter += 10; //Only check with 2 frame

				}
#ifdef PWM_FW
				else if ((pEyeInfo.iris_quality >= nMinQty && pEyeInfo.iris_quality <= nReqQty) &&
					pEyeInfo.iris_mean >= MIN_PUPIL_MEAN && pEyeInfo.iris_mean <= MAX_PUPIL_MEAN) //else if ((QualityL >= nMinQty && QualityL <= nReqQty))//
#else
				else if ((pEyeInfo.iris_quality >= nMinQty && pEyeInfo.iris_quality <= nReqQty))
#endif

				{
					// Here we will take 5 images and compare which is better then sends the best one
					// to user.

					if (g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality <= pEyeInfo.iris_quality)
					{
						std::memcpy(g_ipl_info[0].pCompleteFrame.Final_Frame, g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH * g_ipl_info[0].FINAL_FRAME_HEIGHT);

						std::memcpy(g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImage, g_ipl_info[0].pBitmapImage.m_BitmapImage, g_ipl_info[0].nImageLengthWithBitmapHeader);
						g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImageLen = g_ipl_info[0].nImageLengthWithBitmapHeader;

						g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality = pEyeInfo.iris_quality;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.X = pEyeInfo.iris_center_x;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.Y = pEyeInfo.iris_center_y;
						g_ipl_info[0].pCompleteFrame.pQtyInfo.R = (pEyeInfo.iris_diameter / 2);

					}

					gCounter += 1;
				}
			}


			//if (nMinQty <= g_ipl_info[0].pQtyInfo.Quality && !IsGetQtyInCap  &&  g_ipl_info[0].pBitmapImage.m_BitmapImage != NULL &&  deviceStatus != DEVICE_STREAMING_STOPPED)
			if (gCounter >= nMinCapRotation && !IsGetQtyInCap &&  g_ipl_info[0].pBitmapImage.m_BitmapImage != NULL && deviceStatus != DEVICE_STREAMING_STOPPED)
			{
				if (IsSimilarFrame)
				{
					/*if (GetFinalSpoofResult((char *)g_ipl_info[0].Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT))
					{*/

					IsGetQtyInCap = true;

					/*retValue = MARVIS_Auth_StopCapture();
					if (MARVIS_AUTH_E_SUCCESS != retValue)
					{
					stringstream s;
					s << "[ImageProcessThread]: CommStopCapture Fail With Error Code : " <<
					retValue;
					LOG_ERROR(s.str());
					}*/
					IsGetQtyInCapTemp = true;
					if (!IsAutoCap && IsDeviceInit && m_CAPTURE_COMPLETE_CALLBACK)
					{
						//(m_CAPTURE_COMPLETE_CALLBACK)(ErrorCode, &g_ipl_info[0].pQtyInfo, &g_ipl_info[0].pBitmapImage);
						//(m_CAPTURE_COMPLETE_CALLBACK)(ErrorCode, &g_completeFrame.pQtyInfo, &g_completeFrame.pBitmap);

						//CompleteCapTime = GetTickCount();
						//sprintf(g_ipl_info[0].pCompleteFrame.pQtyInfo.CaptureTime, "%ld",(GetTickCount() - CaptureStartTime));

#ifdef FPS_DEVELOPEMENT_MODE
						float calFPS = 1000.0 / ((GetTickCount() - CapStartTime) / m_FrameCount);

						sprintf(g_ipl_info[0].pCompleteFrame.pQtyInfo.CaptureTime, "%d, %ld, %.2f", m_FrameCount, (GetTickCount() - CapStartTime), calFPS);
#endif

						m_hCaptureCompleteThreadHandle = ::CreateThread(NULL, 0, &CompleteCapThread, 0, 0, NULL);
					}
					else
					{
						m_hCaptureCompleteThreadHandle = ::CreateThread(NULL, 0, &StopCapThread, 0, 0, NULL);


						std::memcpy(g_ipl_info[0].pBitmapImage.m_BitmapImage, g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImage, g_ipl_info[0].nImageLengthWithBitmapHeader);
						g_ipl_info[0].pBitmapImage.m_BitmapImageLen = g_ipl_info[0].nImageLengthWithBitmapHeader;

						g_ipl_info[0].pQtyInfo.Quality = g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality;
						g_ipl_info[0].pQtyInfo.X = g_ipl_info[0].pCompleteFrame.pQtyInfo.X;
						g_ipl_info[0].pQtyInfo.Y = g_ipl_info[0].pCompleteFrame.pQtyInfo.Y;
						g_ipl_info[0].pQtyInfo.R = g_ipl_info[0].pCompleteFrame.pQtyInfo.R;


						m_hCapturePreviewThreadHandle = ::CreateThread(NULL, 0, &CapturePreviewThread, 0, 0, NULL);
					}
					IsGetQtyInCap = false;
					DeviceErrorCode = 0;
					IsAutoCapComplete = true;
					gCounter = 0;

				}
				else
				{
					gCounter = gCounter >= 10 ? 0 : gCounter--;
					g_ipl_info[0].pQtyInfo.Quality = 0;
				}
			}
			//}


			isDataBufferFilled = false;
		}
	}
	catch (const std::exception &/*ex */)
	{
		isDataBufferFilled = false;
		return ErrorCode;
	}
	catch (...)
	{
		isDataBufferFilled = false;
		return ErrorCode;
	}
	return ErrorCode;
}


void CommLibCall CommEventCallback(COMM_LIB_EVENT nEventType, int nErrorCode, void *pEventData)
{
	switch (nEventType)
	{
	case COMM_EVT_PREVIEW_CB:
	{
								if (IsDeviceInit)
								{

									if (0 == nErrorCode)
									{
										ImagePreviewCallback(pEventData);
									}
									else
									{
										DeviceErrorCode = nErrorCode;
										deviceStatus = DEVICE_STREAMING_STOPPED;
										IsDeviceInit = false;
									}
								}
	}
		break;

	default:
		break;
	}
}

#pragma endregion

#pragma region Export Function
int MARVIS_Auth_UninitDevice()
{
	char szOutputString[256] = { 0 };

	ClearData();

	int retVal = CommUninitDevice();
	if (MARVIS_AUTH_E_SUCCESS != retVal && MARVIS_AUTH_E_NOT_INITIALIZED != retVal)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] - CommUninitDevice Fail.[%s]", __FUNCTION__, __LINE__,
			retVal, MARVIS_Auth_GetErrDescription(retVal));
		LOG_ERROR(szOutputString);
	}


	freeBuffer();
	freeIPLLibVariables();
	::Sleep(250);
	return retVal;
}

int MARVIS_Auth_InitDevice(int fd, const char *pcProductName, int ProductNameLen, MARVIS_DEVICE_INFO *StDeviceInfo)
{
	char szOutputString[256] = { 0 };
	int retValue = MARVIS_AUTH_E_NOT_INITIALIZED;
	try
	{
		unsigned char *encDataBuffer = NULL;
		int bufferLen = 0;
		int symmEncKeyLen = 0;

		//pcProductName[ProductNameLen] = 0;
		if (deviceStatus == DEVICE_STREAMING_STARTED)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
				MARVIS_AUTH_E_ALREADY_START_STOP, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_ALREADY_START_STOP));
			LOG_ERROR(szOutputString);
			return MARVIS_AUTH_E_ALREADY_START_STOP;
		}
		if (((pcProductName != NULL) && (pcProductName[0] == '\0')) || 0 == ProductNameLen)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
				MARVIS_AUTH_E_INVALIDPARAM, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_INVALIDPARAM));
			LOG_ERROR(szOutputString);
			return MARVIS_AUTH_E_INVALIDPARAM;
		}

		if (&StDeviceInfo == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
				MARVIS_AUTH_E_INVALIDPARAM, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_INVALIDPARAM));
			LOG_ERROR(szOutputString);
			return MARVIS_AUTH_E_INVALIDPARAM;
		}

		retValue = GetDeviceVidPid(pcProductName);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);

			return retValue;
		}

		retValue = CommIsDeviceConnected(-1, m_szVendorID, m_szProductID);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
				MARVIS_AUTH_E_NO_DEVICE, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_NO_DEVICE));
			LOG_ERROR(szOutputString);
			return MARVIS_AUTH_E_NO_DEVICE;
		}


		string strProductName(pcProductName);
		std::transform(strProductName.begin(), strProductName.end(), strProductName.begin(), ::toupper);

		deviceStatus = DEVICE_UNINITIALIZED;

		MARVIS_Auth_UninitDevice();

		retValue = CommInitDevice(-1, m_szVendorID, m_szProductID, FPS_HIGH);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommInitDevice Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}

		//

		int intVID = std::strtoul(m_szVendorID, 0, 16);
		int intPID = std::strtoul(m_szProductID, 0, 16);
		unsigned char LicenseKey[64] = { 0x0D, 0xD3, 0x39, 0x6B, 0x7B, 0x44, 0x30, 0xC7, 0xB4, 0x15, 0xA8, 0x05, 0x0D, 0x6E, 0x4F, 0xE9, 0x22,
			0x54, 0xCA, 0x88, 0x5F, 0x46, 0x24, 0x03, 0x2A, 0xD6, 0x9C, 0xCA, 0xA8, 0x0B, 0x9C, 0x57, 0xA1, 0x68, 0x5B, 0x85, 0x55, 0x4A, 0xA6,
			0x77, 0x50, 0x44, 0x90, 0xB6, 0xD6, 0x1C, 0x04, 0xA1, 0x2E, 0x19, 0x69, 0x3C, 0xC5, 0xF7, 0x60, 0xE3, 0x75, 0xC4, 0x00, 0xD7, 0xC0,
			0x93, 0x8D, 0xE8 };

		for (int j = 0; j < NO_OF_IRIS; j++)
		{
			retValue = iplLibInit(g_ipl_info[j].FULL_FRAME_WIDTH, g_ipl_info[j].FULL_FRAME_HEIGHT);
			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
					"%s - Line#[%d] ErrorCode#[%d] - iplLibInit Fail.[%s]", __FUNCTION__, __LINE__,
					retValue, MARVIS_Auth_GetErrDescription(retValue));
				LOG_ERROR(szOutputString);
				return retValue;
			}

			int VID = 0x2C0F;
			int PID = 0x2100; //MIS100V2

			retValue = iplDeviceInit(VID, PID, (unsigned char *)LicenseKey, 64);
			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
					"%s - Line#[%d] ErrorCode#[%d] - iplDeviceInit Fail.[%s]", __FUNCTION__, __LINE__,
					retValue, MARVIS_Auth_GetErrDescription(retValue));
				LOG_ERROR(szOutputString);
				return retValue;
			}

			/*g_ipl_info[j].FULL_FRAME_WIDTH = 1280;
			g_ipl_info[j].FULL_FRAME_HEIGHT = 720;
			g_ipl_info[j].FINAL_FRAME_WIDTH = 640;
			g_ipl_info[j].FINAL_FRAME_HEIGHT = 480;
			g_ipl_info[j].CROP_WIDTH = 640;
			g_ipl_info[j].CROP_HEIGHT = 480;
			g_ipl_info[j].CROP_X_COORDINATE = 320;
			g_ipl_info[j].CROP_Y_COORDINATE = 120;
			g_ipl_info[j].DPI = 557;
			memcpy(g_ipl_info[j].Firmware_Version, "3.3.1.16", strlen("3.3.1.16"));*/


			retValue = iplReadMIS100V2EEPROM(intVID, intPID, g_ipl_info[j].serialno, &g_ipl_info[j].FULL_FRAME_WIDTH, &g_ipl_info[j].FULL_FRAME_HEIGHT,
				&g_ipl_info[j].FINAL_FRAME_WIDTH, &g_ipl_info[j].FINAL_FRAME_HEIGHT, &g_ipl_info[j].CROP_WIDTH, &g_ipl_info[j].CROP_HEIGHT, &g_ipl_info[j].CROP_X_COORDINATE, &g_ipl_info[j].CROP_Y_COORDINATE, &g_ipl_info[j].DPI, g_ipl_info[j].Firmware_Version);

			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
					"%s - Line#[%d] ErrorCode#[%d] - iplReadMIS100V2EEPROM Fail.[%s]", __FUNCTION__, __LINE__,
					retValue, MARVIS_Auth_GetErrDescription(retValue));
				LOG_ERROR(szOutputString);
				return retValue;
			}

			retValue = validateMfgData(j);
			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
					"%s - Line#[%d] ErrorCode#[%d] - validateMfgData Fail.[%s]", __FUNCTION__, __LINE__,
					retValue, MARVIS_Auth_GetErrDescription(retValue));
				LOG_ERROR(szOutputString);
				return retValue;
			}

			retValue = SetDeviceInfo(StDeviceInfo, j);
			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
					"%s - Line#[%d] ErrorCode#[%d] - SetDeviceInfo Fail.[%s]", __FUNCTION__, __LINE__,
					retValue, MARVIS_Auth_GetErrDescription(retValue));
				LOG_ERROR(szOutputString);
				return retValue;
			}
		}

		retValue = GetIplParseData();
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - GetIplParseData Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}

		/**************************End Set IPL Param **************************/

		m_cbCommEventCallback = (COMM_EVT_CALLBACK)&CommEventCallback;

		if (CommRegisterCallback(&m_cbCommEventCallback) < 0)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - Failed to register callbak.",
				__FUNCTION__, __LINE__, MARVIS_AUTH_E_REGISTER_CALLBACK);
			LOG_ERROR(szOutputString);
			freeIPLLibVariables();
			return MARVIS_AUTH_E_REGISTER_CALLBACK;
		}

		IsDeviceInit = true;
		deviceStatus = DEVICE_INITIALIZED;

		allocateBuffer(640, 480);
		return MARVIS_AUTH_E_SUCCESS;
	}

	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}

	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}

	return retValue;
}

int MARVIS_Auth_IsDeviceConnected(int fd, const char *pcProductName, int ProductNameLen)
{
	char szOutputString[256] = { 0 };
	int retValue = MARVIS_AUTH_E_NO_DEVICE;
	try
	{
		//pcProductName[ProductNameLen] = 0;
		if (((pcProductName != NULL) && (pcProductName[0] == '\0')) || 0 == ProductNameLen)
		{
			return MARVIS_AUTH_E_INVALIDPARAM;
		}
		retValue = GetDeviceVidPid(pcProductName);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - GetDeviceVidPid Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}
		retValue = CommIsDeviceConnected(-1, m_szVendorID, m_szProductID);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
				MARVIS_AUTH_E_NO_DEVICE, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_NO_DEVICE));
			LOG_ERROR(szOutputString);
			return MARVIS_AUTH_E_NO_DEVICE;
		}
	}

	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	return retValue;
}

int MARVIS_Auth_StopCapture()
{
	char szOutputString[256] = { 0 };

	if (!IsDeviceInit)
	{
		return MARVIS_AUTH_E_NOT_INITIALIZED;
	}

	int retValue = MARVIS_AUTH_E_SUCCESS;
	if (!IsStopCaptureRunning)
	{
		IsStopCaptureRunning = true;
		if (deviceStatus != DEVICE_STREAMING_STARTED || deviceStatus == DEVICE_STREAMING_STOPPED)
		{
			IsStopCaptureRunning = false;
			return 0;
		}

		retValue = CommStopCapture();
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommStopCapture Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			//return retValue;
		}

		//lastIRLED = COMM_DUTY_CYCLE_OFF;
		retValue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_OFF);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			//MARVIS_Auth_StopCapture();
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommSetIRLed Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			//return retValue;
		}

#ifndef PWM_FW
		retValue = CommSetVISLed(0);

		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			//MARVIS_Auth_StopCapture();
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommSetVISLed Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			//return retValue;
		}
#endif

		IsStopButton = true;
		nTimeOutInMilliSeconds = 10000;
		deviceStatus = DEVICE_STREAMING_STOPPED;
		WaitForAutoStopThread();
		IsStopCaptureRunning = false;

	}
	if (IsGetQtyInCap)
	{
		DeviceErrorCode = MARVIS_AUTH_E_SUCCESS;
	}
	else
	{
		DeviceErrorCode = MARVIS_AUTH_E_CAPTURE_STOP;
		IsAutoCapComplete = true;
	}
	return retValue;
}

int MARVIS_Auth_StartCapture(int in_quality, int TimeOut, PREVIEW_CALLBACK p_PREVIEW_CALLBACK, CAPTURE_COMPLETE_CALLBACK p_CAPTURE_COMPLETE_CALLBACK, char* empid)
{
	char szOutputString[256] = { 0 };
	
	if (!IsDeviceInit)
	{
		return MARVIS_AUTH_E_NOT_INITIALIZED;
	}

	if (deviceStatus == DEVICE_STREAMING_STARTED)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_ALREADY_START_STOP, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_ALREADY_START_STOP));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_ALREADY_START_STOP;
	}

	if (deviceStatus == DEVICE_DISCONNECTED || deviceStatus == DEVICE_UNINITIALIZED)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_NOT_INITIALIZED, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_NOT_INITIALIZED));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_NOT_INITIALIZED;
	}

	if (!p_PREVIEW_CALLBACK || !p_CAPTURE_COMPLETE_CALLBACK)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_CALLBACK_NULLPARAM, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_CALLBACK_NULLPARAM));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_CALLBACK_NULLPARAM;
	}

#ifdef MOSIP_MODE
	if (in_quality < 0)
#else
	if (in_quality > 100 || in_quality <= 0)
#endif
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_BAD_QUALITY, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_BAD_QUALITY));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_BAD_QUALITY;
	}

	ResetStartCaptureData();

	int retValue = MARVIS_AUTH_E_OTHER;

	/*IsAutoCap = false;
	IsStopButton = false;
	IsGetQtyInCap = false;
	IsGetQtyInCapTemp = false;
	IsAutoCapComplete = false;
	DeviceErrorCode = 0;
	gCounter = 0;
	MinQtyCounter = 0;
	IsIrisLedOn = false;*/
	try
	{
		m_PREVIEW_CALLBACK = p_PREVIEW_CALLBACK;
		m_CAPTURE_COMPLETE_CALLBACK = p_CAPTURE_COMPLETE_CALLBACK;

		// Only For Auto Capture
		/*if (TimeOut < 10000)
		{
		TimeOut = 10000;
		}*/

		nTimeOutInMilliSeconds = TimeOut;
		nMinQty = in_quality;
		nReqQty = in_quality;
		nMinCapRotation = 6;

		 max_high_texture=0;
		 min_high_texture=0;
		 max_low_texture = 0;
		min_low_texture = 0;

		/*retValue = CommUninitDevice();
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
		"%s - Line#[%d] ErrorCode#[%d] - CommUninitDevice Fail.[%s]", __FUNCTION__, __LINE__,
		retValue, MARVIS_Auth_GetErrDescription(retValue));
		LOG_ERROR(szOutputString);
		return retValue;
		}*/

		//::Sleep(100);

		retValue = CommInitDevice(-1, m_szVendorID, m_szProductID, FPS_HIGH);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommInitDevice Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}
		//::Sleep(100);
		prev_blur_value = 0;
		pre_mean = 0;
		IRLed_Status = true;
		IRLed_On = false;
		ISComplete = false;
		hideframecounter = 0;

		lowpwmhighmean = 0;
		irPropety.clear();
		pwmcounter = 0;
		prev_pwmValue = 0;
		highPwm = 0;
		midPwm = 0;
		lowPwm = 0;
		updown = false;
		high_meanvalue = 0;
		mid_meanvalue = 0;
		low_meanvalue = 0;
		memset(emp_id, 0, 2048);
		strcpy(emp_id, empid);
		retValue = CommStartCapture();
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			MARVIS_Auth_StopCapture();
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommStartCapture Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}

		//lastIRLED = COMM_DUTY_CYCLE_LEVEL15;
		prev_pwmValue = COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL17;
		retValue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL17);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			MARVIS_Auth_StopCapture();
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] - CommSetIRLed Fail.[%s]", __FUNCTION__, __LINE__,
			retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}
		//pwmchangecount = arr1[rand() % 4];
#ifndef PWM_FW
		retValue = CommSetVISLed(1);

		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			MARVIS_Auth_StopCapture();
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommSetVISLed Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}
#endif

		m_NoOfFrameCount = 0;
		m_fFPS = 0.0f;
		QueryPerformanceFrequency(&FpsframeFrequency);
		QueryPerformanceCounter(&FpsStartTime);


		deviceStatus = DEVICE_STREAMING_STARTED;
		CapStartTime = GetTickCount();

		if (TimeOut != 0) {
			DWORD dwAutoStopThreadID = 0;
			m_hAutoStopThreadHandle = ::CreateThread(NULL, 0, &AutoStopCapThread, 0, 0, NULL);
		}
	}
	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}

	return retValue;
}

int MARVIS_Auth_AutoCapture_RD(int in_lowerThreshQty, int in_upperThreshQty, int in_noOfFrameCash, int TimeOut, int *Quality, PREVIEW_CALLBACK p_PREVIEW_CALLBACK, MARVIS_IMAGE_DATA *imageData)
{
	char szOutputString[256] = { 0 };

	if (!IsDeviceInit)
	{
		return MARVIS_AUTH_E_NOT_INITIALIZED;
	}

	if (deviceStatus == DEVICE_STREAMING_STARTED)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_ALREADY_START_STOP, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_ALREADY_START_STOP));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_ALREADY_START_STOP;
	}

	if (deviceStatus == DEVICE_DISCONNECTED || deviceStatus == DEVICE_UNINITIALIZED)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_NOT_INITIALIZED, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_NOT_INITIALIZED));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_NOT_INITIALIZED;
	}

	if (!p_PREVIEW_CALLBACK)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_CALLBACK_NULLPARAM, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_CALLBACK_NULLPARAM));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_CALLBACK_NULLPARAM;
	}

	if (in_lowerThreshQty > 100 || in_lowerThreshQty < 0 || in_upperThreshQty > 100 || in_upperThreshQty < 0 || in_lowerThreshQty > in_upperThreshQty)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_BAD_QUALITY, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_BAD_QUALITY));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_BAD_QUALITY;
	}

	ResetStartCaptureData();

	int retValue = MARVIS_AUTH_E_OTHER;

	IsAutoCap = true;
	/*IsTimeOutAutoCap = false;
	IsGetQtyInCap = false;
	IsGetQtyInCapTemp = false;
	IsAutoCapComplete = false;
	DeviceErrorCode = 0;
	IsStopButton = false;
	gCounter = 0;
	MinQtyCounter = 0;
	IsIrisLedOn = false;*/
	try
	{
		m_PREVIEW_CALLBACK = p_PREVIEW_CALLBACK;

		if (TimeOut < 10000)
		{
			TimeOut = 10000;
		}
		nTimeOutInMilliSeconds = TimeOut;
		nMinQty = in_lowerThreshQty;
		nReqQty = in_upperThreshQty;
		nMinCapRotation = in_noOfFrameCash;

		/*retValue = CommUninitDevice();
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
		"%s - Line#[%d] ErrorCode#[%d] - CommUninitDevice Fail.[%s]", __FUNCTION__, __LINE__,
		retValue, MARVIS_Auth_GetErrDescription(retValue));
		LOG_ERROR(szOutputString);
		return retValue;
		}*/

		//::Sleep(100);

		retValue = CommInitDevice(-1, m_szVendorID, m_szProductID, FPS_HIGH);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommInitDevice Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}

		//::Sleep(100);

		retValue = CommStartCapture();
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			CommStopCapture();
			WaitForAutoStopThread();

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommStartCapture Fail.", __FUNCTION__, __LINE__,
				retValue);
			LOG_ERROR(szOutputString);
			return retValue;
		}

		/*lastIRLED = COMM_DUTY_CYCLE_LEVEL15;
		retValue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL15);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
		MARVIS_Auth_StopCapture();
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
		"%s - Line#[%d] ErrorCode#[%d] - CommSetIRLed Fail.[%s]", __FUNCTION__, __LINE__,
		retValue, MARVIS_Auth_GetErrDescription(retValue));
		LOG_ERROR(szOutputString);
		return retValue;
		}*/

#ifndef PWM_FW
		retValue = CommSetVISLed(1);

		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			MARVIS_Auth_StopCapture();
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommSetVISLed Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}
#endif

		m_NoOfFrameCount = 0;
		m_fFPS = 0.0f;
		QueryPerformanceFrequency(&FpsframeFrequency);
		QueryPerformanceCounter(&FpsStartTime);

		deviceStatus = DEVICE_STREAMING_STARTED;
		CapStartTime = GetTickCount();

		//Auto Stop
		if (TimeOut != 0) {
			DWORD dwAutoStopThreadID = 0;
			m_hAutoStopThreadHandle = ::CreateThread(NULL, 0, &AutoStopCapThread, 0, 0, &dwAutoStopThreadID);
		}

		while (!IsAutoCapComplete)
		{
			MSG uMsg;
			while (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE) > 0)
			{
				TranslateMessage(&uMsg);
				DispatchMessage(&uMsg);
			}
			::Sleep(10);
		}

		if (true == IsGetQtyInCapTemp)
		{
			imageData->m_BitmapImageLen = g_ipl_info[0].pBitmapImage.m_BitmapImageLen;
			memcpy(imageData->m_BitmapImage, g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImage, g_ipl_info[0].pBitmapImage.m_BitmapImageLen);

			*Quality = g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality;

		}
		else if (true == IsTimeOutAutoCap)
		{
			return MARVIS_AUTH_E_TIMEOUT;
		}
		else if (MARVIS_AUTH_E_SUCCESS != DeviceErrorCode)
		{
			return DeviceErrorCode;
		}


	}
	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	return retValue;
}

int MARVIS_Auth_AutoCapture(int TimeOut, int *Quality, PREVIEW_CALLBACK p_PREVIEW_CALLBACK, MARVIS_IMAGE_DATA *imageData)
{
	char szOutputString[256] = { 0 };

	if (!IsDeviceInit)
	{
		return MARVIS_AUTH_E_NOT_INITIALIZED;
	}

	if (deviceStatus == DEVICE_STREAMING_STARTED)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_ALREADY_START_STOP, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_ALREADY_START_STOP));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_ALREADY_START_STOP;
	}

	if (deviceStatus == DEVICE_DISCONNECTED || deviceStatus == DEVICE_UNINITIALIZED)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_NOT_INITIALIZED, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_NOT_INITIALIZED));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_NOT_INITIALIZED;
	}

	if (!p_PREVIEW_CALLBACK)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_CALLBACK_NULLPARAM, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_CALLBACK_NULLPARAM));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_CALLBACK_NULLPARAM;
	}

	ResetStartCaptureData();

	int retValue = MARVIS_AUTH_E_OTHER;

	IsAutoCap = true;

	try
	{
		m_PREVIEW_CALLBACK = p_PREVIEW_CALLBACK;

		if (TimeOut < 10000)
		{
			TimeOut = 10000;
		}
		nTimeOutInMilliSeconds = TimeOut;
		nMinQty = 60;
		nReqQty = 85;
		nMinCapRotation = 6;

		retValue = CommInitDevice(-1, m_szVendorID, m_szProductID, FPS_HIGH);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommInitDevice Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}

		//::Sleep(100);
		prev_blur_value = 0;
		pre_mean = 0;
		IRLed_Status = true;
		IRLed_On = false;
		ISComplete = false;
		hideframecounter = 0;

		lowpwmhighmean = 0;
		irPropety.clear();
		pwmcounter = 0;
		prev_pwmValue = 0;
		highPwm = 0;
		midPwm = 0;
		lowPwm = 0;
		updown = false;
		high_meanvalue = 0;
		mid_meanvalue = 0;
		low_meanvalue = 0;
		memset(emp_id, 0, 2048);
		//strcpy(emp_id, empid);
		retValue = CommStartCapture();
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			CommStopCapture();
			WaitForAutoStopThread();

			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommStartCapture Fail.", __FUNCTION__, __LINE__,
				retValue);
			LOG_ERROR(szOutputString);
			return retValue;
		}

		prev_pwmValue = COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL17;
		retValue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL::COMM_DUTY_CYCLE_LEVEL17);
		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			MARVIS_Auth_StopCapture();
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommSetIRLed Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}
		//pwmchangecount = arr1[rand() % 4];
#ifndef PWM_FW
		retValue = CommSetVISLed(1);

		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			MARVIS_Auth_StopCapture();
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - CommSetVISLed Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);
			return retValue;
		}
#endif

		m_NoOfFrameCount = 0;
		m_fFPS = 0.0f;
		QueryPerformanceFrequency(&FpsframeFrequency);
		QueryPerformanceCounter(&FpsStartTime);

		deviceStatus = DEVICE_STREAMING_STARTED;
		CapStartTime = GetTickCount();

		//Auto Stop
		if (TimeOut != 0) {
			DWORD dwAutoStopThreadID = 0;
			m_hAutoStopThreadHandle = ::CreateThread(NULL, 0, &AutoStopCapThread, 0, 0, &dwAutoStopThreadID);
		}

		while (!IsAutoCapComplete)
		{
			MSG uMsg;
			while (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE) > 0)
			{
				TranslateMessage(&uMsg);
				DispatchMessage(&uMsg);
			}
			::Sleep(10);
		}

		if (true == IsGetQtyInCapTemp)
		{
			imageData->m_BitmapImageLen = g_ipl_info[0].pBitmapImage.m_BitmapImageLen;
			memcpy(imageData->m_BitmapImage, g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImage, g_ipl_info[0].pBitmapImage.m_BitmapImageLen);

			*Quality = g_ipl_info[0].pCompleteFrame.pQtyInfo.Quality;

		}
		else if (true == IsTimeOutAutoCap)
		{
			return MARVIS_AUTH_E_TIMEOUT;
		}
		else if (MARVIS_AUTH_E_SUCCESS != DeviceErrorCode)
		{
			return DeviceErrorCode;
		}


	}
	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	return retValue;
}

int MARVIS_Auth_GetImage(unsigned char *out_pImage, int *ImageLen, IMAGE_FORMAT format, int CompressionRatio) {

	if (NULL == out_pImage || NULL == ImageLen) {
		return MARVIS_AUTH_E_NULLPARAM;
	}

	if (!IsGetQtyInCapTemp || g_ipl_info[0].pCompleteFrame.Final_Frame == NULL) {
		return MARVIS_AUTH_E_NULL_TEMPLATE;
	}

	char szOutputString[256] = { 0 };
	int retValue = 0;

	int l_len = g_ipl_info[0].FINAL_FRAME_WIDTH * g_ipl_info[0].FINAL_FRAME_HEIGHT;

	unsigned char *tmp_Final_Frame = rotate180(g_ipl_info[0].pCompleteFrame.Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT);

	switch (format)
	{
	case BMP:
		if (g_ipl_info[0].pCompleteFrame.Final_Frame != NULL && g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImageLen != NULL && g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImageLen > 0)
		{
			retValue = iplConvertRawToBmp(g_ipl_info[0].pCompleteFrame.Final_Frame, out_pImage, g_ipl_info[0].FINAL_FRAME_HEIGHT, g_ipl_info[0].FINAL_FRAME_WIDTH, 2);
			if (MARVIS_AUTH_E_SUCCESS != retValue)
			{
				_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
					"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
					retValue, MARVIS_Auth_GetErrDescription(retValue));
				LOG_ERROR(szOutputString);

				*ImageLen = 0;
			}
			else
			{
				*ImageLen = g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImageLen;
			}
		}
		break;
	case RAW:
	{
				if (tmp_Final_Frame != NULL)
				{
					memcpy(out_pImage, tmp_Final_Frame, (g_ipl_info[0].FINAL_FRAME_HEIGHT*g_ipl_info[0].FINAL_FRAME_WIDTH));
					*ImageLen = (g_ipl_info[0].FINAL_FRAME_HEIGHT*g_ipl_info[0].FINAL_FRAME_WIDTH);
				}
	}
		break;
	case K7:
	{
			   unsigned char* ISOK7Jp2Data = (unsigned char*)calloc(l_len, sizeof(unsigned char));
			   int ISOK7Jp2DataLength = 0;

			   int K7Width = 0;
			   int K7Height = 0;

			   memset(ISOK7Jp2Data, 0, l_len);

			   if (tmp_Final_Frame != NULL && g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImageLen > 0)
			   {
				   retValue = ire_kind7(tmp_Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, ISOK7Jp2Data, K7Width, K7Height, ISOK7Jp2DataLength, true);

				   if (MARVIS_AUTH_E_SUCCESS == retValue)
				   {
					   *ImageLen = ISOK7Jp2DataLength;;
					   memcpy(out_pImage, ISOK7Jp2Data, *ImageLen);
				   }
				   else
				   {
					   //printf("K7 Image Not Generated\n");
					   retValue = MARVIS_AUTH_E_BAD_TEMPLATE;
					   _snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
						   "%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
						   retValue, MARVIS_Auth_GetErrDescription(retValue));
					   LOG_ERROR(szOutputString);
				   }
			   }

			   free(ISOK7Jp2Data);
			   break;
	}
	case K3:
	{
			   int K3Width = 0;
			   int K3Height = 0;

			   unsigned char* ISOK3Jp2Data = (unsigned char*)calloc(l_len, sizeof(unsigned char));
			   int ISOK3Jp2DataLength = 0;
			   memset(ISOK3Jp2Data, 0, l_len);

			   if (tmp_Final_Frame != NULL && g_ipl_info[0].pCompleteFrame.pBitmap.m_BitmapImageLen > 0)
			   {
				   retValue = ire_kind7(tmp_Final_Frame, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, ISOK3Jp2Data, K3Width, K3Height, ISOK3Jp2DataLength, false);
				   if (MARVIS_AUTH_E_SUCCESS == retValue)
				   {
					   *ImageLen = ISOK3Jp2DataLength;
					   memcpy(out_pImage, ISOK3Jp2Data, *ImageLen);
				   }
				   else
				   {
					   //printf("K3 Image Not Generated\n");
					   retValue = MARVIS_AUTH_E_BAD_TEMPLATE;

					   _snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
						   "%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
						   retValue, MARVIS_Auth_GetErrDescription(retValue));
					   LOG_ERROR(szOutputString);
				   }
			   }
			   free(ISOK3Jp2Data);
			   break;
	}
	default:
		retValue = MARVIS_AUTH_E_BAD_TEMPLATE;
		break;
	}

	free(tmp_Final_Frame);

	if (retValue > 0) {
		return -(retValue);
	}
	return retValue;
}


int MARVIS_Auth_MatchIrisData(unsigned char* galleryImg, unsigned char* probImg, int* out_score)
{

	if (NULL == galleryImg || NULL == probImg || NULL == out_score) {
		return MARVIS_AUTH_E_NULLPARAM;
	}

	/*FILE* pEncodedFile = NULL;
	fopen_s(&pEncodedFile, "probImg.bmp", "wb");
	fwrite(probImg, 1, 308278, pEncodedFile);
	fclose(pEncodedFile);

	FILE* pEncodedFile1 = NULL;
	fopen_s(&pEncodedFile1, "galleryImg.bmp", "wb");
	fwrite(galleryImg, 1, 308278, pEncodedFile1);
	fclose(pEncodedFile1);*/

	char szOutputString[256] = { 0 };
	int retValue = MARVIS_AUTH_E_SUCCESS;
	try
	{
		features_entry *enroll_features_list = NULL;
		enroll_features_list = EnrollIris_Single(probImg, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT);
		if (enroll_features_list == NULL)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - EnrollIris_Single Fail.[%s]", __FUNCTION__, __LINE__,
				MARVIS_AUTH_E_MATCH_IMAGE_FAILED, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_MATCH_IMAGE_FAILED));
			LOG_ERROR(szOutputString);
			return MARVIS_AUTH_E_MATCH_IMAGE_FAILED;
		}

		retValue = MatchIris(enroll_features_list, galleryImg, g_ipl_info[0].FINAL_FRAME_WIDTH, g_ipl_info[0].FINAL_FRAME_HEIGHT, out_score);

		if (MARVIS_AUTH_E_SUCCESS != retValue)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] ErrorCode#[%d] - MatchIris Fail.[%s]", __FUNCTION__, __LINE__,
				retValue, MARVIS_Auth_GetErrDescription(retValue));
			LOG_ERROR(szOutputString);

			return retValue;
		}
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	return retValue;
}

int MARVIS_Auth_GetDeviceList(DEVICE_LIST *StDeviceList, int *DeviceCnt)
{
	char szOutputString[256] = { 0 };
	int retValue = MARVIS_AUTH_E_NO_DEVICE;
	string DeviceName[4] = { MIS100V2 };
	try
	{
		int j = 0;
		for (int i = 0; i < 1; i++)
		{
			retValue = GetDeviceVidPid((char*)DeviceName[i].c_str());
			if (MARVIS_AUTH_E_SUCCESS == retValue)
			{
				retValue = CommIsDeviceConnected(-1, m_szVendorID, m_szProductID);
				if (MARVIS_AUTH_E_SUCCESS == retValue)
				{
					if (StDeviceList != NULL)
					{
						memcpy(StDeviceList[j].Model, DeviceName[i].c_str(), strlen((char*)DeviceName[i].c_str()));
						StDeviceList[j].Model[strlen((char*)DeviceName[i].c_str())] = 0;
					}
					j++;

				}
			}
		}
		*DeviceCnt = j;
	}

	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}

	return MARVIS_AUTH_E_SUCCESS;
}

int MARVIS_Auth_GetSupportedDeviceList(DEVICE_LIST *StDeviceList, int *DeviceCnt)
{
	char szOutputString[256] = { 0 };
	int retValue = MARVIS_AUTH_E_NO_DEVICE;
	try
	{
		string DeviceName[4] = { MIS100V2 };
		int j = 0;
		for (int i = 0; i < 1; i++)
		{
			retValue = GetDeviceVidPid((char*)DeviceName[i].c_str());
			if (MARVIS_AUTH_E_SUCCESS == retValue)
			{
				if (StDeviceList != NULL)
				{
					memcpy(StDeviceList[j].Model, DeviceName[i].c_str(), strlen((char*)DeviceName[i].c_str()));
					StDeviceList[j].Model[strlen((char*)DeviceName[i].c_str())] = 0;
				}
				j++;

			}
		}
		*DeviceCnt = j;
	}

	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	return retValue;
}

int MARVIS_Auth_EnableLogs(const MARVIS_AUTH_LOG_LEVEL eLogLevel, const char *pFolderPath)
{
	char szOutputString[256] = { 0 };

	int retValue = 0;

	if (eLogLevel != MARVIS_AUTH_LOG_LEVEL_OFF && eLogLevel != MARVIS_AUTH_LOG_LEVEL_ERROR)
	{
		return MARVIS_AUTH_E_INVALIDPARAM;
	}
	LogLevel eLogLevelInternal = LOG_LEVEL_ERROR;

	switch (eLogLevel)
	{
	case LOG_LEVEL_ERROR:
		eLogLevelInternal = LOG_LEVEL_ERROR;
		break;
	case LOG_LEVEL_INFO:
		eLogLevelInternal = LOG_LEVEL_INFO;
		break;
	case LOG_LEVEL_DEBUG:
		eLogLevelInternal = LOG_LEVEL_DEBUG;
		break;
	}

	if (eLogLevel != DISABLE_LOG)
	{
		if (pFolderPath != NULL)
		{
			LOG_SETPATH(pFolderPath);
			CNativeLogger::getInstance()->updateLogLevel(eLogLevelInternal);
			//CLogger::getInstance()->enableLog();
			LOG_ALWAYS("File opened successfully on given directory path.");
		}
		else
		{
			CNativeLogger::getInstance()->updateLogLevel(eLogLevelInternal);
		}
	}
	else
	{
		CNativeLogger::getInstance()->disableLog();
	}

	retValue = CommEnableLogs((COMM_LOG_LEVEL)eLogLevel, pFolderPath);
	if (MARVIS_AUTH_E_SUCCESS != retValue)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] - EnableLogs Fail.[%s]", __FUNCTION__, __LINE__,
			retValue, MARVIS_Auth_GetErrDescription(retValue));
		LOG_ERROR(szOutputString);
		return retValue;
	}
	return MARVIS_AUTH_E_SUCCESS;
}

const char *MARVIS_Auth_GetErrDescription(int errorNo)
{
	if (errorNo <= -1 && errorNo >= -100)
	{
		return  CommGetErrorDescription(errorNo);
	}
	else
	{
		switch (errorNo)
		{
		case MARVIS_AUTH_E_NO_DEVICE:
			return "MIS100V2 not Found";
		case MARVIS_AUTH_E_INVALIDPARAM:
			return "Invalid Function parameter";
		case MARVIS_AUTH_E_NULLPARAM:
			return "Null Parameters";
		case MARVIS_AUTH_E_MEMORY:
			return "Not Enough Memory";
		case MARVIS_AUTH_E_CORRUPT_SERIAL:
			return "Serial no is corrupted";
		case MARVIS_AUTH_E_UNKNOWN_SENSOR:
			return "Unknown Sensor";
		case MARVIS_AUTH_E_NOT_INITIALIZED:
			return "MIS100V2 not initialized";
		case MARVIS_AUTH_E_SYNC_PROBLEM:
			return "MIS100V2 Sync Problem";
		case MARVIS_AUTH_E_TIMEOUT:
			return "Timeout from Function";
		case MARVIS_AUTH_E_NOT_GOOD_IMAGE:
			return "Input Image is not Good";
		case MARVIS_AUTH_E_BLANKIMAGE:
			return "Invalid image or unsupported image format";
		case MARVIS_AUTH_E_EXTRACTOR_INIT_FAILED:
			return "Extractor Library cannot Initialize";
		case MARVIS_AUTH_E_FILE_IO:
			return "Error occured while opening/reading file";
		case MARVIS_AUTH_E_BAD_LICENSE:
			return "Provided license is not valid, or no license was found";
		case MARVIS_AUTH_E_BAD_FORMAT:
			return "Unsupported Format";
		case MARVIS_AUTH_E_BAD_VALUE:
			return "Invalid Value Provided";
		case MARVIS_AUTH_E_BAD_TEMPLATE:
			return "Invalide template or unsupported template format";
		case MARVIS_AUTH_E_READ_ONLY:
			return "value cannot be modified";
		case MARVIS_AUTH_E_NOT_DEFINED:
			return "value is not defined";
		case MARVIS_AUTH_E_NULL_TEMPLATE:
			return "template is null";
		case MARVIS_AUTH_E_NOT_IMPLEMENTED:
			return "Function Not Implemented";
		case MARVIS_AUTH_E_BAD_QUALITY:
			return "Quality must be between 1 and 100. ";
		case MARVIS_AUTH_E_CAPTURING_STOPPED:
			return "Capturing stopped";
		case MARVIS_AUTH_E_LATENT_FINGER:
			return "Latent Iris on device, clean and then initialize";
		case MARVIS_AUTH_E_ALREADY_START_STOP:
			return "MIS100V2 already started or already stopped";
		case MARVIS_AUTH_E_LOAD_FIRMWARE_FAILED:
			return "Firmware cannot load on device, try again or check security setting at udev";
		case MARVIS_AUTH_E_OTHER:
			return "Other unspecified Error";
		case MARVIS_AUTH_E_INVALID_REQUEST:
			return "Invalid request";
		case MARVIS_AUTH_E_EVOLUTION_PERIOD_EXPIRED:
			return "Evaluation period has expired";
		case	MARVIS_AUTH_E_REGISTER_CALLBACK:
			return "Failed to Register Callback";
		case MARVIS_AUTH_E_EXCEPTION_OCCURRED:
			return "Exception Occured";
		case MARVIS_AUTH_E_CAPTURE_STOP:
			return "Capture Stop";
		case MARVIS_AUTH_E_CALLBACK_NULLPARAM:
			return "Callback null or empty";
		case MARVIS_AUTH_E_MATCH_IMAGE_FAILED:
			return "Failed to match image";

		case MARVIS_AUTH_E_FULL_FRAME_CORRUPTED:
			return "Full Frame is corrupted";
		case MARVIS_AUTH_E_FINAL_FRAME_CORRUPTED:
			return "Fimal Frame is corrupted";
		case MARVIS_AUTH_E_CROP_CORRUPTED:
			return "Crop Frame is corrupted";
		case MARVIS_AUTH_E_COORDINATE_CORRUPTED:
			return "Coordinate is corrupted";
		case MARVIS_AUTH_E_FIRMWARE_CORRUPTED:
			return "Firmwareversion no is corrupted";
		case MARVIS_AUTH_E_FAKE_IRISFOUND:
			return "Iris not Approved";
		default:
			return "Error code not evaulated";
		}
	}
}

int MARVIS_Auth_GetVersion(char *out_pLibVersion)
{
	if (NULL == out_pLibVersion)
	{
		return MARVIS_AUTH_E_NULLPARAM;
	}
	if (out_pLibVersion)
	{
		std::string strVersion = VER_FILE_VERSION_STR;
		strncpy_s(out_pLibVersion, COMM_LIB_VER_MAX_LEN, strVersion.c_str(), strVersion.length());
		return MARVIS_AUTH_E_SUCCESS;
	}
	return MARVIS_AUTH_E_EXCEPTION_OCCURRED;
}

int MARVIS_Auth_GetFunctionalInfo(char *exeTime)
{
	char FunInfo[50];

	//float fps = MARVIS_Enroll_GetLeftFPS();

	//FunInfo = GetIrisFunctionInfo() + to_string(fps);
	unsigned long tDiff = GetTickCount() - CapStartTime;

	//sprintf(FunInfo, "%s%.2f", GetIrisFunctionInfo(), fps);
	//sprintf(FunInfo, "%ld, %ld", tDiff, (CompleteCapTime - CaptureStartTime));

	memcpy(exeTime, FunInfo, 20);

	return 0;
}

int MARVIS_Auth_PWMLevel(int pwmvalue)
{

	char szOutputString[256] = { 0 };

	if (!IsDeviceInit)
	{
		return MARVIS_AUTH_E_NOT_INITIALIZED;
	}
	int retValue = MARVIS_AUTH_E_SUCCESS;

	retValue = retValue = CommSetIRLed(COMM_DUTY_CYCLE_LEVEL(pwmvalue));
	if (MARVIS_AUTH_E_SUCCESS != retValue)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] - CommStopCapture Fail.[%s]", __FUNCTION__, __LINE__,
			retValue, MARVIS_Auth_GetErrDescription(retValue));
		LOG_ERROR(szOutputString);
		//return retValue;
	}

	return retValue;
}
#pragma endregion

#pragma region Auto Connect Disconncet Event

DWORD WINAPI DeviceDetectionThread(LPVOID lpParam)
{
	char szOutputString[256] = { 0 };
	bool currentStatus[4] = { false };

	while (IsCheckDeviceThread)
	{
		try
		{
			int retVal = CommIsDeviceConnected(-1, MIS100V2_VID_STR, MIS100V2_PID_STR);
			if (COMM_E_SUCCESS == retVal)
			{
				currentStatus[0] = true;
			}
			else
			{
				currentStatus[0] = false;
			}

			if (currentStatus[0] != gPreviousDeviceStatus[0])
			{
				if (currentStatus[0])
				{
					if (m_DEVICE_DETECTION)
					{
						m_DEVICE_DETECTION(MIS100V2, EVENT_CONNECTED);
					}
				}
				else
				{
					if ((strcmp(MIS100V2_PID_STR, m_szProductID) == 0) && (strcmp(MIS100V2_VID_STR, m_szVendorID) == 0))
					{
						Auto_UninitDevice();
					}

					if (m_DEVICE_DETECTION)
					{
						m_DEVICE_DETECTION(MIS100V2, EVENT_DISCONNECTED);
					}
				}

				gPreviousDeviceStatus[0] = currentStatus[0];
			}

			::Sleep(500);
		}
		catch (...)
		{
			_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
				"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
			LOG_ERROR(szOutputString);
		}
	}

	return 0;
}
int MARVIS_Auth_RegisterDetectionCallback(DEVICE_DETECTION p_DEVICE_DETECTION)
{
	char szOutputString[256] = { 0 };
	int retValue = MARVIS_AUTH_E_CALLBACK_NULLPARAM;
	if (!p_DEVICE_DETECTION)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] ErrorCode#[%d] -[%s]", __FUNCTION__, __LINE__,
			MARVIS_AUTH_E_CALLBACK_NULLPARAM, MARVIS_Auth_GetErrDescription(MARVIS_AUTH_E_CALLBACK_NULLPARAM));
		LOG_ERROR(szOutputString);
		return MARVIS_AUTH_E_CALLBACK_NULLPARAM;
	}
	try
	{
		m_DEVICE_DETECTION = p_DEVICE_DETECTION;

		gPreviousDeviceStatus[0] = false;
		//GetConnectedDeviceList();
		retValue = MARVIS_AUTH_E_SUCCESS;
	}
	catch (const std::exception &ex)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred -[%s].", __FUNCTION__, __LINE__, ex.what());
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	catch (...)
	{
		_snprintf_s(szOutputString, sizeof(szOutputString), sizeof(szOutputString),
			"%s - Line#[%d] - Exception occurred.", __FUNCTION__, __LINE__);
		LOG_ERROR(szOutputString);
		retValue = MARVIS_AUTH_E_EXCEPTION_OCCURRED;
	}
	return retValue;
}
void WaitForDeviceDetectionThread()
{
	IsCheckDeviceThread = false;
	if (m_hAutoDeviceDetectThreadHandle)
	{
		switch (WaitForSingleObject(m_hAutoDeviceDetectThreadHandle, 1000))
		{
		case WAIT_OBJECT_0:
		{
							  break;
		}

		case WAIT_TIMEOUT:
		{
							 break;
		}

		default:
		{
				   TerminateThread(m_hAutoDeviceDetectThreadHandle, 0);
				   break;
		}
		}

		CloseHandle(m_hAutoDeviceDetectThreadHandle);
		m_hAutoDeviceDetectThreadHandle = NULL;
	}
}
void AutoDetection()
{
	IsCheckDeviceThread = true;
	Sleep(10);
	m_hAutoDeviceDetectThreadHandle = ::CreateThread(NULL, 0, &DeviceDetectionThread, 0, 0, NULL);
}
#pragma endregion