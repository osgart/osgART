// glew used by BazAR
#include <GL/glew.h>
// BAZAR
#include <garfeild.h>
#include "BazARTracker"

// OpenCV
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <cvaux.h>

// OpenGL
#include <GL/gl.h>
#include <GL/glut.h>
#include <AR/param.h>


#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <AR/gsub.h>
#ifndef AR_HAVE_HEADER_VERSION_2_72
#error ARToolKit v2.72 or later is required to build the OSGART ARToolKit tracker.
#endif

#include "BazARMarker"

#include <osgART/GenericVideo>

#include <iostream>
#include <fstream>



namespace osgART {

	struct BazARTracker::CameraParameter 
	{
		ARParam cparam;	
	};

	
	// Bazar
	IplImage *image = 0;
	IplImage *gray = 0;
	IplImage *display = 0;
	IplImage *FlipImage = 0;


	//BazARTracker::BazARTracker() : GenericTracker(),
	//	m_threshold(100),
	//	m_debugmode(false),
	//	m_marker_num(0),
	//	m_cparam(new CameraParameter)
	//{
	//	// attach a new field to the name "threshold"
	//	m_fields["threshold"] = new TypedField<int>(&m_threshold);
	//	// attach a new field to the name "debug"
	//	m_fields["debug"] = new TypedField<bool>(&m_debugmode);

	//	// for statistics
	//	m_fields["markercount"] = new TypedField<int>(&m_marker_num);

	//}

BazARTracker::BazARTracker() : GenericTracker(),
		m_debugmode(false),
		m_marker_num(0),
		m_cparam(new CameraParameter)
	{
		// attach a new field to the name "threshold"
		// m_fields["threshold"] = new TypedField<int>(&m_threshold);
		// attach a new field to the name "debug"
		m_fields["debug"] = new TypedField<bool>(&m_debugmode);

		// for statistics
		m_fields["markercount"] = new TypedField<int>(&m_marker_num);

	}


	BazARTracker::~BazARTracker()
	{
		delete m_cparam;
	}


	bool BazARTracker::init(int xsize, int ysize, const std::string& pattlist_name, const std::string& camera_name)
	{
		
		// init bazar tracker - MLee's code more or less..
		matCameraRT4_4 = cvCreateMat(4, 4, CV_64F); //64 bit double precision float
		g_matIntrinsic   = cvCreateMat(3, 3, CV_64F);
		
		cvNamedWindow("Gray", CV_WINDOW_AUTOSIZE);
		cvNamedWindow("Result_BAZAR", CV_WINDOW_AUTOSIZE);
		
		// fine tuning for accuracy - careful!!
		detector.ransac_dist_threshold = 5;
		detector.max_ransac_iterations = 800;
		detector.non_linear_refine_threshold = 2.5;
		
		
		// Train or load classifier
		if(!detector.build_with_cache(
			string("data/bazar/model.jpg"), // mode image file name
			400,               // maximum number of keypoints on the model
			32,                // patch size in pixels
			3,                 // yape radius. Use 3,5 or 7.
			16,                // number of trees for the classifier. Somewhere between 12-50
			3                  // number of levels in the gaussian pyramid
			))
		{
		cerr << "Unable to load the model image " << "data/bazar/model.jpg" <<" or its classifier.\n";
		return false;
		}


	

		// A lower threshold will allow detection in harder conditions, but
		// might lead to false positives.
		detector.match_score_threshold=.03f;
		 
		// Read camera parameters for BAZAR
		char	*camCal = "data/bazar/camera_c.txt";
		char	*camExt = "data/bazar/camera_r_t.txt";
		if(!augment.LoadOptimalStructureFromFile(camCal, camExt))
		{
			std::cerr << "ERROR: BazAR couldn't load camera parameters." << std::endl;
			return false;
		}
		
		// image buffers needed for detection, conversion..
		image     = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 4);
		FlipImage = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 4);
		gray      = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 1);
		display   = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 4);


		ARParam  wparam;
	    // Set the initial camera parameters.
		cparamName = camera_name;
	    if(arParamLoad((char*)cparamName.c_str(), 1, &wparam) < 0) {
			std::cerr << "ERROR: Camera parameter load error." << std::endl;
			return false;
	    }
	    arParamChangeSize(&wparam, xsize, ysize,&(m_cparam->cparam));


		
		// Should be adjusted according to your calibration result - open "camera_c.txt"
		//m_cparam->cparam.mat[0][0] = 505.312511;	m_cparam->cparam.mat[0][1] =   0.000000;    m_cparam->cparam.mat[0][2] = 322.794720;
		//m_cparam->cparam.mat[1][0] =   0.000000;  m_cparam->cparam.mat[1][1] = 487.186810;    m_cparam->cparam.mat[1][2] = 92.396145;

		// this is for debugging only, set to minkyungs values to compare avi-input
		// Should be adjusted according to your calibration result - open "camera_c.txt"
		m_cparam->cparam.mat[0][0] = 410.715270;	m_cparam->cparam.mat[0][1] =   0.000000;    m_cparam->cparam.mat[0][2] = 165.518684;
		m_cparam->cparam.mat[1][0] =   0.000000;  m_cparam->cparam.mat[1][1] = 413.258835;    m_cparam->cparam.mat[1][2] = 166.483574;
		// distortion params
		//m_cparam->cparam.dist_factor[0] = 159.25;	m_cparam->cparam.dist_factor[1] = 131.75;	m_cparam->cparam.dist_factor[2] = 104.80;	m_cparam->cparam.dist_factor[3] = 1.012757;




		for(int i=0; i<3; i++)
		{
			for(int j=0; j<3; j++)
			{
				cvmSet(g_matIntrinsic, i, j, m_cparam->cparam.mat[i][j]);
			}
		}

	    arInitCparam(&(m_cparam->cparam));
	    arParamDisp(&(m_cparam->cparam));

		setProjection(10.0f, 8000.0f);
		setDebugMode(m_debugmode);
		//setThreshold(m_threshold);

		setupMarkers();

		cout << "init success" << endl;

		// Success
		return true;
	}


