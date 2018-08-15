/**
 * @Brief   SVM样本训练
 * @IDE     Visual Studio 2010
 * @Library  OpenCV 2.4.10
 * @Author    zl.xiong
 * @Version    1.0
 * @Date      2015-07-03
 * @Email   titxzl@163.com
 * @Update   
 ***********************************@details*************************************************************
 *  
 *********************************************************************************************************
**/
#include <iostream>
#include <opencv2/opencv.hpp>
#include "stdio.h"
#include <fstream>
#include <ostream>
using namespace std;
using namespace cv;
CvSVM svm;

int classify(Mat& image)
{   
	
	Mat trainTempImg;
	trainTempImg=Mat::zeros(28,28,CV_32FC1);
	resize(image,trainTempImg,trainTempImg.size());

	HOGDescriptor *hog=new HOGDescriptor(cvSize(28,28),cvSize(14,14),cvSize(7,7),cvSize(7,7),9);
	vector<float>descriptors;
	hog->compute(trainTempImg,descriptors,Size(1,1),Size(0,0));

	//cout<<"HOG dims: "<<descriptors.size()<<endl;
	Mat SVMtrainMat;
	SVMtrainMat.create(1,descriptors.size(),CV_32FC1);
	int n=0;
	for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
	{
		SVMtrainMat.at<float>(0,n)=*iter;
		n++;
	}
	int ret=svm.predict(SVMtrainMat);
 
	if (ret<60)
		cout<<" 预测结果为 ："<<ret<<endl; 
	else
		cout<<" 预测结果为 ："<<char(ret)<<endl;
	
	return ret;
}

void train()
{   
	vector<string> img_path;
	vector<int> img_catg;

	cv::Directory dir;

	string path1 = "./yangben";
	string exten1 = "*"; 
	bool addPath1 = true; 

	vector<string> foldernames = dir.GetListFolders(path1, exten1, addPath1);

	cout<<"folder names: "<<endl;
	for (int i = 0; i < foldernames.size(); i++)
	{
		cout<<foldernames[i]<<endl;
		  
	}


	string path2= "./yangben";
	string exten2 = "*";
	bool addPath2 = true;//false

	vector<string> allfilenames = dir.GetListFilesR(path2, exten2, addPath2);
	for(int i=0;i<allfilenames.size();i++)
	{
		switch(i/50)    //这里重复造轮子了  用img_catg.push_back(i/50)和 代替;
		{
		case 0:
			img_catg.push_back(0);
			break;
		case 1:
			img_catg.push_back(1);
			break;
		case 2:
			img_catg.push_back(2);
			break;
		case 3:
			img_catg.push_back(3);
			break;
		case 4:
			img_catg.push_back(4);
			break;
		case 5:
			img_catg.push_back(5);
			break;
		case 6:
			img_catg.push_back(6);
			break;
		case 7:
			img_catg.push_back(7);
			break;
		case 8:
			img_catg.push_back(8);
			break;
		case 9:
			img_catg.push_back(9);
			break;
		case 10:
			img_catg.push_back(65);
			break;
		case 11:
			img_catg.push_back(66);
			break;
		case 12:
			img_catg.push_back(67);
			break;
		case 13:
			img_catg.push_back(68);
			break;
		case 14:
			img_catg.push_back(69);
			break;
		case 15:
			img_catg.push_back(70);
			break;
		case 16:
			img_catg.push_back(71);
			break;
		case 17:
			img_catg.push_back(72);
			break;
		case 18:
			img_catg.push_back(74);
			break;
		case 19:
			img_catg.push_back(75);
			break;
		case 20:
			img_catg.push_back(76);
			break;
		case 21:
			img_catg.push_back(77);
			break;
		case 22:
			img_catg.push_back(78);
			break;
		case 23:
			img_catg.push_back(80);
			break;
		case 24:
			img_catg.push_back(81);
			break;
		case 25:
			img_catg.push_back(82);
			break;
		case 26:
			img_catg.push_back(83);
			break;
		case 27:
			img_catg.push_back(84);
			break;
		case 28:
			img_catg.push_back(85);
			break;
		case 29:
			img_catg.push_back(86);
			break;
		case 30:
			img_catg.push_back(87);
			break;
		case 31:
			img_catg.push_back(88);
			break;
		case 32:
			img_catg.push_back(89);
			break;
		case 33:
			img_catg.push_back(90);
			break;

		}
	}

	Mat data_mat,res_mat;
	int num_img=allfilenames.size();
	data_mat=Mat::zeros(num_img,324,CV_32FC1);
	res_mat=Mat::zeros(num_img,1,CV_32FC1);

	Mat src;
	Mat trainImage(cvSize(28,28),8,3);

	for(string::size_type i=0;i!=allfilenames.size();i++)
	{
		src=imread(allfilenames[i].c_str(),1);
		if(src.empty())
		{
			cout<<"Can't load the image: "<<allfilenames[i].c_str()<<endl;
			continue;
		}

		resize(src,trainImage,trainImage.size());
		HOGDescriptor *hog=new HOGDescriptor(cvSize(28,28),//窗口大小
			cvSize(14,14),//block大小
			cvSize(7,7),//每次移动宽度
			cvSize(7,7),//cell单元大小
			9);//直方图bin数目
		vector<float> descriptors;//存放结果
		hog->compute(trainImage,descriptors,Size(1,1),Size(0,0));//hog特征计算
 
		unsigned long n=0;
		for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
		{
			data_mat.at<float>(i,n)=*iter;
			//存储hog特征
			n++;
		}
		res_mat.at<float>(i,0)=img_catg[i];
		//cout<<" 处理完毕： "<<allfilenames[i].c_str()<<" "<<img_catg[i]<<endl;
	}
	CvSVM svm;
	CvSVMParams param;
	CvTermCriteria criteria;
	criteria=cvTermCriteria(CV_TERMCRIT_EPS,1000,FLT_EPSILON);
	param=CvSVMParams(CvSVM::C_SVC,CvSVM::RBF,10.0,0.09,1.0,10.0,0.5,1.0,NULL,criteria);
	svm.train(data_mat,res_mat,Mat(),Mat(),param);
	//保存训练好的分类器
	svm.save("HOG_SVM.xml");
	cout<<"训练完成！"<<endl;
}

int main()
{   
	system("color 0E");  //改变console 背景字体颜色 
	//计时
	double duration = static_cast<double>(getTickCount());
	train();
	duration = static_cast<double>(getTickCount())-duration;
	duration /=getTickFrequency();
	cout <<"训练用时 :"<<duration<<" S"<<endl;    

	svm.load("HOG_SVM.xml");
	char platepic[255];
	for (int j=1;j<8;j++)
	{   
		sprintf(platepic,"%s_%d.jpg","platepic",j);
		Mat pic=imread(platepic,1);
		classify(pic);
	}

	/*char num[255];
	for (int j=0;j<10;j++)
	{   
	sprintf(num,"%d.png",j);
	Mat numpic=imread(num,1);
	classify(numpic);
	}*/
 
	system("pause");
	return 0;
}