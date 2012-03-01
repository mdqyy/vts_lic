#include "opencv2\opencv.hpp"
#include "ForegroundSegmentator.h"
#include "BlobTracker.h"
#include "BlobDetector.h"


using namespace cv;

typedef struct
{
	char* videoPath;
	int trainingFrames;

	ForegroundSegmentator* foregroundSegmentator;
	BlobDetector* blobDetector;
	BlobTracker* blobTracker;
	TrackHistory* trackHistory;

}AutoTrackerParam;

class AutoTracker
{

private:
	char* videoPath;
	ForegroundSegmentator* foregroungSegmentator;
	BlobDetector* blobDetector;
	BlobTracker* blobTracker;
	TrackHistory* trackHistory;

	VideoCapture capture;	
	bool openVideoCapture(int* fps = NULL, double* frameDelay = NULL);

public:
	AutoTracker(AutoTrackerParam param);
    ~AutoTracker();

	void process();

};