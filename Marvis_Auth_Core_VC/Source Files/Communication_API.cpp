#include <string>
#include "..\Header Files\Communication_API.h"



//*********************************************************************************************************************
// MBSInitLibrary
//
//! Initializes the Interface Library. Upon successful initialization, it will receive Core (Interface Library) handle, 
//! which will be used by Host (Caller) application in subsequent functions calls.
//!
//! This function will also need Callback table structure, which will be later on used by Interface Library to invoke 
//! specific functions into Host application.
//!
//! This function will also need Refcon pointer, which will be passed back by Interface Library during callback.
//!
//! \param None.
//!
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int  InitComLibrary()
{
	int returnValue = MBS_E_SUCCESS;
    try
    {
        returnValue = MBSInitLibrary();
    }
    catch (...)
    {
        returnValue = MBS_E_MEMORY;
    }
    return returnValue;
}

//*********************************************************************************************************************
// MBSGetSDKVersion
//
//! This function queries Interface Library version.
//!
//! \param none
//! 
//! \return Interface Library version.
//! 
//*********************************************************************************************************************
void  GetSDKVersion(char *szCommLibVer)
{
    MBSGetCommLibVersion(szCommLibVer);
	//std::string strVersion(szCommLibVer);
	//memcpy(szCommLibVer1,szCommLibVer,sizeof(szCommLibVer));
   
}


//*********************************************************************************************************************
// InitDevice
//
//! It initializes the device (specified by Vendor ID and Product ID).
//!
//! \param controllerVendorID : Device Controller's Vendor ID.
//! \param controllerProductID : Device Controller's Product ID.
//! \param out_pDeviceData : Pointer to receive device data of type MBS_DEVICE_DATA.
//! 
//!       Host application will be responsible to allocate and deallocate memory for out_pDeviceData buffer.
//! 
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************

int  InitDevice(const int nVendorID, const int nProductID, 
							int nFD, MBS_DEVICE_FPS eFPS, 
							unsigned char *out_pSymmEncKey, int *out_pSymmEncKeyLen)
{
    int returnValue = MBS_E_SUCCESS;
    returnValue = MBSInitDevice(nVendorID, nProductID,nFD, eFPS,out_pSymmEncKey, out_pSymmEncKeyLen);
    return returnValue;
}

int GetDeviceInbuiltSerialNo(unsigned char *out_pSerialNo)
{
    int returnValue = MBS_E_SUCCESS;

    if (!out_pSerialNo)
    {
        returnValue = MBS_E_NULL_PARAM;
    }
    else
    {
			returnValue = MBSGetDeviceInbuiltSerialNo(out_pSerialNo);
	}

    return returnValue;
}

//*********************************************************************************************************************
// StartCapture
//
//! Starts capturing the image.
//!
//! \param dataResponseMode : Image data response mode of type MBS_DATA_RESPONSE_MODE.
//! \param *pQualityParams : Pointer of MBS_DEVICE_QUALITY_PARAMS structure which contains quality parameters.
//!									
//!			                Host application will be responsible to allocate and deallocate memory for this.
//!
//! \param *pCallbackTable : Pointer to structure of Callback table.
//! \param bFetchFMR : Boolean (true/false) flag indicating if FMR should be fetched or not.
//! \param *pFMR : Pointer to receive FMR data. Size of the buffer should be equal or less than MBS_FMR_MAX_LENGTH.
//!                 Host application will be responsible to allocate and deallocate memory for this.
//!
//!             If 'bFetchFMR' flag is 'false', this parameter can be NULL.
//!
//!	\param *out_pActualFMRBytes : Pointer to receive actual FMR bytes filled up in 'pFMR' buffer.
//!                 Host application will be responsible to allocate and deallocate memory for this.
//!
//!             If 'bFetchFMR' flag is 'false', this parameter can be NULL.
//!
//! \param *pQualityInfo : Pointer to receive Quality Info. Size of the buffer should be of MBS_FMR_QUALITY_INFO_LENGTH.
//!                 Host application will be responsible to allocate and deallocate memory for this.
//!
//!             If 'bFetchFMR' flag is 'false', this parameter can be NULL.
//! 
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int StartCapture()
{
    int returnValue = MBS_E_SUCCESS;
	returnValue = CommStartCapture();
    return returnValue;
}

