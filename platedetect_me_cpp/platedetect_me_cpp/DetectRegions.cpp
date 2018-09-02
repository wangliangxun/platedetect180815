#include "DetectRegions.h"
int HEIGHT=36;
int WIDTH=136;
void DetectRegions::setFilename(string s)
{
	filename=s;
}
Mat DetectRegions::run(Mat input)
{
	//
	Mat tmp=findplate(input);
	//return detected regions
	return tmp;
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
		imshow("可能的车牌区域",result2);
		cout<<"利用面积和横纵比清楚后轮廓数为contours.size()= "<<contours.size()<<endl;
	}

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
		if (roi_angle-m_angle<0&&roi_angle+m_angle>0)//如果-60<roi_angle<60
		{
			Rect_<float> safeBoundRect;
			bool isFormRect=calcSafeRect(roi_rect,src,safeBoundRect);
			if(!isFormRect)
				continue;
			Mat bound_mat=src(safeBoundRect);
			Mat bound_mat_b=src_b(safeBoundRect);

			Point2f roi_ref_center=roi_rect.center-safeBoundRect.tl();

			Mat deskew_mat;

			if ((roi_angle-3<0&&roi_angle+3>0)||roi_angle==90||roi_angle==-90)
			{
				deskew_mat=bound_mat;
			}
			else
			{
				//角度在3到60°之间的，首先需要旋转
				Mat rotated_mat;
				Mat rotated_mat_b;

				if(!rotation(bound_mat,rotated_mat,roi_rect_size,roi_ref_center,roi_angle))
					continue;
				if(!rotation(bound_mat_b,rotated_mat_b,roi_rect_size,roi_ref_center,roi_angle))
					continue;
				//如果图片偏斜，还需要视角转换affine
				double roi_slope=0;
				
				if (isdeflection(rotated_mat_b,roi_angle,roi_slope))
				{
					affine(rotated_mat,deskew_mat,roi_slope);
				} 
				else
				{
					deskew_mat=rotated_mat;
				}
			}
			Mat plate_mat;
			plate_mat.create(HEIGHT,WIDTH,CV_8UC1);

			if(deskew_mat.cols>=WIDTH||deskew_mat.rows>=HEIGHT)
				resize(deskew_mat,plate_mat,plate_mat.size(),0,0,INTER_AREA);
			else
				resize(deskew_mat,plate_mat,plate_mat.size(),0,0,INTER_CUBIC);

			threshold(plate_mat,plate_mat,0,255,CV_THRESH_OTSU);
			sprintf(stringname,"%s%d.jpg","possibleplate_",i);

			if(showSteps)
				imshow(stringname,plate_mat);
#if 1
			//利用SVM判别是否为车牌
			CvSVM svm;
			svm.load("svm.xml");

			Mat image;
			//由于训练时采用的是彩图，这里需要转换颜色空间
			cvtColor(plate_mat,image,CV_GRAY2BGR);

			Mat p=image.reshape(1,1);
			p.convertTo(p,CV_32FC1);

			int response=(int)svm.predict(p);
			if (response==1)
			{
				output=imge;
				if(showSteps)
					imshow("plate",image);
			}
#endif
		}
	}
#endif
	return output;
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

