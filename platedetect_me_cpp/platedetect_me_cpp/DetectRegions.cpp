#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <time.h>
#include <iostream>
#include <vector>
#include "DetectRegions.h"
//#include <DetectRegions.h>
using namespace cv;
using namespace std;
bool showSteps=true;
CvSVM svm;
//////////////////////////////////getFilename////////////////////////////////////////

string getFilename(string s)
{
	char sep='/';
	char sepExt='.';
#if _WIN32
	sep='\\';
#endif
	size_t i=s.rfind(sep,s.length());
	if (i!=string::npos)
	{
		string fn=(s.substr(i+1,s.length()-i));
		size_t j=fn.rfind(sepExt,fn.length());
		if (i!=string::npos)
		{return fn.substr(0,j);
		} 
		else
		{return fn;
		}
	}
	else
	{
		return "";
	}

}

////////////////////////////////setmidle//////////////////////////////////////////
Mat setmidle(Mat pic)
{

}

///////////////////////////////histeq///////////////////////////////////////////
Mat histeq(Mat in)
{

}

///////////////////////////////classify///////////////////////////////////////////
int classify(Mat& image)
{

}

////////////////////////////////jingquedingwei//////////////////////////////////////////
Mat jingquedingwei(Mat in)
{

}

////////////////////////////////main//////////////////////////////////////////
int main()
{
	system("color 0E");
	cout<<"			����ʶ�� by wlx			";
	char *filename;
	Mat input_image;

	//��ȡ��ʶ��ͼƬ
	filename="./chepai/3.jpg";
	input_image=imread(filename,1);
	if (!input_image.data)
	{cout<<"error read!"<<endl;
	}
	imshow("ԭͼ",input_image);
	double duration=static_cast<double>(getTickCount());
#if 1
	/************************************************************************/
	/* ����ͨ����ɫ���ҿ��ܵĳ������򣬲�����SVM�ж���Щ���ܵ������Ƿ�Ϊ����*/
	/************************************************************************/
	string filename_whithoutExt=getFilename(filename);
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/




#endif

}




















