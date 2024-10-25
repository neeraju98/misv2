#ifndef _OPJ_COMPRESS_H_
#define _OPJ_COMPRESS_H_

#ifdef __cplusplus
extern "C" {
#endif

#define MIDFINGER __declspec(dllexport)


	typedef enum _MIDFINGER_ENROLL_DECODE_FORMAT
	{
		PXM = 10,
		PGX = 11,
		BMP1 = 12,
		YUV = 13,
		TIF = 14,
		RAW1 = 15,
		TGA = 16

	} MIDFINGER_ENROLL_DECODE_FORMAT;


	MIDFINGER int jpeg2k_encode_BMP(const char* FilePath, unsigned char *out_pEncodedData, int *out_pEncodedBufferSize, int CompressionRatio);
	MIDFINGER int jpeg2k_decode_mem(unsigned char *pBuffer, int pBufferLen, unsigned char *out_FilePath);
	MIDFINGER int jpeg2k_encode_BMP_buffer(unsigned char *pBuffer, int pBufferLen, unsigned char *out_pEncodedData, int *out_pEncodedBufferSize, int CompressionRatio);
	MIDFINGER int jpeg2k_decode_buffer(unsigned char *pBuffer, int pBufferLen, MIDFINGER_ENROLL_DECODE_FORMAT Dformat, unsigned char *out_pDecodedData, int *out_pDecodedDataSize);
	MIDFINGER int jpeg2k_encode_mem(unsigned char *pRawBuffer, const int nWidth, const int nHeight,
		unsigned char *out_pEncodedData, int *out_pEncodedBufferSize,
		int CompressionRatio);


	static const int E_SUCCESS = 0;						//Success
	static const int E_INVALID_RAW_BUFFER_DATA = -1;	//Invalid Raw Buffer Data
	static const int E_INVALID_PARAM = -2;				//Invalid Parameter
	static const int E_NULL_PARAM = -3;					//Null Parameters
	static const int E_MEMORY_ALLOCATION = -4;			//Memory allocation failed
	static const int E_FAILED_TO_ENCODE_DATA = -5;		//JPEG2K Encoding failed
	static const int E_FAILED_TO_DECODE_DATA = -6;		//JPEG2K Encoding failed
	static const int E_OTHER = -7;						//Some other error occurred

#ifdef __cplusplus
} //extern "C" {
#endif

#endif /* _OPJ_COMPRESS_H_ */