bool DetectRegions::verifySizes(RotatedRect mr)
{
	float error=0.4;
	float  aspect=440/140;//车牌长宽比例
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

//计算一个安全的Rect
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
/************************************************************************/
/*
	参数：
		输入安全边界mat
		输出旋转后的mat
		输入包络矩形尺寸
		输入包络矩形中心-安全边界左上坐标=转化后的中心
		包络矩形旋转角度
*/
/************************************************************************/
bool DetectRegions::rotation(Mat& in, Mat& out, const Size rect_size, const Point2f center, const double angle)
{
	Mat in_large;
	in_large.create(in.rows*1.5,in.cols*1.5,in.type());

	int x=in_large.cols/2-center.x>0?in_large.cols/2-center.x:0;
	int y=in_large.rows/2-center.y>0?in_large.rows/2-center.y:0;

	int width=x+in.cols>in_large.cols?in.cols:in_large.cols-x;
	int heigh=y+in.rows>in_large.rows?in.rows:in_large.rows-y;

	if(width!=in.cols||heigh!=in.rows)
		return false;

	Mat imageRoi=in_large(Rect(x,y,width,heigh));
	addWeighted(imageRoi,0,in,1,0,imageRoi);

	Point2f center_diff(in.cols/2,in.rows/2);
	Point2f new_center(in_large.cols/2,in_large.rows/2);

	Mat rot_mat=getRotationMatrix2D(new_center,angle,1);

	Mat mat_rotated;
	warpAffine(in_large,mat_rotated,rot_mat,Size(in_large.cols,in_large.rows),CV_INTER_CUBIC);

	Mat img_crop;
	getRectSubPix(mat_rotated,Size(rect_size.width,rect_size.height),new_center,img_crop);

	return true;

}
/************************************************************************/
/* 判断是否偏斜
	输入二值化图像，输出判断结果
*/
/************************************************************************/
bool DetectRegions::isdeflection(const Mat& in, const double angle, double& slope)
{
	int nRows=in.rows;
	int nCols=in.cols;
	assert(in.channels()==1);

	int comp_index[3];
	int len[3];//存的是像素值不等于0的第一列的列值;

	comp_index[0]=nRows/4*1;
	comp_index[1]=nRows/4*2;
	comp_index[2]=nRows/4*3;

	const uchar*p;

	for (int i=0;i<3;i++)
	{
		int index=comp_index[i];
		p=in.ptr<uchar>(index);

		int j=0;
		int value=0;
		while (0==value&&j<nCols)
		{value=int(p[j++]);
		}
		len[i]=j;
	}
	double maxlen=max(len[2],len[0]);
	double minlen=min(len[2],len[0]);
	double difflen=abs(len[2]-len[0]);

	double PI=3.14159265;
	double g=tan(angle*PI/180.0);

	if (maxlen-len[1]>nCols/32||len[1]-minlen>nCols/32)
	{
		double slope_can_1=double(len[2]-len[0])/double(comp_index[1]);
		double slope_can_2=double(len[1]-len[0])/double(comp_index[0]);
		double slope_can_3=double(len[2]-len[1])/double(comp_index[0]);
		slope=abs(slope_can_1-g)<=abs(slope_can_2-g)?slope_can_1:slope_can_2;
		//							   tan(a)+tan(b)
		//这儿用到了一个近似：tan(a+b)=--------------≈tan(a)+tan(b)[tan(b)→0]
		//							   1-tan(a)tan(b)

		return true;
	} 
	else
	{
		slope=0;
	}
	return false;

}
//扭变操作//通过opencv的仿射变换
void DetectRegions::affine(Mat& in,Mat& out,const double slope)
{
	Point2f dstTri[3];
	Point2f plTri[3];

	int height=in.rows;
	int width=in.cols;
	double xiff=abs(slope)*height;
	if (slope>0)
	{
		//左偏形，新起点坐标系在xiff/2位置
		plTri[0]=Point2f(0,0);
		plTri[1]=Point2f(width-xiff-1,0);
		plTri[2]=Point2f(0+xiff,height-1);
		dstTri[0]=Point2f(xiff/2,0);
		dstTri[1]=Point2f(width-1-xiff/2,0);
		dstTri[2]=Point2f(xiff/2,height-1);
	} 
	else
	{
		//左偏型，新起点坐标系在 -xiff/2位置
		plTri[0] = Point2f(0 + xiff, 0);
		plTri[1] = Point2f(width - 1, 0);
		plTri[2] = Point2f(0, height - 1);

		dstTri[0] = Point2f(xiff/2, 0);
		dstTri[1] = Point2f(width - 1 - xiff + xiff/2, 0);
		dstTri[2] = Point2f(xiff/2, height - 1);
	}
	Mat warp_mat=getAffineTransform(plTri,dstTri);//求变换矩阵
	Mat affine_mat;
	affine_mat.create(height,width,CV_8UC3);
	if (in.rows>HEIGHT||in.cols>WIDTH)
	{warpAffine(in,affine_mat,warp_mat,affine_mat.size(),CV_INTER_AREA);
	} 
	else
	{warpAffine(in,affine_mat,warp_mat,affine_mat.size(),CV_INTER_CUBIC);
	}
	out=affine_mat;
}






