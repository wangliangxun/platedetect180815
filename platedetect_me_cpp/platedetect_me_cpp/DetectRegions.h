#ifndef DetectRegions_h
#define DetectRegions_h
#include <string.h>
#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>
using namespace cv;
using namespace std;

class DetectRegions
{
public:
	enum Color{BLUE,YELLOW};
	DetectRegions();
	string filename;
	void setFilename(string f);
	bool saveRegions;
	bool showSteps;
	Mat run(Mat input);
private:
	Mat findplate(Mat input);
	bool verifySizes(RotatedRect mr);
	bool calcSafeRect(const RotatedRect& roi_rect,const Mat& src,Rect_<float>& safeBoundRect);
	bool affine(Mat& in,Mat& out,const double slope);
	bool isdeflection(const Mat& in, const double angle, double& slope);
	bool rotation(Mat& in, Mat& out, const Size rect_size, const Point2f center, const double angle);
	Mat sobelyuchuli(Mat in);
	Mat colorMatch(const Mat& src,const Color r);
	Mat histeq(Mat in);
};
#endif




















