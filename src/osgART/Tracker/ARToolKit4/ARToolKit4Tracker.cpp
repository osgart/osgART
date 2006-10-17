#include "ARToolKit4Tracker"

#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#ifndef AR_HAVE_HEADER_VERSION_4_1
#error ARToolKit v4.1 or later is required to build the OSGART ARToolKit4 tracker.
#endif

#include "SingleMarker"
#include "MultiMarker"

#include <osgART/GenericVideo>

#include <iostream>
#include <fstream>


#define PD_LOOP 3

template <typename T> 
int Observer2Ideal(	const T dist_factor[4], 
					const T ox, 
					const T oy,
					T *ix, T *iy )
{
    T  z02, z0, p, q, z, px, py;
    register int i = 0;

    px = ox - dist_factor[0];
    py = oy - dist_factor[1];
    p = dist_factor[2]/100000000.0;
    z02 = px*px+ py*py;
    q = z0 = sqrt(px*px+ py*py);

    for( i = 1; ; i++ ) {
        if( z0 != 0.0 ) {
            z = z0 - ((1.0 - p*z02)*z0 - q) / (1.0 - 3.0*p*z02);
            px = px * z / z0;
            py = py * z / z0;
        }
        else {
            px = 0.0;
            py = 0.0;
            break;
        }
        if( i == PD_LOOP ) break;

        z02 = px*px+ py*py;
        z0 = sqrt(px*px+ py*py);
    }

    *ix = px / dist_factor[3] + dist_factor[0];
    *iy = py / dist_factor[3] + dist_factor[1];

    return(0);
}


namespace osgART {

	struct ARToolKit4Tracker::CameraParameter 
	{
		ARParam cparam;	
	};


	ARToolKit4Tracker::ARToolKit4Tracker() : GenericTracker(),
		m_threshold(AR_DEFAULT_LABELING_THRESH),
		m_debugmode(AR_DEFAULT_DEBUG_MODE),
		m_marker_num(0),
		m_cparam(new CameraParameter),
		m_artoolkit_pixformat(AR_DEFAULT_PIXEL_FORMAT),
		m_artoolkit_pixsize(0),
		m_arhandle(NULL),
		m_ar3dhandle(NULL),
		m_arPattHandle(NULL)
	{
		// attach a new field to the name "threshold"
		m_fields["threshold"] = new TypedField<int>(&m_threshold);
		// attach a new field to the name "debug"
		m_fields["debug"] = new TypedField<bool>(&m_debugmode);

		// for statistics
		m_fields["markercount"] = new TypedField<int>(&m_marker_num);
	}

	ARToolKit4Tracker::~ARToolKit4Tracker()
	{
		cleanupMarkers();
		if (m_ar3dhandle) {
			ar3DDeleteHandle(m_ar3dhandle);
			m_ar3dhandle = NULL;
		}
		if (m_arhandle) {
			arDeleteHandle(m_arhandle);
			m_arhandle = NULL;
		}
		delete m_cparam;
	}


	bool ARToolKit4Tracker::init(int xsize, int ysize, 
		const std::string& pattlist_name, 
		const std::string& camera_name)
	{
		ARParam  wparam;
		
	    // Set the initial camera parameters.
		cparamName = camera_name;
	    if(arParamLoad((char*)cparamName.c_str(), 1, &wparam) < 0) {
			std::cerr << "ERROR: Camera parameter load error." << std::endl;
			return false;
	    }

	    arParamChangeSize(&wparam, xsize, ysize,&(m_cparam->cparam));
        std::cout <<  "*** Camera Parameter ***" << std::endl;
        arParamDisp(&(m_cparam->cparam));

        if ((m_arhandle = arCreateHandle(&(m_cparam->cparam))) == NULL) {
            std::cerr << "Error: arCreateHandle." << std::endl;
			return false;
        }
		if (arSetPixelFormat(m_arhandle, AR_DEFAULT_PIXEL_FORMAT) < 0) {
			std::cerr << "Error: arSetPixelFormat." << std::endl;
			return false;
		}
        if (arSetDebugMode(m_arhandle, m_debugmode) < 0) {
            std::cerr << "Error: arSetDebugMode." << std::endl;
			return false;
        }
        if (arSetImageProcMode(m_arhandle, AR_DEFAULT_IMAGE_PROC_MODE) < 0) { // AR_IMAGE_PROC_FRAME_IMAGE (normal) or AR_IMAGE_PROC_FIELD_IMAGE (Interlaced .. eg. DVCam).
            std::cerr << "Error: arSetDebugMode." << std::endl;
			return false;
        }
        if (arSetLabelingThresh(m_arhandle, m_threshold) < 0) {
            std::cerr << "Error: arSetDebugMode." << std::endl;
			return false;
        }
        if ((m_ar3dhandle = ar3DCreateHandle(&(m_cparam->cparam))) == NULL) {
            std::cerr << "Error: ar3DCreateHandle." << std::endl;
			return false;
        }
        
		setProjection(10.0f, 8000.0f);

		if (!setupMarkers(pattlist_name)) {
			std::cerr << "ERROR: Marker setup failed." << std::endl;
			return false;
		}

		// Success
		return true;
	}


std::string trim(std::string& s,const std::string& drop = " ")
{
	std::string r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}


