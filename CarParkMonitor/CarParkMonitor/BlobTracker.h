#pragma once
#include "Blob.h"
#include "TrackHistory.h"

#define NO_MATCH -1

typedef struct
{
	int frameCount;

	Mat frame;
	Mat grayFrame;	
	Mat foreground;
				
	IdGenerator* generator;
	vector<blob*> detectedBlobs;

	int frameBufferSize;
	vector<Mat> frameBuffer;
	vector<Mat> grayFrameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<shared_ptr<blob>>> blobBuffer;

}TrackerParam;

typedef struct
{
	vector<shared_ptr<blob>> newBlobs;
	vector<shared_ptr<blob>> prevLostBlobs;

	void init()
	{
		newBlobs.clear();
		prevLostBlobs.clear();
	}

}MatcherResult;

class BlobTracker
{
private:
	TrackHistory* trackHistory;
	FeatureDetector* featureDetector;
	DescriptorExtractor* descriptorExtractor;
	TrackerParam trackerParam;
	
	void forewardBackwardTrack(Rect r, vector<Mat> frames);
	void forewardTrack(vector<Point2f> points, vector<Mat> frames, vector<Point2f>* result);
	void trackBackward(vector<Point2f> futurePoints, vector<Mat> frames, vector<Point2f>* result);
	void filterInliers(vector<Point2f> startPoints, vector<Point2f> backTrackedPoints, vector<Point2f>& result);
	vector<Point2f> getGoodFeatures(Mat image, Mat mask);

	void debugDraw(const char* message, vector<Point2f> points, const Mat& output);

public:
	BlobTracker(void);
	BlobTracker(TrackHistory* trackHistory);
	~BlobTracker(void);	

	void track(TrackerParam params, MatcherResult* result);	
};