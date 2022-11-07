#include"BGR2YUV.h"

CColorTransition::~CColorTransition() {
	if (m_pYUVI444 != NULL)
		delete []m_pYUVI444;

	if (m_BmpData != NULL) 
		delete []m_BmpData;

	if (m_pYUVI420 != NULL)
		delete []m_pYUVI420;
	

}
CColorTransition::CColorTransition() {

}

void CColorTransition::BmpReader(const char* fileName) {
	int ret;

	FILE* fp = fopen(fileName, "rb");
	if (fp == NULL) {
		printf("Can not open file.");
		exit(0);
	}

	ret = fread(&m_Bmpfileheader, 1, sizeof(BmpFileHeader), fp);
	if (m_Bmpfileheader.bfType[0] != 'B' && m_Bmpfileheader.bfType[1] != 'M')
	{
		printf("Not a BMP file.");
		fclose(fp);
		exit(0);
	}
	ret = fread(&m_BmpInfoHeader, 1, sizeof(BitMapInfoHeader), fp);
	if (ret != sizeof(BitMapInfoHeader)) {
		printf("Read BitMapInfoHeader failed.");
		fclose(fp);
		exit(0);
	}

	m_nHeight = m_BmpInfoHeader.biHeight;
	m_nWidth = m_BmpInfoHeader.biWidth;

	m_nBytesPerLine = ((m_nWidth * m_BmpInfoHeader.biBitCount + 31) >>5) <<2;
	m_nRealSize = m_nBytesPerLine * m_nHeight;
	m_BmpData = (BYTE*) new char[m_nRealSize];
	fseek(fp, m_Bmpfileheader.bfOffBits, SEEK_SET);
	
	for(int i=0;i<m_nHeight;i++){
		ret = fread(&m_BmpData[m_nRealSize-m_nBytesPerLine*(i+1)], 1, m_nBytesPerLine, fp);
		if (ret != int(m_nBytesPerLine)) {
			delete[] m_BmpData;
			m_BmpData = NULL;
			exit(0);
	    }
	}

	fclose(fp);
}


int CColorTransition::Clip(int nNum,int nMin,int nMax){
	if(nNum < nMin) nNum = nMin;
	if(nNum > nMax) nNum = nMax;
	return nNum;
}

void CColorTransition::BGR888ToYUVI444() {

	int nY, nU, nV;
	m_pYUVI444 = new BYTE[m_nHeight * m_nWidth * 3];
	BYTE* pY = m_pYUVI444;
	BYTE* pU = pY + m_nHeight * m_nWidth;
	BYTE* pV = pU + m_nHeight * m_nWidth;

	for (int j = 0; j < m_nHeight; j++)
	{
		BYTE *pLine = &m_BmpData[j * m_nBytesPerLine];
		for (int i = 0; i < m_nWidth; i++)
		{
			nY = (m_nForwardCoeff[0] * pLine[3 * i + 2] + m_nForwardCoeff[1] * pLine[3 * i + 1] + m_nForwardCoeff[2] * pLine[3 * i] + 128) >> 8;
			nU = (m_nForwardCoeff[3] * pLine[3 * i + 2] + m_nForwardCoeff[4] * pLine[3 * i + 1] + m_nForwardCoeff[5] * pLine[3 * i] + 128) >> 8;
			nV = (m_nForwardCoeff[6] * pLine[3 * i + 2] + m_nForwardCoeff[7] * pLine[3 * i + 1] + m_nForwardCoeff[8] * pLine[3 * i] + 128) >> 8;

			nU += 128;
			nV += 128;

			*pY = Clip(nY, 0, 255);
			*pU = Clip(nU, 0, 255);
			*pV = Clip(nV, 0, 255);

			pY++;
			pU++;
			pV++;
		}
	}
}

