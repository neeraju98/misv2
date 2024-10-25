

#ifndef __FingerPositionDetect_H__
#define __FingerPositionDetect_H__

#ifdef __cplusplus 
extern "C" {
#endif 

#define Dllexport __declspec(dllexport)

	typedef unsigned char      uint8_t;
	typedef unsigned short     uint16_t;
	typedef unsigned int       uint32_t;
	typedef unsigned long long uint64_t;
	
	typedef enum _Position_
	{
		OK, LEFT, RIGHT, TOP, NOT_IN_BOTTOM, NOT_OK
	}Position;

	typedef enum _POSITION_ERROR_CODE_ {

		POSITION_SUCCESS = 0,
		POSITION_E_EXCEPTION = -204,
		POSITION_E_IMG_DATA_NULL = -205,
		POSITION_E_HEIGHT_NON_POSITIVE = -206,
		POSITION_E_WIDTH_NON_POSITIVE = -207,
		POSITION_E_MEAN_VALUE_WRONG = -208,
		POSITION_E_FAIL = -209,
		POSITION_E_PARAMETER_NULL_OR_ZERO = -210,

	} POSITION_ERROR_CODE;

	typedef struct _POSTION_OFFSET {
		int left;
		int leftoffset;
		int leftPercent;
		int right;
		int rightoffset;
		int rightPercent;
		int top;
		int topoffset;
		int topPercent;
		int bottom;
		int bottomoffset;
		int bottomPercent;
		int upperpart;
		int upperpartPercent;
		int middlepart;
		int middlepartPercent;
		int lowerpart;
		int lowerpartPercent;
	}POSTION_OFFSET;

	/**
	* @brief MIDFinger_Auth_GetErrDescription: This API will return error description for the provided error code.
	* @param errorNo[In]: Error code for which error description is required.
	* @return : 0 in case of success, other for failed or error.
	*/
	Dllexport const char *MEngin_GetErrDescription(int errorNo);

	/* MEngin_GetFingerPosition to get fingure position */
	Dllexport int MEngin_GetPosition(unsigned char *in_rawImg, int in_width, int in_height, Position&outPosition, POSTION_OFFSET*in_offsetvalue);

	Dllexport int MEngin_CropFindQuality(uint8_t* frambuffer, uint8_t* QualityIndex, uint16_t FramWidth, uint16_t QualityCrop_W, uint16_t QualityCrop_H,
		uint16_t QualityWindow_W, uint16_t QualityWindow_H, uint16_t QualityCrop_W_Index, uint16_t QualityCrop_H_Index, uint8_t QualityThMax, uint8_t QualityThMin);

	Dllexport int MEngin_GetImageSimilarityScore(const unsigned char* rawData, unsigned char* prev_frame, int width, int height, int histOffset, float &out_score);

#ifdef __cplusplus 
}
#endif 

#endif


