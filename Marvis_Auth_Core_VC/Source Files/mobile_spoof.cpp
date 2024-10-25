#include <string>
#include <fstream>
#include <cmath>


#define HOR_PER        100//50
#define VER_PER        100//70

using namespace std;

unsigned char *highRawImage;

int gblWidth = 0;
int gblHeight = 0;
int newWidth = 0;
int newHeight = 0;

int xw1;
int xw2;
int yh1;
int yh2;

unsigned char *newRaw;
unsigned char *out_frame;

float roundD(double var) {
    auto value = (float) (var * 100 + .5);
    return (float) value / 100;
}

int allocateBuffer(int width, int height) {
    highRawImage = new unsigned char[width * height];
    memset(highRawImage, 255, width * height);

    gblWidth = width;
    gblHeight = height;

    newWidth = (HOR_PER * width) / 100;
    newHeight = (VER_PER * height) / 100;

    xw1 = (gblWidth / 2) - (newWidth / 2);
    xw2 = xw1 + newWidth;
    yh1 = (gblHeight / 2) - (newHeight / 2);
    yh2 = yh1 + newHeight;

//    LOG_ERROR("xw1: %d, yh1: %d, newWidth: %d, newHeight: %d", xw1, yh1, newWidth, newHeight);
    newRaw = new unsigned char[newHeight * newWidth];
    memset(newRaw, 0, newHeight * newWidth);

	out_frame = new unsigned char[newHeight * newWidth];
	memset(out_frame, 0, newHeight * newWidth);

    return 0;
}

float getBlur() {

	float blurScore = 0.0;
	try
	{
		unsigned char *blurRaw = (unsigned char *)calloc((newHeight * newWidth) + 1, sizeof(unsigned char));

		//Blur Logic
		short laplase_mask[3][3] = {
			{ 0, 1, 0 },
			{ 1, -4, 1 },
			{ 0, 1, 0 } };

		int len = newWidth * newHeight;
		float sum;

		//Laplase
		for (int i = 0; i < newHeight; i++) {
			for (int j = 0; j < newWidth; j++) {
				blurRaw[(i * newWidth) + j] = 0;
				sum = 0;

				for (int a = -1; a < 2; a++) {
					for (int b = -1; b < 2; b++) {
						if ((i + a) >= 0 && (j + b) >= 0 && (i + a) < newHeight && (j + b) < newWidth)
							sum = sum + newRaw[((i + a) * newWidth) + (j + b)] * laplase_mask[a + 1][b + 1];
					}
				}

				if (sum > 255) sum = 255;
				if (sum < 0) sum = 0;

				if (sum > blurRaw[(i * newWidth) + j]) {
					blurRaw[(i * newWidth) + j] = sum;
				}

			}
		}

		sum = 0;
		float mean;
		float variance;
		//float deviation;

		for (int i = 0; i < len; i++)
			sum = sum + blurRaw[i];
		mean = sum / len;

		sum = 0;
		for (int i = 0; i < len; i++) {
			sum = sum + pow((blurRaw[i] - mean), 2);
		}
		variance = sum / len;
		blurScore = roundD(variance);

		std::free(blurRaw);
		blurRaw = NULL;
	}
	catch (...)
	{
		blurScore = -1;
	}
	
	return blurScore;
}

bool isBlurImage() {
	//Blur Logic
	short laplase_mask[3][3] = {
		{ 0, 1, 0 },
		{ 1, -4, 1 },
		{ 0, 1, 0 } };

	int len = newWidth * newHeight;
	float sum;

	//Laplase
	for (int i = 0; i < newHeight; i++) {
		for (int j = 0; j < newWidth; j++) {
			out_frame[(i * newWidth) + j] = 0;
			sum = 0;

			for (int a = -1; a < 2; a++) {
				for (int b = -1; b < 2; b++) {
					if ((i + a) >= 0 && (j + b) >= 0 && (i + a) < newHeight && (j + b) < newWidth)
						sum = sum + newRaw[((i + a) * newWidth) + (j + b)] * laplase_mask[a + 1][b + 1];
				}
			}

			if (sum > 255) sum = 255;
			if (sum < 0) sum = 0;

			if (sum > out_frame[(i * newWidth) + j]) {
				out_frame[(i * newWidth) + j] = sum;
			}

		}
	}

	sum = 0;
	float mean;
	float variance;
	//float deviation;

	for (int i = 0; i < len; i++)
		sum = sum + out_frame[i];
	mean = sum / len;

	sum = 0;
	for (int i = 0; i < len; i++) {
		sum = sum + pow((out_frame[i] - mean), 2);
	}
	variance = sum / len;

	//deviation = sqrt(variance);
	float blur = roundD(variance);
	//    *blurScore = blur;
	if (blur > 1500) {
		return false;
	}
	else {
		return true;
	}
	//    return false;
}