void CColorTransition::BmpWriter(const char* bmpfile) {

	int ret;
	FILE* fp = fopen(bmpfile, "wb");

	if (fp == NULL) {
		printf("Save image failed.");
		exit(0);
	}

	ret = fwrite(&m_Bmpfileheader, 1, sizeof(BmpFileHeader), fp);
	if (ret != sizeof(BmpFileHeader)) {
		printf("Save BmpFileHeader Failed.\n");
		fclose(fp);
		exit(0);
	}

	ret = fwrite(&m_BmpInfoHeader, 1, sizeof(BmpInfoHeader), fp);
	if (ret != sizeof(BmpInfoHeader)) {
		printf("Save BmpInfoHeader Failed.\n");
		fclose(fp);
		exit(0);
	}

	for (int i = 0; i < m_nHeight; i++) {
		ret = fwrite(&m_BmpData[m_nRealSize - m_nBytesPerLine * (i + 1)], 1, m_nBytesPerLine, fp);
		if (ret != int(m_nBytesPerLine)) {
			printf("Save BmpData Failed.\n");
			fclose(fp);
			exit(0);
		}
	}

	fclose(fp);
}

void CColorTransition::SaveYUVI444(const char* fileName) {
	int ret;

	FILE* fp = fopen(fileName, "wb");

	if (fp == NULL) {
		printf("Open file failed.");
		fclose(fp);
		exit(0);
	}

	ret = fwrite(m_pYUVI444, 1, m_nHeight * m_nWidth * 3, fp);
	if (ret != m_nHeight * m_nWidth * 3) {
		printf("Save YUVI444 failed.\n");
		fclose(fp);
		exit(0);
	}

	fclose(fp);
}


void CColorTransition::YUVI444ToBGR888() {

	BYTE *pBGR888 = m_BmpData;
	BYTE *pY = m_pYUVI444;
	BYTE *pU = pY + m_nHeight * m_nWidth;
	BYTE *pV = pU + m_nHeight * m_nWidth;

	int nY,nU,nV;
	int nB,nG,nR;
	for (int j = 0; j < m_nHeight; j++)
	{
		for(int i = 0; i < m_nWidth; i++)
		{
		   nY = *pY;
		   nU = *pU;
		   nV = *pV;
		   
		   nU -= 128;
		   nV -= 128;
		   
		   nR = (m_nInverseCoeff[0]*nY + m_nInverseCoeff[1]*nU + m_nInverseCoeff[2]*nV+128)>>8;
		   nG = (m_nInverseCoeff[3]*nY + m_nInverseCoeff[4]*nU + m_nInverseCoeff[5]*nV+128)>>8;
		   nB = (m_nInverseCoeff[6]*nY + m_nInverseCoeff[7]*nU + m_nInverseCoeff[8]*nV+128)>>8;

		   pBGR888[j * m_nWidth + 3 * i]     = Clip(nB, 0, 255);
		   pBGR888[j * m_nWidth + 3 * i + 1] = Clip(nG, 0, 255);
		   pBGR888[j * m_nWidth + 3 * i + 2] = Clip(nR, 0, 255);
		   
		   pY++;
		   pU++;
		   pV++; 
		}
	}
}


void CColorTransition::YUVI444ToYUV420(){//odd resolution ???

    adjustResolution(&m_pYUVI444);
	int halfHeight = m_nHeight / 2;
	int halfWidth  = m_nWidth / 2;
	m_pYUVI420 = new BYTE[m_nHeight * m_nWidth + halfHeight * halfWidth * 2];
	
	BYTE *pYdes = m_pYUVI420;
	BYTE* pUdes = pYdes + m_nHeight * m_nWidth;
	BYTE* pVdes = pUdes + halfHeight * halfWidth;
	
	BYTE *pYsrc = m_pYUVI444;
	BYTE *pUsrc = pYsrc + m_nHeight * m_nWidth;
	BYTE *pVsrc = pUsrc + m_nHeight * m_nWidth;
	
	memcpy(pYdes, pYsrc, m_nHeight * m_nWidth *sizeof(BYTE));
	
	for(int i=0;i<halfHeight;i++){
		for(int j=0;j<halfWidth;j++){

		   *pUdes = *pUsrc;
		   *pVdes = *pVsrc;
		   
		   pUdes++;
           pVdes++;
		   
		   pUsrc += 2;
		   pVsrc += 2;
		}
		//pUsrc = pYsrc + m_nHeight*m_nWidth + 2*m_nWidth*(i+1);
		//pVsrc = pYsrc + m_nHeight*m_nWidth*2 + 2*m_nWidth*(i+1);
		pUsrc += m_nWidth;
		pVsrc += m_nWidth;
	}
}