std::string trim(std::string& s,const std::string& drop = " ")
{
	std::string r=s.erase(s.find_last_not_of(drop)+1);
	return r.erase(0,r.find_first_not_of(drop));
}


	bool BazARTracker::setupMarkers()
	{
		bool ret = true;
		addBazARMarker();
		return true;
	}

	int BazARTracker::addBazARMarker() {

		BazARMarker* bazarMarker = new BazARMarker();

		if (!bazarMarker->initialise())
		{
			return -1;
		}

		m_markerlist.push_back(bazarMarker);

		return m_markerlist.size() - 1;
	}

	unsigned char* BazARTracker::getDebugImage() {
		return arImage;
	}
		
	void BazARTracker::setDebugMode(bool d) 
	{
		m_debugmode = d;
		arDebug = (m_debugmode) ? 1 : 0;
	}

	bool BazARTracker::getDebugMode() 
	{
		return m_debugmode;
	}

    /*virtual*/ 
	void BazARTracker::setImageRaw(unsigned char * image, PixelFormatType format)
    {
		if (m_imageptr_format != format) {
			// format has changed.
			// Translate the pixel format to an appropriate type for ARToolKit v2.
			switch (format) {
				case VIDEOFORMAT_RGB24:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_RGB;
					m_artoolkit_pixsize = 3;
					break;
				case VIDEOFORMAT_BGR24:
					m_artoolkit_pixformat = AR_PIXEL_FORMAT_BGR;
					m_artoolkit_pixsize = 3;
					std::cerr<<"im getting bgr.."<<std::endl;
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
		}
		
        // We are only augmenting method in parent class.
        GenericTracker::setImageRaw(image, format);
    }

	void BazARTracker::update()
	{
		ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
		register int             j, k;

		// Do not update with a null image
		if (m_imageptr == NULL) return;

		// Image format conversion for BAZAR
		for(int i=0; i<m_cparam->cparam.xsize*m_cparam->cparam.ysize*4; i++)
		{
			image->imageData[i] = m_imageptr[i];
		}
		
		// Image format change for processing color->gray
		if(image->nChannels == 4) 
		{
			cvCvtColor(image, gray, CV_RGBA2GRAY);
		} 
		else if(image->nChannels == 3) 
		{
			cvCvtColor(image, gray, CV_BGR2GRAY);
		} 
		else 
		{
			gray = image;
		}
		
		cvFlip(gray, gray);				// bazar uses hoizontially flipped input images
		cvShowImage("Gray", gray);		// show what bazar will see
		

		// if the trained planar surface is detected
		if (detector.detect(gray))  
		{
			// start on a new frame
			augment.Clear();

			// we have only 1 camera
			add_detected_homography(detector, augment);

			// bundle adjust
			augment.Accomodate(4, 1e-4);
		
			show_result(augment, image, &display);	// render bazar output and get GL-projection matrix
			cvShowImage("Result_BAZAR",display);

			(static_cast<BazARMarker*>(m_markerlist[0].get()))->update(matCameraRT4_4);
		} 
		else 
		{
			cvShowImage("Result_BAZAR", display);
			
			(static_cast<BazARMarker*>(m_markerlist[0].get()))->update(NULL);
		}
	}

	void BazARTracker::setProjection(const double n, const double f) 
	{
		//GSUB_LITE - NEW ART STYLE
		arglCameraFrustumRH(&(m_cparam->cparam), n, f, m_projectionMatrix);
	}

/*
 * \param Intrinsic  : 3 by 3 camera intrinsic matrix  
 * \param bazProjMat : 3 by 4 projection matrix  = K[RT] obtained by calling "augment.GetProjectionMatrix(0)" function
 * \arRT : 4 by 4 [R t] matrix for rendering
*/
	void BazARTracker::GetARToolKitRTfromBAZARProjMat(CvMat *Intrinsic, CvMat *bazProjMat, CvMat *arRT)
	{
		int i, j;
		CvMat *temps = cvCreateMat(3, 4, CV_64F);
	
		CvMat *invK = cvCloneMat(Intrinsic);
		cvInv(Intrinsic, invK);
		cvMatMul(invK, bazProjMat, temps);

		cvmSetIdentity(arRT);
		for(i=0; i<temps->rows; i++)
		{
			for(j=0; j<temps->cols;j++)  cvmSet(arRT, i, j, cvmGet(temps, i,j));
		}
	
		cvReleaseMat(&temps);
		cvReleaseMat(&invK);
	}


/**
 * Overwriting BazAR's show_result function
 * X, Y, Z axis --> Red, Green, Blue
**/

	void BazARTracker::show_result(CamAugmentation &augment, IplImage *video, IplImage **dst)
	{
		int i, j;
		if (*dst==0) *dst=cvCloneImage(video);
		else cvCopy(video, *dst);

		CvMat *m  = augment.GetProjectionMatrix(0);
	
		// Flip...(This occured from OpenGL origin / camera origin)
		// 
		
		CvMat *coordinateTrans = cvCreateMat(3, 3, CV_64F);
		cvmSetIdentity(coordinateTrans);
		cvmSet(coordinateTrans, 1, 1, -1);
		cvmSet(coordinateTrans, 1, 2, m_cparam->cparam.ysize);
		cvMatMul(coordinateTrans, m, m);
	
		// extract intrinsic camera parameters from bazar's projection matrix..
		GetARToolKitRTfromBAZARProjMat(g_matIntrinsic, m, matCameraRT4_4);
			
		cvTranspose(matCameraRT4_4, matCameraRT4_4);
		cvReleaseMat(&coordinateTrans);

		// draw the coordinate system axes

		double w =video->width/2.0;
		double h =video->height/2.0;

		// 3D coordinates of an object
		double pts[4][4] = {
			{w,h,0, 1}, // 0,0,0,1
			{w*2,h,0, 1}, // w, 0
			{w,h*2,0, 1}, // 0, h
			{w,h,-w-h, 1} // 0, 0, -
		};

		CvMat ptsMat, projectedMat;
		cvInitMatHeader(&ptsMat, 4, 4, CV_64FC1, pts);
		cvInitMatHeader(&projectedMat, 3, 4, CV_64FC1, projected);
	
		cvGEMM(m, &ptsMat, 1, 0, 0, &projectedMat, CV_GEMM_B_T );

		for (int i=0; i<4; i++) 
		{
			projected[0][i] /= projected[2][i];
			projected[1][i] /= projected[2][i];
		}

		// draw the projected lines
		cvLine(*dst, cvPoint((int)projected[0][0], (int)projected[1][0]),
			cvPoint((int)projected[0][1], (int)projected[1][1]), CV_RGB(255,0,0), 2);
		cvLine(*dst, cvPoint((int)projected[0][0], (int)projected[1][0]),
			cvPoint((int)projected[0][2], (int)projected[1][2]), CV_RGB(0,255,0), 2);
		cvLine(*dst, cvPoint((int)projected[0][0], (int)projected[1][0]),
			cvPoint((int)projected[0][3], (int)projected[1][3]), CV_RGB(0,0,255), 2);

	}
	
	
	bool BazARTracker::add_detected_homography(planar_object_recognizer &detector, CamAugmentation &a)
	{
		static std::vector<CamCalibration::s_struct_points> pts;
		pts.clear();

		for (int i=0; i<detector.match_number; ++i) {
			image_object_point_match * match = detector.matches+i;
			if (match->inlier) {
				pts.push_back(CamCalibration::s_struct_points(
					PyrImage::convCoordf(match->image_point->u, int(match->image_point->scale), 0),
					PyrImage::convCoordf(match->image_point->v, int(match->image_point->scale), 0),
					PyrImage::convCoordf(match->object_point->M[0], int(match->object_point->scale), 0),
					PyrImage::convCoordf(match->object_point->M[1], int(match->object_point->scale), 0)));
			}
		}

		a.AddHomography(pts, detector.H);
		return true;
	}


}; // namespace osgART
