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

// OSGART
#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <AR/gsub.h>
#ifndef AR_HAVE_HEADER_VERSION_2_72
#error ARToolKit v2.72 or later is required to build the OSGART ARToolKit tracker.
#endif

//BAZAR
#include "BazARMarker"
//OSGART
#include <osgART/GenericVideo>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>


// use this for string -> number conversion
template <class T>
bool from_string(T& t, 
				const std::string& s, 
				std::ios_base& (*f)(std::ios_base&))
{
std::istringstream iss(s);
return !(iss >> f >> t).fail();
}

 


namespace osgART {

	/*BazarConfigParameter::BazarConfigParameter(void){
			camCalFileName = (char *)malloc( sizeof(char)*255 );
			camExtFileName = (char *)malloc( sizeof(char)*255 );
			modelFileName = (char *)malloc( sizeof(char)*255 );
			ransac_dist_threshold = 0;
			max_ransac_iterations = 0;
			non_linear_refine_threshold = 0;
			match_score_threshold = 0;
	}
	BazarConfigParameter::~BazarConfigParameter()
	{
		free(camCalFileName);
		free(camExtFileName);
		free(modelFileName);
	}*/

	struct BazARTracker::CameraParameter 
	{
		ARParam cparam;	
	};
	
	// Bazar's opencv images
	IplImage *image = 0;		// captured image
	IplImage *gray = 0;			// detector input
	IplImage *display = 0;		// debug

	BazARTracker::BazARTracker() : GenericTracker(),
		m_debugmode(false),
		m_marker_num(0),
		m_cparam(new CameraParameter)
	{
		// attach a new field to the name "debug"
		m_fields["debug"] = new TypedField<bool>(&m_debugmode);
		// for statistics
		m_fields["markercount"] = new TypedField<int>(&m_marker_num);
	}


	BazARTracker::~BazARTracker()
	{
		delete m_cparam;
	}