void CColorTransition::adjustResolution(BYTE **pInYUVI444){

	if((m_nHeight %2==0)&&(m_nWidth %2==0)) return;
	BYTE *pOutYUVI444 = NULL;

	if(m_nHeight %2==1){
		pOutYUVI444 = new BYTE[(m_nHeight +1)* m_nWidth *3];
		
		for(int i=0;i<3;i++){
			memcpy(pOutYUVI444 + m_nWidth * (i * m_nHeight + i), (*pInYUVI444) + m_nWidth * (i * m_nHeight), m_nWidth * m_nHeight * sizeof(BYTE));
			memcpy(pOutYUVI444 + m_nWidth * ((i + 1) * m_nHeight + i), (*pInYUVI444) + m_nWidth * ((i + 1) * m_nHeight - 1), m_nWidth * sizeof(BYTE));
		}
		m_nHeight +=1;
		delete [](*pInYUVI444);
		*pInYUVI444 = pOutYUVI444;
	}
	
	if(m_nWidth %2==1){
		pOutYUVI444 = new BYTE[m_nHeight *(m_nWidth +1)*3];
		
		BYTE *pYsrc = *pInYUVI444;
		BYTE *pUsrc = pYsrc+m_nHeight * m_nWidth;
		BYTE *pVsrc = pUsrc+m_nHeight * m_nWidth;
		
		BYTE *pYdes = pOutYUVI444;
		BYTE *pUdes = pYdes+m_nHeight *(m_nWidth +1);
		BYTE *pVdes = pUdes+m_nHeight *(m_nWidth +1);
		
		for(int i=0;i< m_nHeight;i++){
			memcpy(pYdes+ m_nWidth *i+i,pYsrc+ m_nWidth *i , m_nWidth *sizeof(BYTE));
			memcpy(pYdes+m_nWidth*(i+1)+i,pYsrc+ m_nWidth *(i+ 1)-1, sizeof(BYTE));
			
			memcpy(pUdes+ m_nWidth *i+i,pUsrc+ m_nWidth *i , m_nWidth *sizeof(BYTE));
			memcpy(pUdes+m_nWidth*(i+1)+i,pUsrc+ m_nWidth *(i+ 1)-1, sizeof(BYTE));
			
			memcpy(pVdes+ m_nWidth *i+i,pVsrc+ m_nWidth *i , m_nWidth *sizeof(BYTE));
			memcpy(pVdes+m_nWidth*(i+1)+i,pVsrc+ m_nWidth *(i+ 1)-1, sizeof(BYTE));
		}
		m_nWidth +=1;
		delete [](*pInYUVI444);
		*pInYUVI444 = pOutYUVI444;
	}
}


void CColorTransition::savaYU12(const char *fileName){
	int ret;
	BYTE *pY = m_pYUVI420;
	BYTE* pU = pY + m_nHeight * m_nWidth;
	BYTE* pV = pU + m_nHeight * m_nWidth / 4;
	
	FILE *fp = fopen(fileName,"wb");
	
	ret = fwrite(pY, 1, m_nHeight * m_nWidth * sizeof(BYTE), fp);
	if(ret!= m_nHeight * m_nWidth){
		printf("Save Y failed.");
		fclose(fp);
		exit(0);
	}
	
	ret = fwrite(pU, 1, m_nHeight * m_nWidth / 4 * sizeof(BYTE), fp);
	if(ret!= m_nHeight * m_nWidth /4){
		printf("Save U failed.");
		fclose(fp);
		exit(0);
	}
	
	ret = fwrite(pV, 1, m_nHeight * m_nWidth / 4 * sizeof(BYTE), fp);
	if(ret!= m_nHeight * m_nWidth /4){
		printf("Save V failed.");
		fclose(fp);
		exit(0);
	}
	
	fclose(fp);	
}
 
