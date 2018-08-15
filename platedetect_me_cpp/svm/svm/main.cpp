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

#include<opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <ml.h>
#include <iostream>
#include <time.h>
using namespace cv;
using namespace std;
 
void getPlate(Mat& trainingImages, vector<int>& trainingLabels)
{
	string dir_path="./train/plate";
	 Directory dir;
	
	vector<string> filenames=dir.GetListFiles(dir_path,"*.jpg",false);
    char file_in[255];
	sprintf(file_in,"train/plate/");

	int size =  filenames.size();
	if (0 == size)
		cout << "No File Found !" << endl;
	char name[255];
	for (int i = 0;i < size;i++)
	{
		sprintf(name,"%s%d.jpg",file_in,i);
		//cout<<name<<endl;
		Mat img = imread(name,1);
		if (!img.data)
		{
			cout<<"error"<<endl;
		}
		 //imshow("dfe",img); 
		img= img.reshape(1, 1);
		trainingImages.push_back(img);
		trainingLabels.push_back(1);
	}
} 

void getNoPlate(Mat& trainingImages, vector<int>& trainingLabels)
{
	string dir_path="./train/noplate";
	Directory dir;

	vector<string> filenames=dir.GetListFiles(dir_path,"*.jpg",false);
	char file_in[255];
	sprintf(file_in,"train/noplate/");

	int size =  filenames.size();
	if (0 == size)
		cout << "No File Found !" << endl;
	char name[255];
	for (int i = 0;i < size;i++)
	{
		sprintf(name,"%s%d.jpg",file_in,i);
		Mat img = imread(name,1);
		if (!img.data)
		{
			cout<<"error"<<endl;
		}
		 
		img= img.reshape(1, 1);
		trainingImages.push_back(img);
		trainingLabels.push_back(0);
	}
} 

 

int main()
{   
	////////训练样本/////////
	Mat classes; 
	Mat trainingData; 

	Mat trainingImages;
	vector<int> trainingLabels;

	getPlate(trainingImages, trainingLabels);
	getNoPlate(trainingImages, trainingLabels);

	Mat(trainingImages).copyTo(trainingData);
	trainingData.convertTo(trainingData, CV_32FC1);
	Mat(trainingLabels).copyTo(classes);

	CvSVMParams SVM_params;
	SVM_params.svm_type = CvSVM::C_SVC;
	SVM_params.kernel_type = CvSVM::LINEAR;  
	SVM_params.degree = 0;
	SVM_params.gamma = 1;
	SVM_params.coef0 = 0;
	SVM_params.C = 1;
	SVM_params.nu = 0;
	SVM_params.p = 0;
	SVM_params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.01);

	CvSVM svm(trainingData, classes, Mat(), Mat(), SVM_params);

	cout << "Svm generate done!" << endl;
		FileStorage fsTo("train/svm.xml", cv::FileStorage::WRITE);
	svm.write(*fsTo, "svm");

	////////识别测试/////////
	
	svm.load("svm.xml");

	Mat img1=imread("1.jpg",1);
	imshow("img1",img1);
	Mat p=img1.reshape(1,1);
	p.convertTo(p, CV_32FC1);
	cout<<"img1 is plate ? :";
	int response = (int)svm.predict( p );
	if(response==1)
		cout<<"	Yes"<<endl;
	else
		cout<<"No"<<endl;
	
	Mat img2=imread("2.jpg",1);
	imshow("img2",img2);
 	p=img2.reshape(1,1);
	p.convertTo(p, CV_32FC1);
	cout<<"img2 is plate ? :";
	response = (int)svm.predict( p );
	if(response==1)
		cout<<"Yes"<<endl;
	else
		cout<<"No"<<endl;

	//Mat img1=imread("1.jpg",1);
	//imshow("img1",img1);
	//cout<<"img1 is plate ? :";
	//classify(img1);


	//cout<<"img1 is plate ? :";
	//if(response==1)
	//	cout<<"yes"<<endl;
	//else
	//	cout<<"no"<<endl;
	/////////////////////////
	
	cvWaitKey();
	return 0;

}