//*********************************************************************************************************************
// MBSStopCapture
//
//! It stops image capturing.
//!
//! \param : None.
//!
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int  StopCapture()
{
    int returnValue = MBS_E_SUCCESS;
    returnValue = MBSStopCapture();
    return returnValue;
}

//*********************************************************************************************************************
// MBSGetMFGData
//
//! Gets device manufacturing data from the device. Size of the buffer must be MFG_DATA_ENCRYPTED_LENGTH.
//!
//! \param *out_pMfgData : Pointer of MBS_DEVICE_MANUFACTURING_DATA structure to receive manufacturing data.
//!									
//!			                Host application will be responsible to allocate and deallocate memory for this.
//! 
//! \param *out_pMfgDataLen : Pointer to receive Manufacturing data length.
//! 
//!			         Host application will be responsible to allocate and deallocate memory for this.
//! 
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int GetMFGData(unsigned char *out_pMfgData, int *out_pMfgDataLen)
{
    int returnValue = MBS_E_SUCCESS;

    if (!out_pMfgData || !out_pMfgDataLen)
    {
        returnValue = MBS_E_NULL_PARAM;
    }
    else
    {
        returnValue = CommGetMFGData(out_pMfgData, out_pMfgDataLen);
    }

    return returnValue;
}


//*********************************************************************************************************************
// MBSSoftReset
//
//! It does soft reset on the device.
//!
//! \param None.
//! 
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int  SoftReset()
{
    int returnValue = MBS_E_SUCCESS;
    returnValue = MBSSoftReset();
    return returnValue;
}

//*********************************************************************************************************************
// MBSGetErrorDescription
//
//! Gets error description for the given error code.
//!
//! \param errorCode : Error code for which description is needed.
//! \param *pErrorDescription : Pointer to receive error description. Size of Buffer must be MBS_ERROR_DESC_MAX_LENGTH.
//!
//!                 Host application will be responsible to allocate and deallocate memory for pErrorDescription buffer.
//! 
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int  GetErrorDescription(const int errorCode, char *pErrorDescription)
{
    int returnValue = MBS_E_SUCCESS;

    if (!pErrorDescription)
    {
        returnValue = MBS_E_NULL_PARAM;
    }
    else
    {
            returnValue = MBSGetErrorDescription(errorCode, pErrorDescription);
    }

    return returnValue;
}

//*********************************************************************************************************************
// MBSUninitLibrary
//
//! Uninitializes Interface Library.
//!
//! \param None.
//! 
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int  UninitLibrary()
{
    int returnValue = MBS_E_SUCCESS;
	returnValue =  MBSUninitLibrary();
    return returnValue;
}

//*********************************************************************************************************************
// MBSEnableLogging
//!
//! Enable log feature for the FingerdeviceAuthlibrary
//!
//! \param *pLogFilePath : Log file name with full path.
//!
//!                  Host application may pass log file name with its target location(e.g \\direcory path\\filename.log").
//!
//!                  If NULL is passed then, default log file will get created.
//!
//! \return Sucess or Failure code.
//!
//*********************************************************************************************************************
int EnableLogging(MBS_LOG_LEVEL logLevel,const char *pLogfilePath)
{
    int returnValue = MBS_E_SUCCESS;
	MBSSetLogProperties(logLevel,pLogfilePath);
    return returnValue;
}


//*********************************************************************************************************************
// MBSUninitDevice
//
//! Uninitializes the device.
//!
//! \param None.
//! 
//! \return Success or Failure code.
//! 
//*********************************************************************************************************************
int  UninitDevice()
{
    int returnValue = MBS_E_SUCCESS;
    returnValue = MBSUninitDevice();
    return returnValue;
}