	bool ARToolKit4Tracker::setupMarkers(const std::string& patternListFile)
	{
		// Need to check whether the passed file even exists.
		// Need to check for error when opening file.
		std::ifstream markerFile;
		markerFile.open(patternListFile.c_str());
		if (!markerFile.is_open()) return false;

		if ((m_arPattHandle = arPattCreateHandle()) == NULL) {
			std::cerr << "setupMarkers(): Error: arPattCreateHandle." << std::endl;
			markerFile.close();
			return (false);
		}
		
		bool ret = true;

		int patternNum = 0;
		markerFile >> patternNum;
		std::string patternName, patternType;
		for (int i = 0; (i < patternNum) && (!markerFile.eof()); i++) {
			// Get the whole line for the marker file (will handle spaces in filename).
			patternName = "";
			while (trim(patternName) == "" && !markerFile.eof()) {
				getline(markerFile, patternName);
			}
			
			// Check whether markerFile exists?
			markerFile >> patternType;

			if (patternType == "SINGLE") {
				double width, center[2];
				markerFile >> width >> center[0] >> center[1];
				if (addSingleMarker(patternName, width) == -1) {
					std::cerr << "Error adding single pattern: " << patternName << std::endl;
					ret = false;
					break;
				}
			} else if (patternType == "MULTI") {
				if (addMultiMarker(patternName) == -1) {
					std::cerr << "Error adding multi-marker pattern: " << patternName << std::endl;
					ret = false;
					break;
				}
			} else {
				std::cerr << "Unrecognized pattern type: " << patternType << std::endl;
				ret = false;
				break;
			}
		}

		markerFile.close();
		arPattAttach(m_arhandle, m_arPattHandle);

		// Cleanup from any errors.
		if (!ret) {
			cleanupMarkers();
		}
		return ret;
	}

	void ARToolKit4Tracker::cleanupMarkers(void) {
		arPattDetach(m_arhandle);
		// Need to unload markers here.
		//
		if (m_arPattHandle) {
			arPattDeleteHandle(m_arPattHandle);
			m_arPattHandle = NULL;
		}
	}

	int 
	ARToolKit4Tracker::addSingleMarker(const std::string& pattFile, double width) {

		SingleMarker* singleMarker = new SingleMarker();

		if (!singleMarker->initialise(m_arPattHandle, pattFile, width)) {
			singleMarker->unref();
			return -1;
		}

		m_markerlist.push_back(singleMarker);

		return m_markerlist.size() - 1;
	}

	int 
	ARToolKit4Tracker::addMultiMarker(const std::string& multiFile) 
	{
		MultiMarker* multiMarker = new MultiMarker();
		
		if (!multiMarker->initialise(multiFile)) {
			multiMarker->unref();
			return -1;
		}

		m_markerlist.push_back(multiMarker);

		return m_markerlist.size() - 1;

	}

	void ARToolKit4Tracker::setThreshold(int thresh)	{
		m_threshold = osg::clampBetween(thresh,0,255);
		arSetLabelingThresh(m_arhandle, m_threshold);
	}

	int ARToolKit4Tracker::getThreshold() {
		return m_threshold;
	}

	unsigned char* ARToolKit4Tracker::getDebugImage() {
		if (!m_arhandle) return NULL;
		return ((unsigned char *)&(m_arhandle->labelInfo.bwImage));
	}
		
	void ARToolKit4Tracker::setDebugMode(bool d) 
	{
		m_debugmode = d;
		arSetDebugMode(m_arhandle, (m_debugmode) ? 1 : 0);
	}

	bool ARToolKit4Tracker::getDebugMode() 
	{
		return m_debugmode;
	}

