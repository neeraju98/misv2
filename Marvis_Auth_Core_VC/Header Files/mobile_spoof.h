
int allocateBuffer(int width, int height);

float getMean(const unsigned char* rawData);

float getBlur();
float getBlur_inputImg(unsigned char* newRaw, int newWidth, int newHeight);
float getMean_cropArea(unsigned char* rawData, int rawWidth, int rawHeight, int xw1, int yh1, int xw2, int yh2);
void getMeanStd_cropArea(const unsigned char* rawData, int rawWidth, int rawHeight, int xw1, int yh1, int xw2, int yh2, float &meanValue, float &stdValue);
float getMean_FromImage(const unsigned char* rawData, int rawWidth, int rawHeight);
void getCropAreaDetail(unsigned char* out_rawData, int& rawWidth, int& rawHeight);
int GetMeanAndStddev(unsigned char* in_inputImageData, int in_Width, int in_Height, double* out_meanValue, double* out_stddevvalue);
void freeBuffer();