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
	Mat src=pic.clone();
	int size=(src.rows>src.cols)?src.rows:src.cols;
	Mat dst(Size(size,size),src.type(),Scalar(0,0,0));
	int x=(int)floor((float)(size-src.cols)/2.0f);
	int y=(int)floor((float)(size-src.rows)/2.0f);
	Rect r(x,y,src.cols,src.rows);
	Mat dstroi=dst(r);
	addWeighted(dstroi,0,src,1,0,dstroi);
	return dst;
}

///////////////////////////////histeq///////////////////////////////////////////
Mat histeq(Mat in)
{


}

///////////////////////////////classify///////////////////////////////////////////
int classify(Mat& image)
{
	Mat trainTempImg;
	trainTempImg=Mat::zeros(28,28,CV_8UC1);
	resize(image,trainTempImg,trainTempImg.size());

	HOGDescriptor *hog=new HOGDescriptor(cvSize(28,28),cvSize(14,14),cvSize(7,7),cvSize(7,7),9);
	vector<float>descriptors;
	hog->compute(trainTempImg,descriptors,Size(1,1),Size(0,0));
	Mat SVMtrainMat;
	SVMtrainMat.create(1,descriptors.size(),CV_32FC1);
	int n=0;
	for (vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
	{
		SVMtrainMat.at<float>(0,n)=*iter;
		n++;
	}
	int ret=svm.predict(SVMtrainMat);
	return ret;
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

	//���������
	/*for (int i=0;i<areas.size();i++)
	{cout<<"area["<<i<<"]:"<<areas[i]<<endl;
	}*/

	int number_7=areas.size()-7;
	if (number_7<0)
	{
		cout<<"error!������С��7"<<endl;
		return 0;
	}
	int contoursmin_7=areas[number_7];
	/*cout<<"areas[number_7]:"<<contoursmin_7<<endl;*/
	/************************************************************************/
	/* ɾ��������߹�С����������ֵ��ͨ�����������ֵ�۲��������
	�������������ҳ���7�����������ƹ���7���ַ����������Ǳ��������ŵ�������
	*/
	/************************************************************************/
	int cmin=contoursmin_7;
	int cmax=100;
	itc=contours.begin();
	while (itc!=contours.end())
	{
		if(itc->size()<cmin||itc->size()>cmax)
			itc=contours.erase(itc);
		else
			++itc;
	}
	if (contours.size()>7)
	{
		cout<<"����7��ѡ�����������Ϊ��"<<contours.size()<<endl;
	}
	Mat result=plateroi.clone();
	cvtColor(result,result,CV_GRAY2BGR);
	//drawContours(result,contours,-1,Scalar(0,0,255),1);//��������
	//imshow("����",result);
	Mat kuangding=result.clone();

	if (showSteps)
	{
		for (int i=0;i<contours.size();i++)
		{
			Rect r=boundingRect(Mat(contours[i]));
			rectangle(kuangding,r,Scalar(0,255,0),1);
		}
		imshow("���ο�λ��",kuangding);
	}

#endif

#if 1
	/*���ڸ�������������������ڵ��ַ��޷�ȷ�������ǵڼ����ַ�,
	����Ծ��ε�x�����������
	x�����ŵڼ��Ͷ�Ӧ�ŵڼ����ַ�*/
	Mat pic1;
	vector<Mat> pic;
	vector<int> position_x;
	for (int i=0;i<contours.size();i++)
	{
		Rect r=boundingRect(Mat(contours[i]));
		int x=r.x;
		position_x.push_back(x);
	}
	sort(position_x.begin(),position_x.end());
	for (int j=0;j<position_x.size();j++)
	{
		int dangqiandian=position_x[j];
		for (int i=0;i<contours.size();i++)
		{
			Rect r=boundingRect(Mat(contours[i]));
			if(dangqiandian==r.x)
				pic.push_back(result(r));
		}

	}
	char platepic[255];
	int zongzifu=7;
	if (position_x.size()<7)
	{
		cout<<"cannot find 7 ���ַ�"<<endl;
		return 0;
	}
	//cout<<"position_x.size() is"<<position_x.size()<<endl;
	for (int j=0;j<7;j++)
	{
		sprintf(platepic,"%s_%d.jpg","platepic",j);
		pic[j]=setmidle(pic[j]);
		imwrite(platepic,pic[j]);
	}
	if (showSteps)
	{
		int number_zifu=1;
		imshow("diyige",pic[number_zifu]);
	}
	
#endif

//�ַ�ʶ��
#if 1
	svm.load("HOG_SVM.xml");
	vector<int> outnumber;
	for (int j=0;j<7;j++)
	{
		Mat imgetest=pic[j];
		int zifu=classify(imgetest);
		outnumber.push_back(zifu);
	}

	cout<<"=======================================================\n";
	cout<<"����⵽�ĳ���Ϊ��"<<endl;

	for (int i=1;i<outnumber.size();i++)
	{
		if(outnumber[i]<60)
			cout<<outnumber[i]<<" ";
		else
			cout<<char(outnumber[i])<<" ";
		if(i==outnumber.size()-1)
			cout<<endl;
	}
	cout<<"=======================================================\n";
#endif
	duration=static_cast<double>(getTickCount())-duration;
	duration=getTickFrequency()/duration;
	cout<<"�����ʱ��"<<duration<<"s"<<endl;

	for (;;)
	{
		int c;
		c=cvWaitKey(10);
		if((char)c==27)
			break;
	}
	return 0;

}




















