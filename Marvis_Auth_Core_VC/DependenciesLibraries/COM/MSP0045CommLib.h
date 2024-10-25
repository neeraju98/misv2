/******************************************************************************

	 Copyright (c) 2020  Mantra Softech (India) Pvt Ltd. All rights reserved.

		MSP0045CommLib.h	(API declarations for MATISX Iris Auth device)

	 Mantra Iris Scanner Device Communication Library APIs & Declarations.

******************************************************************************/

#ifndef __MSP0045_COMM_LIB_H__
#define __MSP0045_COMM_LIB_H__

#if defined (__cplusplus)
extern "C" {
#endif

#pragma pack(push, 1)   // Exact Fit - No Padding

#ifndef CommLibCall MIS100V2_PID
#define CommLibCall __stdcall
#endif

/************************************ Macros **********************************/

/** Max length for device version info */
#define COMM_LIB_VER_MAX_LEN                      12    ///< Length in Bytes.

/** Max length for manufacturing data */
#define COMM_MFG_DATA_MAX_LEN                     256   ///< Length in Bytes.

/** Max length for error description */
#define COMM_ERROR_DESC_MAX_LEN                   128   ///< Length in Bytes.
#define COMM_FIRMWARE_VERSION_INFO_LENGTH		  37		///< Length in Bytes.

const char MIS100V2_VID_STR[] = "2C0F";	        ///< MIS100V2 Vendor ID.
const char MIS100V2_PID_STR[] = "2100";           ///< MIS100V2 Product ID.

/********************************** Enumerations ******************************/

/**
 * @brief The COMM_DEVICE_FPS enum : enum for various FPS at which device operates.
 */
typedef enum _COMM_DEVICE_FPS
{
    /** Low FPS (For Android) */
    FPS_LOW = 0,

    /** High FPS (For Windows and Linux). This is default value. */
    FPS_HIGH = 1,
} COMM_DEVICE_FPS;

typedef enum _COMM_LOG_LEVEL
{
    /** log level 0 - no logs - default */
	COMM_LEVEL_LOG_OFF = 0,

    /** log level 1 - for error messages */
	COMM_LEVEL_LOG_ERROR = 1,
#ifdef DEVELOPMENT_MODE
    /** log level 2 - for user information messages */
	COMM_LEVEL_LOG_INFO  = 2,

    /** log level 3 - for user debug messages */
	COMM_LEVEL_LOG_DEBUG = 3,
#endif
} COMM_LOG_LEVEL;

typedef struct _COMM_IMAGE_PREVIEW_PARAMS
{
   /** This variable will hold image data.*/
	unsigned char *pCapturedImage;

   /** This will hold length of image data */
	unsigned int nImageBytes;
} COMM_IMAGE_PREVIEW_PARAMS;

typedef enum _COMM_LIB_EVENT
{
    /** Event for preview call back */
    COMM_EVT_PREVIEW_CB = 0,
}COMM_LIB_EVENT;


//*********************************************************************************************************************
//
//													Error/Return Codes
//
//*********************************************************************************************************************
typedef enum _COMM_ERROR_CODES
{
	/** Success response */
	COMM_E_SUCCESS = 0,

	/** Invalid input parameter given to library in respective API */
	COMM_E_INVALID_INPUT_PARAM = -1,

	/** Invalid device handle */
	COMM_E_HANDLE_INVALID = -2,

	/** Timeout occured */
	COMM_E_COMMAND_TIMEOUT = -3,

	/** Unsupported API called for device */
	COMM_E_METHOD_NOT_SUPPORTED = -4,

	/** Error due to libusb/device failure */
	COMM_E_HARDWARE_INTERFACE_ERR = -5,

	/** Memory allocation failed */
	COMM_E_MEMORY_ALLOC_FAILED = -6,

	/** Failed to init library */
	COMM_E_INIT_LIB_FAILED = -7,

	/** Failed to init device */
	COMM_E_INIT_DEVICE_FAILED = -8,

	/** Device not connected */
	COMM_E_NO_DEVICE = -11,

	/** Device is not initialized */
	COMM_E_DEVICE_NOT_INITIALIZED = -12,

	/** Failed to get device count */
	COMM_E_FAILED_TO_GET_DEV_CNT = -14,

	/** Failed to get device Descriptor */
	COMM_E_FAILED_TO_GET_DESC = -15,

	/** Failed to get device using handler */
	COMM_E_FAILED_TO_GET_DEVICE = -16,

	/** Failed to get config descriptor */
	COMM_E_FAILED_TO_GET_CONF_DESCRIPTOR = -17,

	/** Failed no end point found above 0x80 value */
	COMM_E_NO_ENDPOINT = -18,

	/** Failed to claim both Iris Sensor */
	COMM_E_FAILED_TO_CLAIM_DEVICE = -19,

	/** Start capture is already in progress */
	COMM_E_START_CAPTURE_ALREADY_IN_PROGRESS = -31,

	/** Failed to start capture */
	COMM_E_START_CAPTURE_FAILED = -32,

	/** Failed to stop capture */
	COMM_E_STOP_CAPTURE_FAILED = -33,

	/** Failed to read mfg data from Primary address */
	COMM_E_READ_MFG_DATA_FROM_PRIMARY_FAILED = -41,

	/** Failed to read data from Secondary address */
	COMM_E_READ_MFG_DATA_FROM_SECONDARY_FAILED = -42,

	/** Failed to write MFG data to Primary address */
	COMM_E_WRITE_MFG_DATA_ON_PRIMARY_FAILED = -43,

	/** Failed to write data to Secondary address */
	COMM_E_WRITE_MFG_DATA_ON_SECONDARY_FAILED = -44,

	/** Failed to update LED */
	COMM_E_LED_UPDATE_FAILED = -51,

	/** Invalid Duty Cycle Level */
	COMM_E_INVALID_DUTY_CYCLE_LEVEL = -52,

	/** Failed to update duty cycle */
	COMM_E_DUTY_CYCLE_UPDATE_FAILED = -53,

	/** Failed to update duty cycle */
	COMM_E_STOP_LIGHT_FAILED = -54,

	/*Reset firmware version info failed*/
	COMM_E_READ_FIRMWARE_VERSION_INFO_FAILED = -55,         

	/*Failed to force FX2 CPU into reset*/
	COMM_FAILED_TOFORCE_FX2_CPU_INTO_RESET = -56,		

	/*Failed to load Vend_Ax firmware*/
	COMM_FAILED_TO_LOAD_VEND_AX = -57,			

} COMM_ERROR_CODES;

typedef enum _COMM_DUTY_CYCLE_LEVEL
{
    /** Duty cycle Off */
    COMM_DUTY_CYCLE_OFF = 0,

    /** Duty cycle level 1 - which corresponds to value 06% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL1 = 1,

    /** Duty cycle level 2 - which corresponds to value 12% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL2 = 2,

    /** Duty cycle level 3 - which corresponds to value 17% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL3 = 3,

    /** Duty cycle level 4 - which corresponds to value 23% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL4 = 4,
    
    /** Duty cycle level 5 - which corresponds to value 28% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL5 = 5,
    
    /** Duty cycle level 6 - which corresponds to value 34% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL6 = 6,

    /** Duty cycle level 7 - which corresponds to value 39% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL7 = 7,
    
    /** Duty cycle level 8 - which corresponds to value 45% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL8 = 8,
    
    /** Duty cycle level 9 - which corresponds to value 50% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL9 = 9,

    /** Duty cycle level 10 - which corresponds to value 56% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL10 = 10,

    /** Duty cycle level 11 - which corresponds to value 61% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL11 = 11,

    /** Duty cycle level 12 - which corresponds to value 66% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL12 = 12,

    /** Duty cycle level 13 - which corresponds to value 72% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL13 = 13,
    
    /** Duty cycle level 14 - which corresponds to value 77% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL14 = 14,

    /** Duty cycle level 15 - which corresponds to value 83% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL15 = 15,   /* Default Duty Cycle value     */

    /** Duty cycle level 16 - which corresponds to value 88% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL16 = 16,

    /** Duty cycle level 17 - which corresponds to value 94% Duty Cycle */
    COMM_DUTY_CYCLE_LEVEL17 = 17,
} COMM_DUTY_CYCLE_LEVEL;

/************************ Functions ******************************************/

/**
 * @brief CommInitLibrary : API to initialize communication library and resources.
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommInitLibrary(void);

/**
 * @brief COMM_EVT_CALLBACK : Callback event function. It will post all kinds 
 *                          of events generated from communication library to
 *                          calling module.
 * @param eEventType : Event type from COMM_LIB_EVENT.
 * @param status : 0 if success else negative value. 
 *                  Do no check evtData if status is negative.
 * @param *pEventData : Event data, which depends on event type.
 *
 *	--------------------  -------------------  -----------------------------------
 *      Event Type			Error Code			   Event Data (Pointer of)
 *  --------------------  -------------------  -----------------------------------
 *  COMM_EVT_PREVIEW_CB     Success or Error	COMM_IMAGE_PREVIEW_PARAMS structure
 *
 *     Memory allocation and deallocation will be done by library itself.
 *     Host App MUST only copy data from given buffer in this call back function 
 *     and leave this function ASAP. No other processing should be done in this 
 *     function. If any thing else is done, this can cause degraded performance 
 *      and malfunctioning.
 *
 * @return : void
 */
typedef void (CommLibCall *COMM_EVT_CALLBACK) (const COMM_LIB_EVENT eEventType, const int status,
                                    const void *pEventData);

/**
 * @brief CommRegisterCallback : API to register call back for getting all events.
 * @param pEventCallBack : Pointer to callback function to get COMM_LIB_EVENT.
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommRegisterCallback(const COMM_EVT_CALLBACK *pEventCallBack);

/**
 * @brief CommGetLibVersion : API to get lib version.
 * @param pVersion (out) : Pointer to receive communication library version.
 * 
 *  		Size of the buffer must be COMM_LIB_VER_MAX_LEN.
 * 
 * Calling module will be responsible to allocate and deallocate memory for this.
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommGetLibVersion(char *pVersion);

/**
 * @brief CommIsDeviceConnected : API to check if device is connected or not.
 *
 *                      This API can be called even before CommInitDevice.
 *
 * @param fd (in) : FD passed to open device on USB interface
 *                          (only for Android - for Windows and Linux pass (-1))
 * @param pVendorId (in) : Vendor Id of the device. Null terminated String.
 * @param pProductId (in) : Product Id of the device. Null terminated String.
 *
 * @return 0 - if device is connected, if not connected respective error code, for error also negative.
 */
int CommLibCall CommIsDeviceConnected(const int fd, const char *pVendorId, const char *pProductId);

/**
 * @brief CommInitDevice : API to initialize device.
 * @param fd (in) : fd (File Descriptor) passed to open device on USB interface.
 *                  (only for Android - For Windows and Linux pass -1.)
 * @param pVendorId (in) : Vendor Id of the device. Null terminated String.
 * @param pProductId (in) : Product Id of the device. Null terminated String.
 *
 * @return 0 - success, negative - failure.
 *
 */
int CommLibCall CommInitDevice(const int fd, const char *pVendorId, const char *pProductId,	COMM_DEVICE_FPS eFPS);

/**
 * @brief CommStartCapture : API to send start capture to device.
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommStartCapture();

/**
 * @brief CommStopCapture : API to stop capture to device.
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommStopCapture(void);

/**
 * @brief CommGetMFGData : API to get MFG data of device.
 *
 *
 * @param pIrisBuffer (out) : Pointer to receive MFG data. 
 *                      Memory allocation and deallocation is responsibility 
 *                      of calling module. Size is COMM_MFS_MFG_DATA_LEN_MAX.
 *
 * @param pIrisBufferSize (out) : Pointer to receive size of databuffer
 *                      Memory allocation and deallocation is responsibility 
 *                      of calling module.
 *
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommGetMFGData(unsigned char *pIrisBuffer, unsigned int *pIrisBufferSize);


/**
* @brief CommRestoreMFGData : API to restore MFG data in device.
*
* @return 0 - success, negative - failure.
*/
int CommLibCall CommRestoreMFGData();

/**
 * @brief CommGetErrorDescription : API to get description of errors got as 
 *                              return value of a function.
 * @param errNo (in) : Error no returned from any of the APIs from the library.
 *
 * @return : Error description.
 */
const char* CommLibCall CommGetErrorDescription(const int errNo);

/**
 * @brief CommEnableLogs : API to enable/disable the logging level.
 * @param eLogLevel (in) : Log level of type COMM_LOG_LEVEL.
 *
 *  To disable log, pass eLogLevel value as COMM_LEVEL_LOG_OFF.
 *
 * @param pDirPath (in) : Path of log directory. If this argument is NULL, 
 *              logs will be written in current directory.
 * 
 * Log filename will be "Comm_YYYY-MM-DD-HH-MM.log". 
 *
 * If 'pDirPath' does not exist, this API will create the directory.
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommEnableLogs(const COMM_LOG_LEVEL eLogLevel, const char *pDirPath);

/**
* @brief CommSetVISLed : API to set VIS Led status.
 *
 * @param ledStatus (in) : 1 to turn led ON. 0 to turn led OFF.

 * @param LEDValue: Set LEDValue.
 *
 * @return 0 - success, negative - failure
 */
int CommLibCall CommSetVISLed(const unsigned int ledStatus,int LEDValue);

/**
 * @brief CommSetIRLed : API to update IR Led Duty cycle.
 *
 * @param eDutyCycleLevel (in) : Duty cycle level of type COMM_DUTY_CYCLE_LEVEL.
 *
 *      To turn off the Led, pass level value COMM_DUTY_CYCLE_OFF.
 *      To set any other value, pass level value between
 *                  COMM_DUTY_CYCLE_LEVEL1 ......... COMM_DUTY_CYCLE_LEVEL17.
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommSetIRLed(const COMM_DUTY_CYCLE_LEVEL eDutyCycleLevel);

/**
 * @brief CommUninitDevice: API to uninitialize the device.
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommUninitDevice(void);

/**
 * @brief CommUninitLibrary : API to uninitialize library and cleanup resources.
 *
 *  Note : This API must be called only after CommUninitDevice().
 *
 * @return 0 - success, negative - failure.
 */
int CommLibCall CommUninitLibrary(void);

//*********************************************************************************************************************
// CommReadFirmwareVersionInfo
//
//! Reads firmware version details from Flash memory.
//!
//! \param *out_pFirmwareVersionDetails : Pointer to Version details. Size of the buffer must be COMM_FIRMWARE_VERSION_INFO_LENGTH.
//!									
//!                 Host application will be responsible to allocate and deallocate memory for this.
//!									
//!	\param *out_pFirmwareVersionDetailsLen : Length of the retrieved information.
//!
//!                 Host application will be responsible to allocate and deallocate memory for this.
//! 
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int CommLibCall CommReadFirmwareVersionInfo(unsigned char *out_pFirmwareVersionDetails, int *out_pFirmwareVersionDetailsLen);

//*********************************************************************************************************************
// CommReadFirmwareVersion
//
//! Reads firmware version details from Flash memory.
//!
//! \param *out_pFirmwareVersionDetails : Pointer to Version details. Size of the buffer must be COMM_FIRMWARE_VERSION_INFO_LENGTH.
//!									
//!                 Host application will be responsible to allocate and deallocate memory for this.
//!									
//!	\param *out_pFirmwareVersionDetailsLen : Length of the retrieved information.
//!
//!                 Host application will be responsible to allocate and deallocate memory for this.
//! 
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int CommLibCall CommReadFirmwareVersion(unsigned char *out_pFirmwareVersionDetails, int *out_pFirmwareVersionDetailsLen);


//*********************************************************************************************************************
// CommLoadFirmware
//
//! This function reads FW Version information.
//!
//!	\param *in_FirmwareData : Pointer to buffer of firmware data.Firmware data has not in used.
//!
//!                    Host application will be responsible to allocate and deallocate memory for this buffer.
//!
//!	\param in_FirmwaredataLen : Length of the input firmware data.
//!  
//!                    Host application will be responsible to allocate and deallocate memory for this buffer.
//!
//!	\param in_FirmwareVersion : Version of firmware data.
//!  
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************

int CommLibCall CommLoadFirmware(unsigned char* in_FirmwareData, int in_FirmwaredataLen, int in_FirmwareVersion);

#ifdef DEVELOPMENT_MODE
	/**
	* @brief Dev_CommGetFPS : API to get FPS of Iris.
	*
	* @param pFPS (out) : Pointer to receive Iris FPS.
	* 			Calling module will be responsible to allocate and deallocate
	*           memory for this.
	*
	* 			Calling module will be responsible to allocate and deallocate
	*           memory for this.
	*
	* @return 0 - success, negative - failure.
	*/
	int CommLibCall Dev_CommGetFPS(float *pFPS);

	/**
	* @brief Dev_CommSetMFGData : API to set manufacturing data in device.
    *
    * @param pDataBuffer (in) : Pointer to Iris MFG data. 
    *                      Memory allocation and deallocation is responsibility 
    *                      of calling module. Size is COMM_MFS_MFG_DATA_LEN_MAX.
    *
    * @param bufferSize (in) : Data buffer size.
    *
    * @return 0 - success, negative - failure.
	*/
	int CommLibCall Dev_CommSetMFGData(const unsigned char *pDataBuffer, const unsigned int bufferSize);

	/**
	* @brief CommStartCapture : API to send start capture to device.
	*
	* @return 0 - success, negative - failure.
	*/
	int CommLibCall Dev_CommStartCaptureForTarget();

#endif //#ifdef DEVELOPMENT_MODE

#pragma pack(pop)       //Back to whatever the previous packing mode was 

#if defined (__cplusplus)
}
#endif

#endif // __MSP0045_COMM_LIB_H__