float getMeanOld(const unsigned char *rawData) {
    memset(newRaw, 0, newHeight * newWidth);
    double sum = 0;
    int cnt = 0;
    for (int i = yh1; i < yh2; ++i) {
        for (int j = xw1; j < xw2; ++j) {
            sum += rawData[(gblWidth * i) + j];
            newRaw[cnt] = rawData[(gblWidth * i) + j];
            cnt++;
        }
    }

    float overAllMean = roundD(sum / (newWidth * newHeight));

	return overAllMean;

    /*
        48 block (21*26)
        W: 316, After 40%, W: 126 = 21*6
        H: 354, After 60%, H: 212 = 26*8 + 4
    */
    int blockWidth = 21;
    int blockHeight = 26;
    int blockLen = blockWidth * blockHeight;

    int meanSumCount = 0;
    float meanSum = 0;
    for (int y = 0; y < 8; ++y) {
        int yLoc = blockHeight * y;
        for (int x = 0; x < 6; ++x) {
            int xLoc = blockWidth * x;

            double blockSum = 0;
            int cnt1 = 0;
            for (int i = yLoc; i < yLoc + blockHeight; i++) {
                for (int j = xLoc; j < xLoc + blockWidth; j++) {
                    blockSum += newRaw[(newWidth * i) + j];
                    cnt1++;
                }
            }
            float blockMean = roundD(blockSum / blockLen);
            if (blockMean <= overAllMean && (blockMean > 8 && blockMean < 250)) {
                meanSum += blockMean;
                meanSumCount++;
            }
        }
    }

    if (meanSumCount > 5) {
        return roundD(meanSum / (float) meanSumCount);
    } else {
        return 0;
    }
}

float getMean(const unsigned char* rawData) {
	memset(newRaw, 0, newHeight * newWidth);
	double sum = 0;
	int cnt = 0;
	for (int i = yh1; i < yh2; ++i) {
		for (int j = xw1; j < xw2; ++j) {
			sum += rawData[(gblWidth * i) + j];
			newRaw[cnt] = rawData[(gblWidth * i) + j];
			cnt++;
		}
	}

	float overAllMean = roundD(sum / cnt);

	/*int totalSum = 0;
	for (size_t i = 0; i < newHeight; i++)
	{
		for (size_t j = 0; j < newWidth; j++)
		{
			totalSum += newRaw[(i * newWidth) + j];
		}
	}

	float out_meanVal = totalSum / (newWidth * newHeight);*/

	return overAllMean;
}

int processLiveness(unsigned char *rawData) {
    int ret = -1;

    memcpy(highRawImage, rawData, gblWidth * gblHeight);
    float mean = getMean(highRawImage);
    if (mean > 8 && mean < 250) {
        ret = 0;
    }
    return ret;
}

void freeBuffer() {
    if (newRaw != NULL) {
        delete[] newRaw;
		newRaw = NULL;
    }
	if (out_frame != NULL) {
		delete[] out_frame;
		out_frame = NULL;
	}
    if (highRawImage != NULL) {
        delete[] highRawImage;
		highRawImage = NULL;
    }
}

