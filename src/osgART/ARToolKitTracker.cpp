#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>

#include <AR/gsub_lite.h>

#include <osgART/ARToolKitTracker>
#include <osgART/SingleMarker>
#include <osgART/MultiMarker>

#include <osgART/GenericVideo>



#include <iostream>
#include <fstream>


namespace osgART {

	ARToolKitTracker::ARToolKitTracker():
		threshold(100)
	{
	}

	ARToolKitTracker::~ARToolKitTracker()
	{
	    // Should delete list of markers...
	}


	bool ARToolKitTracker::init(int xsize, int ysize, 
		const std::string& pattlist_name, 
		const std::string& camera_name)
	{
		ARParam  wparam;
		
	    // Set the initial camera parameters.
		cparamName = camera_name;
	    if(arParamLoad((char*)cparamName.c_str(), 1, &wparam) < 0) {
	        fprintf(stderr, "ERROR: Camera parameter load error !!\n");
			return false;
	    }

	    arParamChangeSize(&wparam, xsize, ysize,(ARParam*)&cparam);
	    arInitCparam((ARParam*)&cparam);
	    arParamDisp((ARParam*)&cparam);

		arFittingMode = AR_FITTING_TO_IDEAL;
	    arImageProcMode = AR_IMAGE_PROC_IN_FULL;

		setProjection(10.0f, 8000.0f);
		setDebugMode(false);
		setThreshold(100);

		if (!setupMarkers(pattlist_name)) {
			fprintf(stderr, "ERROR: Marker setup failed.\n");
			return false;
		}

		// Success
		return true;
	}



	bool ARToolKitTracker::setupMarkers(const std::string& patternListFile)
	{
		std::ifstream markerFile;

		markerFile.open(patternListFile.c_str());

		// Need to check for error when opening file
		if (!markerFile.is_open()) return false;

		bool ret = true;

		int patternNum = 0;
		markerFile >> patternNum;

		std::string patternName, patternType;

		for (int i = 0; i < patternNum; i++)
		{
			markerFile >> patternName >> patternType;

			if (patternType == "SINGLE")
			{
				
				double width, center[2];
				markerFile >> width >> center[0] >> center[1];
				if (addSingleMarker(patternName, width, center) == -1) {
					fprintf(stderr, "Error adding single pattern: %s\n", patternName.c_str());
					ret = false;
					break;
				}

			}
			else if (patternType == "MULTI")
			{
				if (addMultiMarker(patternName) == -1) {
					fprintf(stderr, "Error adding multi-marker pattern: %s\n", patternName.c_str());
					ret = false;
					break;
				}

			} 
			else 
			{
				fprintf(stderr, "Unrecognized pattern type: %s.\n", patternType.c_str());
				ret = false;
				break;
			}
		}

		markerFile.close();

		return ret;
	}

	int ARToolKitTracker::addSingleMarker(const std::string& pattFile, double width, double center[2]) {

		Marker* singleMarker = new SingleMarker();
		if (!static_cast<SingleMarker*>(singleMarker)->initialise((char*)pattFile.c_str(), width, center))
		{
			delete singleMarker;
			return -1;
		}

		pattList.push_back(singleMarker);

		return pattList.size() - 1;

	}

	int ARToolKitTracker::addMultiMarker(const std::string& multiFile) 
	{
		Marker* multiMarker = new MultiMarker();
		
		if (!static_cast<MultiMarker*>(multiMarker)->initialise((char*)multiFile.c_str()))
		{
			delete multiMarker;
			return -1;
		}

		pattList.push_back(multiMarker);

		return pattList.size() - 1;

	}

	void ARToolKitTracker::setThreshold(int thresh)
	{
		// Clamp to 0-255
		threshold = thresh;
	}

	int ARToolKitTracker::getThreshold() {
		return threshold;
	}


	unsigned char* ARToolKitTracker::getDebugImage() {
		return arImage;
	}
		
	void ARToolKitTracker::setDebugMode(bool d) 
	{
		m_debugMode = d;
		if (m_debugMode) arDebug = 1;
		else arDebug = 0;
	}

	bool ARToolKitTracker::getDebugMode() 
	{
		return m_debugMode;
	}


	unsigned int ARToolKitTracker::getMarkerCount() {
		return pattList.size();
	}

	void ARToolKitTracker::update()
	{	

		ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
		int             marker_num;						// Count of number of markers detected.
	    int             j, k;

		// Do not update with a null image
		if (image == NULL) return;

		// Detect the markers in the video frame.
		if(arDetectMarker(image, threshold, &marker_info, &marker_num) < 0) 
		{
			fprintf(stderr, "Error detecting markers in image.\n");
			return;
		}
			
			
		// Check through the marker_info array for highest confidence
		// visible marker matching our preferred pattern.

		for (std::vector<Marker*>::const_iterator iter = pattList.begin(); iter != pattList.end(); iter++)		
		{
			
			Marker* currentMarker = (*iter);

			if (currentMarker->getType() == Marker::ART_SINGLE)
			{

				SingleMarker* singleMarker = static_cast<SingleMarker*>(currentMarker);

				k = -1;
				for (j = 0; j < marker_num; j++)	
				{
					if (singleMarker->getPatternID() == marker_info[j].id) 
					{
						if (k == -1) k = j; // First marker detected.
						else 
						if(marker_info[j].cf > marker_info[k].cf) k = j; // Higher confidence marker detected.
					}
				}
					
				if(k != -1) 
				{
					singleMarker->update(&marker_info[k]);
				} 
				else 
				{
					singleMarker->update(NULL);
				}
			}
			else if (currentMarker->getType() == Marker::ART_MULTI)
			{
				static_cast<MultiMarker*>(currentMarker)->update(marker_info, marker_num);
				
			}
		}
	}


#if 0
	void ARToolKitTracker::ARTransToGL(double para[3][4], double gl[16]) 
	{
	    for (int j = 0; j < 3; j++) 
		{
	        for (int i = 0; i < 4; i++) 
			{
	            gl[i*4+j] = para[j][i];
	        }
	    }

	    gl[3] = gl[7] = gl[11] = 0.0f;
	    gl[15] = 1.0f;

	}
#endif


	void ARToolKitTracker::setProjection(const double n, const double f) {
		arglCameraFrustum((ARParam*)&cparam, n, f, m_projectionMatrix);
	}

}; // namespace osgART