	bool BazARTracker::init(int xsize, int ysize, 
			const std::string& bazar_config_name,
			const std::string& camera_name)
	{
		
		/*OSGART*/
		ARParam  wparam;
	    // Set the initial camera parameters.
		cparamName = camera_name;
	    if(arParamLoad((char*)cparamName.c_str(), 1, &wparam) < 0) {
			std::cerr << "ERROR: Camera parameter load error." << std::endl;
			return false;
	    }
	    arParamChangeSize(&wparam, xsize, ysize,&(m_cparam->cparam));

		arInitCparam(&(m_cparam->cparam));
	    arParamDisp(&(m_cparam->cparam));
		

	/*BAZAR*/
		// load BazAR's configuration files and detector parameters
		if (!loadBazARConfig(bazar_config_name, &bazconf)) exit(0);
		// load BazAR's camera calibration file
		if (!loadBazARCamParams((char*)(bazconf.camCalFileName),&bazconf)) exit(0);

		// init bazar tracker
		matCameraRT4_4 = cvCreateMat(4, 4, CV_64F); //64 bit double precision float
		g_matIntrinsic   = cvCreateMat(3, 3, CV_64F);
		
		// output windows
		if (getDebugMode()){
			cvNamedWindow("Gray", CV_WINDOW_AUTOSIZE);
			cvNamedWindow("Result_BAZAR", CV_WINDOW_AUTOSIZE);
		}
		// fine tuning for accuracy - careful!!
		detector.ransac_dist_threshold = (float)bazconf.ransac_dist_threshold;
		detector.max_ransac_iterations = (float)bazconf.max_ransac_iterations;
		detector.non_linear_refine_threshold = (float)bazconf.non_linear_refine_threshold;
		detector.match_score_threshold = (float)bazconf.match_score_threshold;	// A lower threshold will allow detection in harder conditions, but
																				// might lead to false positives
		// Train or load classifier
		if(!detector.build_with_cache(
			// hse25: no hard coded stuff :) -- jaja
			(char*)(bazconf.modelFileName),	// mode image file name
			400,               // maximum number of keypoints on the model
			32,                // patch size in pixels
			3,                 // yape radius. Use 3,5 or 7.
			16,                // number of trees for the classifier. Somewhere between 12-50
			3                  // number of levels in the gaussian pyramid
			))
		{
		cerr << "BazARTracker: Unable to load the model image " << (char*)(bazconf.modelFileName) <<" or its classifier.\n";
		return false;
		}
		 
		// set camera parameters for BAZAR
		char	*camCal = (char*)(bazconf.camCalFileName);
		char	*camExt = (char*)(bazconf.camExtFileName);
		if(!augment.LoadOptimalStructureFromFile(camCal, camExt))
		{
			std::cerr << "BazARTracker: couldn't load camera parameters: " << camCal << " " << camExt << std::endl;
			return false;
		}
		
		// image buffers needed for detection, conversion..
		image     = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 4); // captured image
		gray      = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 1); // detector input
		display   = cvCreateImage(cvSize(xsize, ysize), IPL_DEPTH_8U, 4); // debug
		
		// use bazar's camera calibration 
		m_cparam->cparam.mat[0][0] = bazconf.camCalMatrix[0][0];	m_cparam->cparam.mat[0][1] = bazconf.camCalMatrix[0][1];    m_cparam->cparam.mat[0][2] = bazconf.camCalMatrix[0][2];
		m_cparam->cparam.mat[1][0] = bazconf.camCalMatrix[1][0];	m_cparam->cparam.mat[1][1] = bazconf.camCalMatrix[1][1];    m_cparam->cparam.mat[1][2] = bazconf.camCalMatrix[1][2];
		m_cparam->cparam.mat[2][0] = bazconf.camCalMatrix[2][0];	m_cparam->cparam.mat[2][1] = bazconf.camCalMatrix[2][1];    m_cparam->cparam.mat[2][2] = bazconf.camCalMatrix[2][2];
		
		for(int i=0; i<3; i++)
		{
			for(int j=0; j<3; j++)
			{
				cvmSet(g_matIntrinsic, i, j, m_cparam->cparam.mat[i][j]);
			}
		}
	// end BAZAR
	  
		setProjection(10.0f, 8000.0f);
		setDebugMode(m_debugmode);
		
		setupMarkers();

		// Success
		return true;
	}


	bool  BazARTracker::loadBazARCamParams(char* bazar_camcalib_name, BazarConfigParameter *bazconf){
		
		FILE *stream;

		if( (stream = fopen( bazar_camcalib_name, "r+t" )) != NULL ){
		int num,width,height;
		while( fscanf( stream, "no: %i\nwidth: %i\nheight: %i\n", &num, &width, &height ) != EOF ){
		float h;	
		for(int i=0; i < 3; i++)
			for(int j=0; j < 3; j++) {
			fscanf( stream, "%f", &h );
			bazconf->camCalMatrix[i][j] = h;
			}
			fscanf( stream, "\n" );
		}
		fclose( stream );
		} else {
			cerr << "BazARTracker: Could not load matrices from file " << bazar_camcalib_name << endl;
			return false;
		}
	return true;
	}

	bool BazARTracker::loadBazARConfig(const std::string& bazar_config_name, BazarConfigParameter  *bazconf)
	{
		// open configuration file
		ifstream file (bazar_config_name.c_str());
		
		// check for success
		if (!file.is_open()){
			std::cerr << "BazARTracker: Unable to open file" << bazar_config_name << std::endl;
			return false;
		}

		/* read location of configuration files */
		// read content
		string line;
		int pos = 0;
		bool done = false;

		// read location of camera_c.txt	
		while(!done){
			getline (file,line);
			if( !((line.find("//") == 0))){
				pos = line.find("txt",0);
				if( (pos == string.npos) || !strcpy(bazconf->camCalFileName, (line.substr( 0, pos+3).c_str()))){
					std::cerr<<"BazARTracker: Error reading " << bazar_config_name << std::endl;				
					file.close();
					return false;
				}
				else{
					done = true;
				}
			}
		}
		// read location of camera_r_t.txt	
		done = false;
		while(!done){
			getline (file,line);
			if(line.find("//") != 0){
				if(!strcpy(bazconf->camExtFileName, (line.substr( 0, line.find("txt",0)+3)).c_str())){
					std::cerr<<"BazARTracker: Error reading " << bazar_config_name << std::endl;				
					file.close();
					return false;
				}
				else{
					done = true;
				}
			}
		}

		// read location of model	
		done = false;
		while(!done){
			getline (file,line);
			if(line.find("//") != 0){
				if(!strcpy(bazconf->modelFileName, (line.substr( 0, line.find("jpg",0)+3)).c_str())){
					std::cerr<<"BazARTracker: Error reading " << bazar_config_name << std::endl;	
					file.close();
					return false;
				}
				else{
					done = true;
				}
			}
		}


		/* read optional detector tuning parameters */
		bool tun_params_ok = true;
		
		//read detector.ransac_dist_threshold
		done = false;
		float value = 0;
		while(!done && tun_params_ok){
			if(file.eof()) {tun_params_ok = false; break;}
			getline (file,line);
			if(line.find_first_of("//") != 0){
				if(value = atof(line.c_str()))
				{
					bazconf->ransac_dist_threshold = value;
					cout<<"BazARTracker: detector.ransac_dist_threshold = "<< right << bazconf->ransac_dist_threshold <<endl;
				}
				else
				{
					cout<<"BazARTracker: detector.ransac_dist_threshold = "<< bazconf->ransac_dist_threshold <<endl;
					cout<<"BazARTracker: ---> possible read error, sure this is 0?" << endl;
				}
				done = true;
			}
		}
		
		//read detector.max_ransac_iterations
		done = false;
		value = 0;
		while(!done && tun_params_ok){
			if(file.eof()) {tun_params_ok = false; break;}
			getline (file,line);	
			if(line.find_first_of("//") != 0){
				if(value = atof(line.c_str()))
				{
					bazconf->max_ransac_iterations = value;
					cout<<"BazARTracker: detector.max_ransac_iterations = "<< right << bazconf->max_ransac_iterations <<endl;
				}
				else
				{
					cout<<"BazARTracker: detector.max_ransac_iterations = "<< bazconf->max_ransac_iterations <<endl;
					cout<<"BazARTracker: ---> possible read error, sure this is 0?" << endl;
				}
				done = true;
			}	
		}

		//read detector.non_linear_refine_threshold	
		done = false;
		value = 0;
		while(!done && tun_params_ok){
			if(file.eof()) {tun_params_ok = false; break;}
			getline (file,line);
			if(line.find_first_of("//") != 0){
				if(value = atof(line.c_str()))
				{
					bazconf->non_linear_refine_threshold = value;
					cout<<"BazARTracker: detector.non_linear_refine_threshold = "<< bazconf->non_linear_refine_threshold <<endl;
				}
				else
				{
					cout<<"BazARTracker: detector.non_linear_refine_threshold = "<< bazconf->non_linear_refine_threshold <<endl;
					cout<<"BazARTracker: ---> possible read error, sure this is 0?" << endl;
				}
				done = true;
			}
		}
			
		//read detector.match_score_threshold
		done = false;
		value = 0;
		while(!done && tun_params_ok){
			if(file.eof()) {tun_params_ok = false; break;}
			getline (file,line);
			if(line.find_first_of("//") != 0){
				if(value = atof(line.c_str()))
				{
					bazconf->match_score_threshold = value;
					cout<<"BazARTracker: detector.match_score_threshold = "<< bazconf->match_score_threshold <<endl;
				}
				else
				{
					cout<<"BazARTracker: detector.match_score_threshold = "<< bazconf->match_score_threshold <<endl;
					cout<<"BazARTracker: ---> possible read error, sure this is 0?" << endl;
				}
				done = true;
			}
		}
		
		
		/* report errors */
		if (!tun_params_ok){
			std::cerr	<< "BazARTracker: detector parameters not specified or erroneous." << endl
						<< "BazARTracker: --> using default parameters" << endl;

			bazconf->ransac_dist_threshold =		RANSAC_DIST_THRESHOLD;
			bazconf->max_ransac_iterations =		MAX_RANSAC_ITERATIONS;
			bazconf->non_linear_refine_threshold =	NON_LINEAR_REFINE_THRESHOLD;
			bazconf->match_score_threshold =		MATCH_SCORE_THRESHOLD;

			std::cerr	<< "BazARTracker: detector.ransac_dist_threshold = "		<< bazconf->ransac_dist_threshold			<< endl
						<< "BazARTracker: detector.max_ransac_iterations = "		<< bazconf->max_ransac_iterations			<< endl
						<< "BazARTracker: detector.non_linear_refine_threshold = "	<< bazconf->non_linear_refine_threshold		<< endl
						<< "BazARTracker: detector.match_score_threshold = "		<< bazconf->match_score_threshold			<< endl;
		}

		file.close();	
		return true;
	}

	bool BazARTracker::setupMarkers()
	{
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

	ARParam* BazARTracker::getCParam()
	{
		return &(m_cparam->cparam);
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
		
		if(getDebugMode()) cvShowImage("Gray", gray);		// show what bazar will see
		

		// if the trained planar surface is detected
		if (detector.detect(gray))  
		{
			// start on a new frame
			augment.Clear();

			// we have only 1 camera
			add_detected_homography(detector, augment);

			// bundle adjust
			augment.Accomodate(4, 1e-4);
		
			show_result(augment, image, &display);	// GL-projection matrix, if debug: render additional output
			
			if (getDebugMode()) cvShowImage("Result_BAZAR", display);

			(static_cast<BazARMarker*>(m_markerlist[0].get()))->update(matCameraRT4_4);
		} 
		else 
		{
			if (getDebugMode()) cvShowImage("Result_BAZAR", display);
			
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
		
		// inverse matrix with intrinsic camera parameters
		CvMat *invK = cvCloneMat(Intrinsic);
		cvInv(Intrinsic, invK);
		// multiply bazProjMat with inverted intrinsic camera parameter matrix -> extract intrinsic parameters
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
		if (getDebugMode()){
			if (*dst==0) *dst=cvCloneImage(video);
			else cvCopy(video, *dst);
		}

		CvMat *m  = augment.GetProjectionMatrix(0);
		// Flip...(This occured from OpenGL origin / camera origin)
		CvMat *coordinateTrans = cvCreateMat(3, 3, CV_64F);
		cvmSetIdentity(coordinateTrans);
		cvmSet(coordinateTrans, 1, 1, -1);
		cvmSet(coordinateTrans, 1, 2, m_cparam->cparam.ysize);
		cvMatMul(coordinateTrans, m, m);
	
		// extract intrinsic camera parameters from bazar's projection matrix..
		GetARToolKitRTfromBAZARProjMat(g_matIntrinsic, m, matCameraRT4_4);
			
		cvTranspose(matCameraRT4_4, matCameraRT4_4);
		cvReleaseMat(&coordinateTrans);

		// Debug
		if (getDebugMode()) {
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
