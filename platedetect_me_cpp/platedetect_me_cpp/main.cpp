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

	//������߱߿��Ӱ��
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
	//�����ܵĳ�������
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
	/* �ҵ����ƵĴ�������󣬾�ȷ��λ                                       */
	/************************************************************************/
	Mat plateroi=jingquedingwei(plate);
	imwrite("myplate.jpg",plateroi);
	if(showSteps)
		imshow("",plateroi);
#if 1
	/************************************************************************/
	/* ��ȷ��λ�󣬶Գ��ƽ���һЩ��Ҫ�Ĵ���Ȼ��ʼ�ָ��ַ�                 */
	/************************************************************************/
	Mat zifu=plateroi.clone();
	//��Ϊ��һ���������������������确�������ʱ������������ַָ����ǰ������Ȼ���ټ������
	Mat kernel(3,1,CV_8U,Scalar(1));//ֻ�ڴ�ֱ�������ͣ��������ͻ���ַ�����һ��
	dilate(zifu,zifu,kernel);
	erode(zifu,zifu,kernel);
	imshow("zifu",zifu);
	//������������
	vector<vector<Point>>contours;
	findContours(zifu,contours,RETR_EXTERNAL,CHAIN_APPROX_SIMPLE);//��������������������ͼ�����Ϊ��ֵͼ
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




















