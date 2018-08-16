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













