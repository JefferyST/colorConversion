#include<iostream>
#include"BGR2YUV.h"
using namespace std;

int main() {
	CColorTransition bmpTransition;
	bmpTransition.BmpReader("lena.bmp");
	bmpTransition.BGR888ToYUVI444();
	bmpTransition.YUVI444ToYUV420();
	bmpTransition.savaYU12("lena.yuv");
}


