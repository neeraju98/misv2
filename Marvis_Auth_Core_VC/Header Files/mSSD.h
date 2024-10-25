#pragma once

#ifdef __cplusplus 
extern "C" {
#endif 

#ifndef MSSDLIB_EXPORTS
#define MSSDLIB_API __declspec(dllexport)
#else
#define MSSDLIB_API __declspec(dllimport)
#endif

	typedef enum _modeltype_ {
		Teachable_Machine = 0,
		Python_Manual = 1,

	}modeltype;

	MSSDLIB_API int MSSD_CheckLiveness(char* in_modelBuffer, int in_modelBufferLen, bool IsModelLoadFromFile,
		char* imageBuffer, int imgWidth, int imgHeight, bool* out_IsLive, float* out_score, int num_results = 2);

	MSSDLIB_API int MSSD_CheckLiveness_imgPreprocess(char* in_modelBuffer, int in_modelBufferLen, bool IsModelLoadFromFile,
		char* imageBuffer, int imgWidth, int imgHeight, int* out_IsLive, float* out_score, int num_results = 2, modeltype mtype = Teachable_Machine);

	MSSDLIB_API int MSSD_CheckLoadModel(char* in_modelBuffer, int in_modelBufferLen);

	MSSDLIB_API const char* MSSD_GetErrDescription(int errorNo);

	typedef enum _MSSDLIB_ERROR_CODE_ {

		MSSD_SUCCESS = 0,
		MSSD_EXCEPTION = -7101,
		MSSD_NULL_PARAM = -7102,
		MSSD_FAILED_TO_LOAD_MODEL = -7103,
		MSSD_FAILED_TO_READ_FILE = -7104,
	}MSSDLIB_ERROR_CODE;
#ifdef __cplusplus 
}
#endif 