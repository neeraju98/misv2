#ifndef __NBIS_H__
#define __NBIS_H__

// Nbis.h


namespace Nbis
{
	typedef enum _Finger 
	{
		LeftOrRightSlap = 0,
		RightThumb = 1,
		RightIndex = 2,
		RightMiddle = 3,
		RightRing = 4,
		RightLittle = 5,
		LeftThumb = 6,
		LeftIndex = 7,
		LeftMiddle = 8,
		LeftRing = 9,
		LeftLittle = 10,	
		RightSlap = 13,
		LeftSlap = 14,
		BothThumbs = 23
	} Finger;

	typedef enum _PixelFormat 
	{
		Format8bppIndexed = 8,
		Format16bppGrayScale = 16,
	} PixelFormat;

	class Nfiq 
	{
	public:
		static int FromFile(unsigned char* pLicenseKey, char* pFileName);
		static int FromBitmap(unsigned char* pLicenseKey, unsigned char* pBitmapData, int nImageWidth, int nImageHeight, 
							int nImageDepth, int nPixelsPerInch);
		static int FromWsq(unsigned char* pLicenseKey, unsigned char* pWsqByteArray, int nWsqDataLength);

		static int FromRawData(unsigned char* image_data,int image_width,int image_height,int image_depth, int pixels_per_inch);

		static const int NFIQ_E_SUCCESS = 0;                    //	Success
		static const int NFIQ_E_UNSUPPORTED_FILE_FORMAT = -1;	//	Unsupported file format
		static const int NFIQ_E_INVALID_PARAM = -2;				//	Invalid Parameter
		static const int NFIQ_E_NULL_PARAM = -3;				//	Null Parameters
		static const int NFIQ_E_INVALID_LICENSE_KEY = -4;		//	Invalid license key
		static const int NFIQ_E_FILE_DOES_NOT_EXIST = -5;		//	File does not exist
		static const int NFIQ_E_FAILED_TO_LOAD_FILE = -6;		//	Failed to load file
	};

	class Shared 
	{
	public:
		static char* GetFileExtension(const char* pFileName);
		static int DetermineImageDepth(PixelFormat pixedFormat);
		static bool LoadFileIntoBitmap(const char* pFileName, HBITMAP& hBitmap, BITMAP& bitmap);
	};
};

#endif  //__NBIS_H__