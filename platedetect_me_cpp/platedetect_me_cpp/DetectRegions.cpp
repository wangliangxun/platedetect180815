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
	/*                 ���ؾ�����ɫƥ���ĻҶ�ͼ                           */
	/************************************************************************/
	if(showSteps)
		imshow("colorMatchpic-��ɫƥ����ҿ�������",colorMatchpic);
	Mat img_threshold=sobelyuchuli(colorMatchpic);//��ֵ�˲���sobel��Ե������ֵ����������
	if(showSteps)
		imshow("img_threshold",img_threshold);

	//findContours��������
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
		cout<<"���ǰ��������contours.size = "<<contours.size()<<endl;
		imshow("���μ�����������",result1);
	}
	

	vector<vector<Point>>::iterator itc=contours.begin();
	vector<RotatedRect> rects;

	while(itc!=contours.end())
	{
		RotatedRect mr=minAreaRect(Mat(*itc));
		if (!verifySizes(mr))
		{itc=contours.erase(itc);
		} 
		else
		{++itc;
		rects.push_back(mr);
		}
	}
	
	if (showSteps)
	{
		Mat result2;
		input.copyTo(result2);
		for (int i=0;i<rects.size();i++)
		{
			Rect r0=boundingRect(Mat(contours[i]));
			rectangle(result2,r0,cv::Scalar(0,255,255),2);
		}
		imshow("���ܵĳ�������",result2);
		cout<<"��������ͺ��ݱ������������Ϊcontours.size()= "<<contours.size()<<endl;
	}
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
#if 1
	Mat src;
	input.copyTo(src);
	cvtColor(src,src,CV_BGR2GRAY);
	Mat src_b=src.clone();
	
	char stringname[255];
	char possibleplate[255];
	sprintf(possibleplate,"possibleplate_");
	
	for (int i=0;i<rects.size();i++)
	{
		RotatedRect roi_rect=rects[i];
		float r=(float)roi_rect.size.width/(float)roi_rect.size.height;
		float roi_angle=roi_rect.angle;

		Size roi_rect_size=roi_rect.size;
		if (r<1)
		{roi_angle=90+roi_angle;
		swap(roi_rect_size.width,roi_rect_size.height);
		}
		float m_angle=60;
		if (roi_angle-m_angle<0&&roi_angle+m_angle>0)//���-60<roi_angle<60
		{
			Rect_<float> safeBoundRect;
			bool isFormRect=calcSafeRect(roi_rect,src,safeBoundRect);



			/************************************************************************/
			/*                                                                      */
			/************************************************************************/
		}

	}



#endif



}

Mat DetectRegions::colorMatch(const Mat& src,const Color r)
{
	const float max_sv=255;
	const float minref_sv=64;
	const float minabs_sv=95;
	/*ɫ��H
		�ýǶȶ�����ȡֵ��ΧΪ0�㡫360�㣬�Ӻ�ɫ��ʼ����ʱ�뷽����㣬
		��ɫΪ0�㣬��ɫΪ120��,��ɫΪ240�㡣���ǵĲ�ɫ�ǣ���ɫΪ60�㣬��ɫΪ180��,Ʒ��Ϊ300�㣻*/
	//bule��H��Χ
	const int min_blue=100;
	const int max_blue=140;
	//yellow��H��Χ
	const int min_yellow=15;
	const int max_yellow=40;

	Mat src_hsv;
	cvtColor(src,src_hsv,CV_BGR2HSV);
	vector<Mat>	 hsvSplit;
	split(src_hsv,hsvSplit);
	equalizeHist(hsvSplit[2],hsvSplit[2]);
	merge(hsvSplit,src_hsv);

	//ƥ��ģ���ɫ���л��Ѳ�����Ҫ����ɫ
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

	//��ȡ��ɫƥ���Ķ�ֵͼ
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
	blur(img,pic,Size(5,5));//��ֵ�˲�
	Sobel(pic,img_soble,CV_8U,1,0,3,1,0,BORDER_DEFAULT);//sobel��Ե���
	threshold(img_soble,img_threshold,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);//��ֵ��
	Mat element=getStructuringElement(MORPH_RECT,Size(27,3));//��ȡ�ṹԪ��
	morphologyEx(img_threshold,img_threshold,CV_MOP_CLOSE,element);//��̬ѧ������
	return img_threshold;
}

bool DetectRegions::verifySizes(RotatedRect mr)
{
	float error=0.4;
	float  aspect=440/140;//���Ƴ������
	int min=15*aspect*15;
	int max=125*aspect*125;
	float rmin=aspect*(1-error);
	float rmax=aspect*(1+error);

	int area=mr.size.height*mr.size.width;
	float r=(float)mr.size.width/(float)mr.size.height;
	if(r<1)
		float r=(float)mr.size.height/(float)mr.size.width;
	if ((area<min||area>>max)||(r<rmin||r>rmax))
	{return false;
	} 
	else
	{return true;
	}
}

//����һ����ȫ��Rect
bool DetectRegions::calcSafeRect(const RotatedRect& roi_rect,const Mat& src,Rect_<float>& safeBoundRect)
{
	Rect_<float> boudRect=roi_rect.boundingRect();
	float tl_x=boudRect.x>0?boudRect.x:0;
	float tl_y=boudRect.y>0?boudRect.y:0;
	float br_x=boudRect.x+boudRect.width<src.cols?boudRect.x+boudRect.width-1:src.cols-1;
	float br_y=boudRect.y+boudRect.height<src.rows?boudRect.y+boudRect.height-1:src.rows-1;
	
	float roi_width=br_x-tl_x;
	float roi_heigh=br_y-tl_y;
	
	if (roi_heigh<=0||roi_width<=0)
		return false;
	safeBoundRect=Rect_<float>(tl_x,tl_y,roi_width,roi_heigh);
	return true;
}




