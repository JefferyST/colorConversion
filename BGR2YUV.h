#pragma once
#include"Utils.h"
#include<stdio.h>
#include <stdlib.h>
#include<string.h>

#pragma pack(2)
typedef struct BmpFileHeader
{
	BYTE bfType[2];		//"BM"
	DWORD bfSize;		//��С
	WORD bfReserved1;	//������
	WORD bfReserved2;	//������
	DWORD bfOffBits;	//ƫ���ֽ�
} BitMapFileHeader;

typedef struct BmpInfoHeader
{
	DWORD biSize;           //λͼ��Ϣͷ��С
	LONG biWidth;           // ͼ��Ŀ�
	LONG biHeight;          // ͼ��ĸ�
	WORD biPlanes;		    // ��ʾbmpͼƬ��ƽ��������Ȼ��ʾ��ֻ��һ��ƽ�棬���Ժ����1
	WORD biBitCount;        // һ������ռ��λ������biBitCount=24ʱ����BMPͼ�����24Bit���ͼ��û�е�ɫ����
	DWORD biCompression;    // ˵��ͼ������ѹ�������ͣ�0Ϊ��ѹ����
	DWORD biSizeImage;      // ����������ռ��С, ���ֵӦ�õ���(biSize-bfOffBits)��ֵ
	LONG biXPelsPerMeter;   // ˮƽ�ֱ��ʣ�������/�ױ�ʾ��һ��Ϊ0
	LONG biYPelsPerMeter;   // ��ֱ�ֱ��ʣ�������/�ױ�ʾ��һ��Ϊ0
	DWORD biClrUsed;        // λͼʵ��ʹ�õĲ�ɫ���е���ɫ������
	DWORD biClrImportant;   // ��ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ
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