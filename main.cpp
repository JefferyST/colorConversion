#include<iostream>
#include"BGR2YUV.h"
using namespace std;

int main() {
	CColorTransition bmpTransition;
	bmpTransition.BmpReader("test.bmp");
	bmpTransition.BGR888ToYUVI444();
	bmpTransition.SaveYUVI444("test.yuv");

	//bmpTransition.YUV444ToYUV420();
	
	//bmpTransition.savaYU12("testyu12.yuv");
	//bmpTransition.YUVI444ToBGR888();
	//bmpTransition.write("test2save.bmp");
	//bmpTransition.SaveYUVI444("test.yuv");
}