void CColorTransition::savaYV12(const char *fileName){
	int ret;
	BYTE *pY = m_pYUVI420;
	BYTE *pU = pY+ m_nHeight * m_nWidth;
	BYTE *pV = pU+ m_nHeight * m_nWidth /4;
	
	FILE *fp = fopen(fileName,"wb");
	
	ret = fwrite(pY,1, m_nHeight * m_nWidth *sizeof(BYTE),fp);
	if(ret!= m_nHeight * m_nWidth){
		printf("Save Y failed.");
		fclose(fp);
		exit(0);
	}
	
	ret = fwrite(pV,1, m_nHeight * m_nWidth /4*sizeof(BYTE),fp);
	if(ret!= m_nHeight * m_nWidth /4){
		printf("Save V failed.");
		fclose(fp);
		exit(0);
	}
	
	ret = fwrite(pU,1, m_nHeight * m_nWidth /4*sizeof(BYTE),fp);
	if(ret!= m_nHeight * m_nWidth /4){
		printf("Save U failed.");
		fclose(fp);
		exit(0);
	}
	
	fclose(fp);
}
 

void CColorTransition::savaNV12(const char *fileName){
	int ret;
	BYTE *pY = m_pYUVI420;
	BYTE *pU = pY+ m_nHeight * m_nWidth;
	BYTE *pV = pU+ m_nHeight * m_nWidth /4;
	BYTE *pUV = new BYTE[m_nHeight * m_nWidth /2];
	FILE* fp = fopen(fileName, "wb");

	for(int i=0;i< m_nHeight * m_nWidth /4;i++){
		pUV[i*2]=pU[i];
		pUV[i*2+1]=pV[i];
	}
	
	ret = fwrite(pY,1, m_nHeight * m_nWidth *sizeof(BYTE),fp);
	if(ret!= m_nHeight * m_nWidth){
		printf("Save Y failed.");
		fclose(fp);
		exit(0);
	}
	
	ret = fwrite(pUV,1, m_nHeight * m_nWidth /2*sizeof(BYTE),fp);
	if(ret!= m_nHeight * m_nWidth /2){
		printf("Save UV failed.");
		fclose(fp);
		exit(0);
	}
	
    delete []pUV;
	pUV = NULL;
	fclose(fp);
}

void CColorTransition::savaNV21(const char *fileName){
	int ret;
	BYTE *pY = m_pYUVI420;
	BYTE *pU = pY+ m_nHeight * m_nWidth;
	BYTE *pV = pU+ m_nHeight * m_nWidth /4;
	BYTE *pUV = new BYTE[m_nHeight * m_nWidth /2];
	FILE* fp = fopen(fileName, "wb");

	for(int i=0;i< m_nHeight * m_nWidth /4;i++){
		pUV[i*2]   = pV[i];
		pUV[i*2+1] = pU[i];
	}
	
	ret = fwrite(pY, 1, m_nHeight * m_nWidth * sizeof(BYTE), fp);
	if(ret!= m_nHeight * m_nWidth){
		printf("Save Y failed.");
		fclose(fp);
		exit(0);
	}
	
	ret = fwrite(pUV, 1, m_nHeight * m_nWidth / 2 * sizeof(BYTE), fp);
	if(ret!= m_nHeight * m_nWidth /2){
		printf("Save UV failed.");
		fclose(fp);
		exit(0);
	}
	
    delete []pUV;
	pUV = NULL;
	fclose(fp);
}

 
 

////References
////https://en.wikipedia.org/wiki/YCbCr
////http://www.itu.int/rec/R-REC-BT.601
////ITU-R BT.601-7
//
////https://blog.csdn.net/sunty2016/article/details/106589379/
////https://blog.csdn.net/qq_20797295/article/details/102679394/
//

////https://blog.csdn.net/guyuealian/article/details/82454945
//

//
////https://www.cnblogs.com/yongdaimi/p/10647005.html
////https://blog.csdn.net/zhiyuan2021/article/details/123931447
