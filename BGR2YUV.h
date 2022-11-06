#pragma once
#include"Utils.h"
#include<stdio.h>
#include <stdlib.h>
#include<string.h>

#pragma pack(2)
typedef struct BmpFileHeader
{
	BYTE bfType[2];		//"BM"
	DWORD bfSize;		//大小
	WORD bfReserved1;	//保留字
	WORD bfReserved2;	//保留字
	DWORD bfOffBits;	//偏移字节
} BitMapFileHeader;

typedef struct BmpInfoHeader
{
	DWORD biSize;           //位图信息头大小
	LONG biWidth;           // 图像的宽
	LONG biHeight;          // 图像的高
	WORD biPlanes;		    // 表示bmp图片的平面属，显然显示器只有一个平面，所以恒等于1
	WORD biBitCount;        // 一像素所占的位数，当biBitCount=24时，该BMP图像就是24Bit真彩图，没有调色板项
	DWORD biCompression;    // 说明图象数据压缩的类型，0为不压缩。
	DWORD biSizeImage;      // 像素数据所占大小, 这个值应该等于(biSize-bfOffBits)的值
	LONG biXPelsPerMeter;   // 水平分辨率，用象素/米表示。一般为0
	LONG biYPelsPerMeter;   // 垂直分辨率，用象素/米表示。一般为0
	DWORD biClrUsed;        // 位图实际使用的彩色表中的颜色索引数
	DWORD biClrImportant;   // 对图象显示有重要影响的颜色索引的数目，如果是0，表示都重要
}	BitMapInfoHeader;


class CColorTransition {
private:
	BmpFileHeader m_Bmpfileheader;
	BmpInfoHeader m_BmpInfoHeader;
	int m_nHeight;
	int m_nWidth;
	int m_nRealSize;
	int m_nBytesPerLine;
	BYTE* m_BmpData;
	BYTE* m_pYUVI444;
	BYTE* m_pYUVI420;
	//ITU-R BT.601-7 Full Range [Y U V] = m_nForwardCoeff[]*[R G B]
	int m_nForwardCoeff[9] = { 77,  150,  29,
						   -43,  -85, 128,
						   128, -107, -21 };

	//709 Full Range
	//int m_nForwardCoeff[9] = { 54,  183,  18,
	//	                        -29,  -99, 128,
	//					        128, -116, -12}; 

	//2020 Full Range
	//int m_nForwardCoeff[9] = { 67,  174,  15,
	//	                        -36,  -92, 128,
	//					        128, -118, -10}; 

	//ITU-R BT.601-7 Full Range [R G B] = m_nInverseCoeff[]*[Y U V]
	int m_nInverseCoeff[9] = { 256,   0,  359,
					           256, -88, -183,
					           256, 454,    0 };

	//709 Full Range
	//int m_nInverseCoeff[9] = {256,   0,  403,
	//                          256, -48, -120,
	//					        256, 475,    0};

	//2020 Full Range
	//int m_nInverseCoeff[9] = {256,   0,  377,
	//                          256, -42, -146,
	//						    256, 482,    0};


public:
	CColorTransition();
	~CColorTransition();


	void BmpReader(const char* fileName);
	void BmpWriter(const char* bmpfile);
	int Clip(int num, int min, int max);
	void BGR888ToYUVI444();
	void YUVI444ToBGR888();
	void SaveYUVI444(const char* fileName);
	void YUVI444ToYUV420();
	void adjustResolution(BYTE **pInYUVI444);
	void savaYU12(const char* fileName);
	void savaYV12(const char* fileName);
	void savaNV12(const char* fileName);
	void savaNV21(const char* fileName);

};