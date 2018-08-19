#include "DetectRegions.h"
int HEIGHT=36;
int WIDTH=136;
void DetectRegions::setFilename(string s)
{
	filename=s;
}
Mat DetectRegions::run(Mat input)
{
	Mat tmp=findplate(input);
	return tmp;
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
}
Mat DetectRegions::findplate(Mat input)
{
	Mat output;
	Mat imge,pic1,colorMatchpic;
	imge=input.clone();
	colorMatchpic=colorMatch(imge,BLUE);
	/************************************************************************/
	/*                 返回经过颜色匹配后的灰度图                           */
	/************************************************************************/
	if(showSteps)
		imshow("colorMatchpic-颜色匹配查找可能区域",colorMatchpic);
	Mat img_threshold=sobelyuchuli(colorMatchpic);//均值滤波→sobel边缘检测→二值化→闭运算
	if(showSteps)
		imshow("img_threshold",img_threshold);

	//findContours查找轮廓
	vector<vector<Point>> contours;
	findContours(img_threshold,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

	if (showSteps)
	{
		Mat result1;
		input.copyTo(result1);
		for (int i=0;i<contours.size();i++)
		{
			Rect r0=boundingRect(Mat(contours[i]));
			rectangle(result1,r0,cv::Scalar(0,255,255),2);
		}
		cout<<"清楚前的轮廓数contours.size = "<<contours.size()<<endl;
		imshow("初次检测的所有轮廓",result1);
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
}

Mat DetectRegions::colorMatch(const Mat& src,const Color r)
{
	const float max_sv=255;
	const float minref_sv=64;
	const float minabs_sv=95;
	/*色调H
		用角度度量，取值范围为0°～360°，从红色开始按逆时针方向计算，
		红色为0°，绿色为120°,蓝色为240°。它们的补色是：黄色为60°，青色为180°,品红为300°；*/
	//bule的H范围
	const int min_blue=100;
	const int max_blue=140;
	//yellow的H范围
	const int min_yellow=15;
	const int max_yellow=40;

	Mat src_hsv;
	cvtColor(src,src_hsv,CV_BGR2HSV);
	vector<Mat>	 hsvSplit;
	split(src_hsv,hsvSplit);
	equalizeHist(hsvSplit[2],hsvSplit[2]);
	merge(hsvSplit,src_hsv);

	//匹配模板基色，切换已查找想要的颜色
	int min_h=0;
	int max_h=0;
	switch (r)
	{
	case BLUE:
		min_h=min_blue;
		max_h=max_blue;
		break;
	case YELLOW:
		min_h=min_yellow;
		max_h=max_yellow;
		break;
	}
	float diff_h=float((max_h-min_h)/2);
	int avg_h=min_h+diff_h;
	int channels=src_hsv.channels();
	int nRows=src_hsv.rows;
	int nCols=src_hsv.cols;

	if (src_hsv.isContinuous())
	{
		nCols*=nRows;
		nRows=1;
	}
	int i,j;
	uchar* p;
	float s_all=0;
	float v_all=0;
	float count=0;
	for (i=0;i<nRows;++i)
	{
		p=src_hsv.ptr<uchar>(i);
		for (j=0;j<nCols;j+=3)
		{
			int H=int(p[j]);//0~180
			int S=int(p[j+1]);//0~255
			int V=int(p[j+2]);//0~255

			bool colorMatched=false;
			if (H>min_h&&H<max_h)
			{
				int Hdiff=0;
				if (H>avg_h)
					Hdiff=H-avg_h;
				else
					Hdiff=avg_h-H;
				
				float Hdiff_p=float(Hdiff)/diff_h;

				float min_sv=0;
				min_sv=minabs_sv;

				if(S>min_sv&&S<max_sv&&V>min_sv&&V<max_sv)
					colorMatched=true;
			}
			if (colorMatched==true)
			{p[j]=0;p[j+1]=0;p[j+2]=255;
			}
			else
			{p[j]=0;p[j+1]=0;p[j+2]=0;
			}
		}
	}

	//获取颜色匹配后的二值图
	Mat src_gray;
	vector<Mat> hsvSplit_done;
	split(src_hsv,hsvSplit_done);
	src_gray=hsvSplit_done[2];

	return src_gray;

}

Mat DetectRegions::sobelyuchuli(Mat in)
{
	Mat img=in.clone();
	if (img.channels()==3)
	{cvtColor(img,img,CV_BGR2GRAY);
	}
	Mat pic,img_soble,img_threshold;
	blur(img,pic,Size(5,5));//均值滤波
	Sobel(pic,img_soble,CV_8U,1,0,3,1,0,BORDER_DEFAULT);//sobel边缘检测
	threshold(img_soble,img_threshold,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);//二值化
	Mat element=getStructuringElement(MORPH_RECT,Size(27,3));//获取结构元素
	morphologyEx(img_threshold,img_threshold,CV_MOP_CLOSE,element);//形态学闭运算
	return img_threshold;
}








