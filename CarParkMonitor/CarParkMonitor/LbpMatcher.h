#pragma once
#include <opencv2/opencv.hpp>
#include <map>
#include "TrackMatcher.h"

using namespace std;
using namespace cv;

class LbpMatcher: public TrackMatcher
{
private:	
	map<int, Mat> detectionHists;	
	Mat lbpHist(Mat& src);

public:
	LbpMatcher(){}
	~LbpMatcher(){}

	void begin() { detectionHists.clear(); }
	float match(track& tr, detection& dt, Mat& frame);
	void inferModel(track& tr, detection& dt, Mat& frame);
	float distance(track& tr, Mat& region);
	float distance(track& tr, Mat& region, Mat& descriptor);	
};