    /*virtual*/ 
	void ARToolKit4Tracker::setImageRaw(unsigned char * image, PixelFormatType format)
    {
		if (m_imageptr_format != format) {
			// format has changed.
			// Translate the pixel format to an appropriate type for ARToolKit v4.
			switch (format) {
				case VIDEOFORMAT_RGB24:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_RGB;
					m_artoolkit_pixsize = 3;
					break;
				case VIDEOFORMAT_BGR24:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_BGR;
					m_artoolkit_pixsize = 3;
					break;
				case VIDEOFORMAT_BGRA32:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_BGRA;
					m_artoolkit_pixsize = 4;
					break;
				case VIDEOFORMAT_RGBA32:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_RGBA;
					m_artoolkit_pixsize = 4;
					break;
				case VIDEOFORMAT_ARGB32:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_ARGB;
					m_artoolkit_pixsize = 4;
					break;
				case VIDEOFORMAT_ABGR32:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_ABGR;
					m_artoolkit_pixsize = 4;
					break;
				case VIDEOFORMAT_YUV422:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_2vuy;
					m_artoolkit_pixsize = 2;
					break;
				case VIDEOFORMAT_Y8:
				case VIDEOFORMAT_GREY8:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_MONO;
					m_artoolkit_pixsize = 1;
					break;
				default:
					break;
			}
			if (arSetPixelFormat(m_arhandle, m_artoolkit_pixformat) < 0) {
				std::cerr << "setImageRaw: Error: arSetPixelFormat." << std::endl;
				//return (false);
			}			
		}
		
		// We are only augmenting method in parent class.
		GenericTracker::setImageRaw(image, format);
	}

    void ARToolKit4Tracker::update()
	{	

		ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
		
	    register int             j, k;

		// Do not update with a null image
		if (m_imageptr == NULL) return;

		// Detect the markers in the video frame.
		if (arDetectMarker(m_arhandle, m_imageptr) < 0) {
			std::cerr << "Error detecting markers in image." << std::endl;
			return;
		}

		MarkerList::iterator _end = m_markerlist.end();
			
		// Check through the marker_info array for highest confidence
		// visible marker matching our preferred pattern.
		for (MarkerList::iterator iter = m_markerlist.begin(); 
			iter != _end; 
			++iter)		
		{
			
			SingleMarker* singleMarker = dynamic_cast<SingleMarker*>((*iter).get());
			MultiMarker* multiMarker = dynamic_cast<MultiMarker*>((*iter).get());

			if (singleMarker) {			

				k = -1;
				for (j = 0; j < m_arhandle->marker_num; j++){
					if (singleMarker->getPatternID() == m_arhandle->markerInfo[j].id) {
						if (k == -1) k = j; // First marker detected.
						else if (m_arhandle->markerInfo[j].cf > m_arhandle->markerInfo[k].cf) k = j; // Higher confidence marker detected.
					}
				}
					
				if (k != -1) {
					singleMarker->update(m_ar3dhandle, &(m_arhandle->markerInfo[k])); 
				} else {
					singleMarker->update(m_ar3dhandle, NULL);
				}
			} else if (multiMarker) {
				multiMarker->update(m_ar3dhandle, (ARMarkerInfo *)&(m_arhandle->markerInfo), m_marker_num);
			} else {
				std::cerr << "ARToolKitTracker::update() : Unknown marker type id!" << std::endl;
			}
		}
	}

	void ARToolKit4Tracker::setProjection(const double n, const double f) 
	{
		arglCameraFrustumRH(&(m_cparam->cparam), n, f, m_projectionMatrix);
	}

	void ARToolKit4Tracker::createUndistortedMesh(
		int width, int height,
		float maxU, float maxV,
		osg::Geometry &geometry)
	{

		osg::Vec3Array *coords = dynamic_cast<osg::Vec3Array*>(geometry.getVertexArray());
		osg::Vec2Array* tcoords = dynamic_cast<osg::Vec2Array*>(geometry.getTexCoordArray(0));
						
		unsigned int rows = 20, cols = 20;
		float rowSize = height / (float)rows;
		float colSize = width / (float)cols;
		double x, y, px, py, u, v;

		for (unsigned int r = 0; r < rows; r++) {
			for (unsigned int c = 0; c <= cols; c++) {

				x = c * colSize;
				y = r * rowSize;

				Observer2Ideal(m_cparam->cparam.dist_factor, x, y, &px, &py);
				coords->push_back(osg::Vec3(px, py, 0.0f));

				u = (c / (float)cols) * maxU;
				v = (1.0f - (r / (float)rows)) * maxV;
				tcoords->push_back(osg::Vec2(u, v));

				x = c * colSize;
				y = (r+1) * rowSize;

				Observer2Ideal(m_cparam->cparam.dist_factor, x, y, &px, &py);
				coords->push_back(osg::Vec3(px, py, 0.0f));

				u = (c / (float)cols) * maxU;
				v = (1.0f - ((r+1) / (float)rows)) * maxV;
				tcoords->push_back(osg::Vec2(u, v));

			}

			geometry.addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUAD_STRIP, 
				r * 2 * (cols+1), 2 * (cols+1)));
		}
	}

}; // namespace osgART
