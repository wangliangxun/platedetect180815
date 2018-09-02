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
/************************************************************************/
/*                                                                      */
/************************************************************************/
Mat jingquedingwei(Mat in)
{
	Mat pic;
	if(in.channels()==3)
		cvtColor(in,pic,CV_BGR2GRAY);
	else
		pic=in.clone();

	Mat plate_threshold=pic.clone();
	threshold(pic,plate_threshold,10,255,CV_THRESH_BINARY);

	int rmin=(int)(0.1*pic.cols);
	vector<int>sum(plate_threshold.rows,0);
	Mat histx(plate_threshold.cols,plate_threshold.rows,CV_8U,Scalar(0));

	for (int i=0;i<plate_threshold.rows;i++)
	{
		for (int j=0;j<plate_threshold.cols;j++)
		{
			if(plate_threshold.at<uchar>(i,j)==255)
				sum[i]++;
		}
	}
	int shangbianjie=0;
	int xiabianjie=pic.rows;

	for (int i=sum.size()/2+1;i>0;i--)
	{
		if (sum[i]<=rmin)
		{
			shangbianjie=i;
			break;
		}
	}
	for (int i=sum.size()/2+1;i<sum.size();i++)
	{
		if (sum[i]<=rmin)
		{
			xiabianjie=i;
			break;
		}
	}

	Mat plateroi;
	plateroi=pic(Rect(0,shangbianjie,pic.cols,xiabianjie-shangbianjie));

	//消除左边边框的影响
	int nr=plateroi.rows;
	int nc=plateroi.cols*plateroi.channels();
	for (int j=0;j<nr;j++)
	{
		uchar* data=plateroi.ptr<uchar>(j);
		for (int i=0;i<5;i++)
		{
			data[i]=0;
		}
	}
	Mat resizepic(Size(plateroi.cols+20,plateroi.rows+20),CV_8UC1,cv::Scalar(0));
	for (j=0;j<nr;j++)
	{
		uchar* data1=plateroi.ptr<uchar>(j);
		uchar* data2=resizepic.ptr<uchar>(j+10);
		for (int i=0;i<nc;i++)
		{
			data2[i+10]=data1[i];
		}
	}
	return resizepic;

}

////////////////////////////////main//////////////////////////////////////////
int main()
{
	system("color 0E");
	cout<<"			车牌识别 by wlx			";
	char *filename;
	Mat input_image;

	//读取待识别图片
	filename="./chepai/3.jpg";
	input_image=imread(filename,1);
	if (!input_image.data)
	{cout<<"error read!"<<endl;
	}
	imshow("原图",input_image);
	double duration=static_cast<double>(getTickCount());
#if 1
	/************************************************************************/
	/* 首先通过颜色查找可能的车牌区域，并利用SVM判断这些可能的区域是否为车牌*/
	/************************************************************************/
	string filename_whithoutExt=getFilename(filename);
	//检测可能的车牌区域
	DetectRegions detectRegions;
	detectRegions.setFilename(filename_whithoutExt);
	detectRegions.saveRegions=true;
	detectRegions.showSteps=true;
	Mat plate=detectRegions.run(input_image);
	if (!plate.data)
	{
		cout<<"cannot find plate!"<<endl;
		return 0;
	}

#endif
	/************************************************************************/
	/* 找到车牌的呆滞区域后，精确定位                                       */
	/************************************************************************/
	Mat plateroi=jingquedingwei(plate);
	imwrite("myplate.jpg",plateroi);
	if(showSteps)
		imshow("",plateroi);
#if 1
	/************************************************************************/
	/* 精确定位后，对车牌进行一些必要的处理然后开始分割字符                 */
	/************************************************************************/
	Mat zifu=plateroi.clone();
	//因为第一个汉子有两个轮廓，例如‘粤’检测时会把上下两部分分割开，提前先膨胀然后再检测轮廓
	Mat kernel(3,1,CV_8U,Scalar(1));//只在垂直方向膨胀，左右膨胀会把字符连在一块
	dilate(zifu,zifu,kernel);
	erode(zifu,zifu,kernel);
	imshow("zifu",zifu);
	//查找所有轮廓
	vector<vector<Point>>contours;
	findContours(zifu,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);//查找所有外轮廓，输入图像必须为二值图
	vector<vector<Point>>::const_iterator itc=contours.begin();//???????????????????????????????????????
	vector<float> areas;
	while (itc!=contours.end())
	{
		areas.push_back(itc->size());//??????????????????????????????????????????
	}
	sort(areas.begin(),areas.end());//???????????????????????????????????????????
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

#endif












}




















