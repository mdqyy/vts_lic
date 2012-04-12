#include "PTracker.h"
#include "Draw.h"
#include "Helper.h"

void PTracker::start()
{
	long trainingFrames = 100;
	int fps;
	double frameDelay;
	int frameCount = 0;

	if(!video.openCapture(fps, frameDelay))
		return;

	IdGenerator idGenerator(0);

	int bufferSize = 10;
	vector<track> tracks;
	vector<Mat> frameBuffer;
	vector<Mat> foregroundBuffer;
	vector<vector<detection>> detectionBuffer;

	Mat frame;
	while(video.read(frame))
	{
		frameCount++;
		if(video.frameCount < trainingFrames)
		{
			subtractor->learn(frame);
		}else{
			Mat foreground = subtractor->segment(frame);
			ClassifierParams cparam = {frame};
			auto frameDetections = classifier->detect(cparam);

			//////////////////////////////////////////////////////////////
			frameBuffer.push_back(frame.clone());
			foregroundBuffer.push_back(foreground.clone());
			detectionBuffer.push_back(frameDetections);
			if(frameBuffer.size() > bufferSize)
			{				
				frameBuffer[0].release(); 
				foregroundBuffer[0].release();
				detectionBuffer[0].clear();

				frameBuffer.erase(frameBuffer.begin());
				foregroundBuffer.erase(foregroundBuffer.begin());
				detectionBuffer.erase(detectionBuffer.begin());
			}else
				continue;

			//////////////////////////////////////////////////////////////debug draw
			auto detections = detectionBuffer[1];
			auto currentFrame = frameBuffer[1];
			auto prevFrame = frameBuffer[0];
			auto currentForeground = foregroundBuffer[1];			

			//filter detections
			//keep only continuous detections
			//look ahead couple of frames to see if the detections persist
			//for now Hog is stable, so we have none false positive detections

			//tracker region; the hard stuff			
			if(tracks.size() == 0 && detections.size() > 0)
			{
				tracks.clear();
				tracks.reserve(detections.size());
				for_each(begin(detections), end(detections), [&](const detection& d){
					track t = {idGenerator.nextId(), d.rect};
					tracks.push_back(t);
				});
				continue;
			}


			//should first propagate tracks via lucas kanade filter
			//track the prev tracks using short term lucas kanade (choose points using fb and NCC)
			//merge detection(if exists) with short term tracker, thus providing measurement for Kalman filter
			//if short term tracker is unreliable then use Kalman to track

			auto it = tracks.begin();
			auto end = tracks.end();
			for(; it != end; ++it)
			{
				trackLK(*(it), frameBuffer, foregroundBuffer);
			}

			std::map<int, vector<int>> trMatches; 
			std::map<int, vector<int>> detMatches;

			for_each(begin(tracks), std::end(tracks), [&](track& tr){
				int minArea = 0.5 * tr.rect.area();

				auto dit = detections.begin();
				auto dend = detections.end();
				for(;dit != dend; ++dit)
				{
					Rect intersection = tr.rect & dit->rect;
					if(intersection.area() > minArea)
					{												
						tr.rect = dit->rect;
						trMatches[tr.id].push_back(dit->id);
						detMatches[dit->id].push_back(tr.id);

						imshow("matches", Helper::concatImages(prevFrame(tr.rect), currentFrame(dit->rect)));
						//cv::waitKey();
					}						
				}					
			});

			for_each(begin(detections), std::end(detections), [&](detection& d){
				auto it = detMatches.find(d.id);
				if(it == detMatches.end())
				{
					auto track = initTrackFromDetection(d, idGenerator);
					tracks.push_back(track);
				}
			});

			auto fclone = currentFrame.clone();
			Helper::drawDetections(detections, fclone);
			imshow("detections", fclone);
			fclone.release();

			fclone = currentFrame.clone();
			Helper::drawTracks(tracks, fclone);
			imshow("tracks", fclone);
			fclone.release();



			//for_each(begin(tracks), end(tracks), [&](track& tr){
			//	imshow("track", currentFrame(tr.rect));			
			//	auto matches = trMatches[tr.id];
			//	if(matches.size() > 0)
			//	{
			//		
			//		imshow("detection", currentFrame(detections[matches[0]].rect));
			//	}

			//	cv::waitKey();
			//});

			fclone.release();
		}

		//imshow("frame", frame);
		cv::waitKey(frameDelay);
	
	}
}

track PTracker::initTrackFromDetection( const detection& d,IdGenerator& gen )
{
	track t = {gen.nextId(), d.rect};
	return t;
}

const int XCount = 15;
const int YCount = 15;

void PTracker::trackLK( track& tr, vector<Mat> frames, vector<Mat> foregrounds )
{

	Mat currentFrame = frames[0];
	Mat currentForeground = foregrounds[0];
	Mat trackForeground = currentForeground(tr.rect);

	//Mat currentGray;
	//Mat nextGray;	
	//cv::cvtColor(frames[0], currentGray, CV_BGR2GRAY);
	//cv::cvtColor(frames[1], nextGray, CV_BGR2GRAY);

	//get points in grid pattern from foreground
	int xoffset = tr.rect.width / XCount;
	int yoffset = tr.rect.height/ YCount;

	vector<Point> trackedPoints; 
	//for(int i = 0; i < XCount; i++)
	//	for(int j = 0; j < YCount; j++)
	//	{
	//		int x = xoffset*i;
	//		int y = yoffset*j;
	//		char value = trackForeground.at<char>(x,y);
	//		if( value == 255)
	//			trackedPoints.push_back(Point(x,y));			
	//	}

	for(int i = 0; i < trackForeground.rows; i++)
		for(int j = 0; j < trackForeground.cols; j++)
		{
			char value = trackForeground.at<char>(i,j);
			if( value == 255)
				trackedPoints.push_back(Point(i,j));			
		}

	cout << trackForeground << endl;

	Mat fclone = currentFrame(tr.rect).clone();
	Helper::drawPoints(trackedPoints, fclone);
	imshow("tracked points", fclone);
	imshow("track foreground", trackForeground);
	cv::waitKey();
	fclone.release();
}