float getBlur_inputImg(unsigned char * src_raw, int src_width, int src_height) {

	float blurScore = 0.0;
	try
	{
		//newWidth = src_width;
		//newHeight = src_height;
		unsigned char* newRaw_blur = (unsigned char*)calloc((newHeight * newWidth) + 1, sizeof(unsigned char));
		int src_cnt = 0;
		for (int i = yh1; i < yh2; ++i) {
			for (int j = xw1; j < xw2; ++j) {
				newRaw_blur[src_cnt] = src_raw[(src_width * i) + j];
				src_cnt++;
			}
		}

		unsigned char* blurRaw = (unsigned char*)calloc((newHeight * newWidth) + 1, sizeof(unsigned char));

		//Blur Logic
		short laplase_mask[3][3] = {
			{ 0, 1, 0 },
			{ 1, -4, 1 },
			{ 0, 1, 0 } };

		int len = newWidth * newHeight;
		float sum;

		//Laplase
		for (int i = 0; i < newHeight; i++) {
			for (int j = 0; j < newWidth; j++) {
				blurRaw[(i * newWidth) + j] = 0;
				sum = 0;

				for (int a = -1; a < 2; a++) {
					for (int b = -1; b < 2; b++) {
						if ((i + a) >= 0 && (j + b) >= 0 && (i + a) < newHeight && (j + b) < newWidth)
							sum = sum + newRaw_blur[((i + a) * newWidth) + (j + b)] * laplase_mask[a + 1][b + 1];
					}
				}

				if (sum > 255) sum = 255;
				if (sum < 0) sum = 0;

				if (sum > blurRaw[(i * newWidth) + j]) {
					blurRaw[(i * newWidth) + j] = sum;
				}

			}
		}

		sum = 0;
		float mean;
		float variance;
		//float deviation;

		for (int i = 0; i < len; i++)
			sum = sum + blurRaw[i];
		mean = sum / len;

		sum = 0;
		for (int i = 0; i < len; i++) {
			sum = sum + pow((blurRaw[i] - mean), 2);
		}
		variance = sum / len;
		blurScore = roundD(variance);

		std::free(blurRaw);
		std::free(newRaw_blur);
		blurRaw = NULL;
	}
	catch (...)
	{
		blurScore = -1;
	}

	return blurScore;
}

float getMean_cropArea(unsigned char* rawData, int rawWidth, int rawHeight, int xw1, int yh1, int xw2, int yh2) {
	double sum = 0;
	int cnt = 0;
	for (int i = yh1; i < yh2; ++i) {
		for (int j = xw1; j < xw2; ++j) {
			sum += rawData[(rawWidth * i) + j];
			//rawData[(rawWidth * i) + j] = 0;
			cnt++;
		}
	}

	float overAllMean = roundD(sum / cnt);

	return overAllMean;
}

float getMean_FromImage(const unsigned char* rawData, int rawWidth, int rawHeight) {
	double sum = 0;
	int cnt = 0;
	for (int i = 0; i < rawHeight; ++i) {
		for (int j = 0; j < rawWidth; ++j) {
			sum += rawData[(rawWidth * i) + j];
			cnt++;
		}
	}

	float overAllMean = roundD(sum / cnt);

	return overAllMean;
}

void getMeanStd_cropArea(const unsigned char* rawData, int rawWidth, int rawHeight, int xw1, int yh1, int xw2, int yh2, float& meanValue, float& stdValue)
{
	double sum = 0;
	int cnt = 0;
	for (int i = yh1; i < yh2; ++i) {
		for (int j = xw1; j < xw2; ++j) {
			sum += rawData[(rawWidth * i) + j];
			cnt++;
		}
	}

	meanValue = roundD(sum / cnt);

	int len = ((yh2 - yh1) * (xw2 - xw1));

	float variance = 0;
	sum = 0;
	for (int i = 0; i < len; i++) {
		sum = sum + pow((rawData[i] - meanValue), 2);
	}
	variance = sum / len;

	stdValue = sqrt(variance);
}

void getCropAreaDetail(unsigned char* out_rawData, int& rawWidth, int& rawHeight)
{
	memcpy(out_rawData, newRaw, newWidth * newHeight);
	rawWidth = newWidth;
	rawHeight = newHeight;
}

int GetMeanAndStddev(unsigned char* in_inputImageData, int in_Width, int in_Height, double* out_meanValue, double* out_stddevvalue)
{
	double sum = 0;
	int imageSize = in_Width * in_Height;
	for (int i = 0; i < imageSize; ++i) {
		sum += in_inputImageData[i];
	}

	*out_meanValue = sum / imageSize;

	double sumSquaredDiff = 0;
	for (int i = 0; i < imageSize; ++i) {
		double diff = in_inputImageData[i] - *out_meanValue;
		sumSquaredDiff += diff * diff;
	}
	double variance = sumSquaredDiff / imageSize;
	*out_stddevvalue = sqrt(variance);

	return 0;
}