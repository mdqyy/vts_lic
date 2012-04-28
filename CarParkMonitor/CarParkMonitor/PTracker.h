#pragma once
#include "Video.h"
#include "BgSubtractorBase.h"
#include "ClassifierBase.h"
#include "TrackMatcher.h"
#include "KalmanFilter2D.h"

class PTracker
{
private:
	Rect frameRect;
	int frameCount;
	Video& video;
	BgSubtractorBase* subtractor;
	ClassifierBase* classifier;
	TrackMatcher* matcher;

	vector<track> tracks;	
	std::map<int, shared_ptr<KalmanFilter2D> > kalmanFilters;

	track initTrackFromDetection(const detection& d,IdGenerator& gen, Mat& frame);
	track initializeTrack(detection& det, IdGenerator& idGenerator, Mat& grayFrame);
	void deleteTrack(track& tr);

	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<detection>> detectionBuffer;
	bool shiftBuffers(Mat frame);
	bool trackLucasKanade(track& tr, vector<Mat> frames, vector<Mat> grayFrames, vector<Mat> foregrounds,Rect& predictedRect, Mat& output);		
	bool predictKalman(track& tr, Rect& predictedRect);
	void forwardKalman(track& tr);
	void mergePredictions(bool lkSuccess, bool kalmanSuccess, track& tr,Rect& lkRect,Rect& kalmanRect,vector<Mat> frameBuffer);
	Mat lbpHist(Mat& grayFrame);

	void correctKalman(track& tr);
	bool trackHasExited(track& tr, Mat frame);

public:
	~PTracker(){};
	PTracker(Video& vid, BgSubtractorBase* subtr, ClassifierBase* cls, TrackMatcher* mat):
		video(vid),
		subtractor(subtr),
		classifier(cls),
		matcher(mat)
	{};
	
	void start();
};