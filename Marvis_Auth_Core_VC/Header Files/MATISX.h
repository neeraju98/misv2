/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mabis.h
 * Author: Hiren
 *
 * Created on July 6, 2017, 4:04 PM
 */

#ifndef MIS100V2K7_H
#define MIS100V2K7_H


int MATISX_GenerateK7(int* x, int* y, int *rI, unsigned char* indata,int inwidth, int inheight, unsigned char* outdata, int *owidth,int *oheight,unsigned char* ISO_19794_6_Image, int *ISO_Length);
int MATISX_GetQuality(unsigned char* sourcedata,int sourcewidth,int sourceheight, int* x, int* y, int *r,int DPI, int eyetype, int *m_pupil_mean, int* Focus, int* Area, int* Blur);
int MATISX_Init(int key);
int MATISX_Uninit();

int iplWriteImage(unsigned char* frame, char* fileout, int Height, int Width, int flipcode);


#endif /* MABIS_H */