#pragma once
#include <opencv2/opencv.hpp>

using namespace cv;

class Tool
{
private: 
	static float Pi;
	static float Pi4;

	Tool(void){}
	~Tool(void){}
public:
	
	static void moveRect(Rect& r, float x, float y);
	static float median(float* arr, int len);
	static void extend(Rect& r, Point dims);
	static void toCenter(const Point& center, Rect& r);
	static Point rectCenter(const Rect& r);
	static bool rectInside(const Rect& r, const Rect& container);
	static bool isMostlyCircular(const vector<Point>& contour, float treshold = 0.6f);
};
