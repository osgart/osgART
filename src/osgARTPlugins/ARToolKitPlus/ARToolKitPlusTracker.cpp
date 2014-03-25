/* -*-c++-*-
 *
 * osgART - AR for OpenSceneGraph
 * Copyright (C) 2005-2009 Human Interface Technology Laboratory New Zealand
 * Copyright (C) 2009-2013 osgART Development Team
 *
 * This file is part of osgART
 *
 * osgART 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * osgART 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with osgART 2.0.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <osg/Notify>

#include "osgART/Target"
#include "osgART/VisualTracker"
#include "osgART/Video"
#include "osgART/CameraConfiguration"
#include "osgART/Utils"
#include "osgART/PluginManager"

#include <osgDB/FileUtils>
#include <osgDB/ReadFile>
#include <osg/io_utils>

#include <ARToolkitplus/ar.h>
#include <ARToolkitplus/arMulti.h>
#include <ARToolKitPlus/TrackerSingleMarkerImpl.h>
#include <ARToolKitPlus/TrackerMultiMarkerImpl.h>


#define _ARTPLUS_MAX_MARKER 16				//!< Maximum number of marker the library will process per frame.
//#define _ART_PLUS_TRACKER_PARAM_SIMPLE		6,6,6,(ARToolKitPlus::PIXEL_FORMAT)1,		_ARTPLUS_MAX_MARKER		//!< Main template values for the Simple ID marker mode.
//#define _ART_PLUS_TRACKER_PARAM_TEMPLATE	16,16,64,(ARToolKitPlus::PIXEL_FORMAT)15,	_ARTPLUS_MAX_MARKER		//!< Main template values for the Template marker mode.
#define _ART_PLUS_TRACKER_PARAM_SIMPLE		6,6,6,1,		_ARTPLUS_MAX_MARKER		//!< Main template values for the Simple ID marker mode.
#define _ART_PLUS_TRACKER_PARAM_TEMPLATE	16,16,64,15,	_ARTPLUS_MAX_MARKER		//!< Main template values for the Template marker mode.
#define _ART_PLUS_VERSION					ARTOOLKITPLUS_VERSION_MAJOR "." ARTOOLKITPLUS_VERSION_MINOR //!< Set the current version of ARToolkit plus.

//Default values	====================
#define _ART_PLUS_DFLT_FARCLIP			8000.f	
#define _ART_PLUS_DFLT_NEARCLIP			10.f
#define _ART_PLUS_DFLT_MARKERMODE		ARToolKitPlus::MARKER_TEMPLATE
#define _ART_PLUS_DFLT_THRESHOLD		100
#define _ART_PLUS_DFLT_THRESHOLD_AUTO	true
#define _ART_PLUS_DFLT_THRESHOLD_AUTO_RETRY_NBR	2
#define _ART_PLUS_DFLT_USE_LITE			false
#define _ART_PLUS_DFLT_POSE_ESTIM		ARToolKitPlus::POSE_ESTIMATOR_RPP
#define	_ART_PLUS_DFLT_IMG_PROC_MODE	ARToolKitPlus::IMAGE_FULL_RES	
#define _ART_PLUS_DFLT_UNDISTORT_MODE	ARToolKitPlus::UNDIST_NONE
//**************************************

#if ((ARTOOLKITPLUS_VERSION_MAJOR != 2) && (ARTOOLKITPLUS_VERSION_MINOR != 1))
# error "Plugin only works with ARToolKitPlus 2.1.x"
#endif

// Make sure that required OpenGL constant definitions are available at compile-time.
// N.B. These should not be used unless the renderer indicates (at run-time) that it supports them.
// Define constants for extensions (not yet core).
#ifndef GL_APPLE_ycbcr_422
#  define GL_YCBCR_422_APPLE				0x85B9
#  define GL_UNSIGNED_SHORT_8_8_APPLE		0x85BA
#  define GL_UNSIGNED_SHORT_8_8_REV_APPLE	0x85BB
#endif
#ifndef GL_EXT_abgr
#  define GL_ABGR_EXT						0x8000
#endif
#ifndef GL_MESA_ycbcr_texture
#  define GL_YCBCR_MESA						0x8757
#  define GL_UNSIGNED_SHORT_8_8_MESA		0x85BA
#  define GL_UNSIGNED_SHORT_8_8_REV_MESA	0x85BB
#endif



class DummyCameraConfiguration : public osgART::CameraConfiguration
{

public:

	inline DummyCameraConfiguration()
	{
	}

	inline ~DummyCameraConfiguration()
	{
	}

	inline const DummyCameraConfiguration* getCameraConfiguration() const
	{
		return this;
	}

	inline bool load(const std::string& filename)
	{
		//specific load of camera parameters that can be used with this
		//tracker

		//in this example we do nothing here

		osg::notify() << "Dummy Camera Configuration load " <<std::endl;

		osg::Matrix dummy_projection_matrix_test;
		//dummy_projection_matrix_test.makePerspective(40.,1.3,1.,10000.);

		dummy_projection_matrix_test.set(
			2, 0, 0, 0,
			0, 2.66667, 0, 0,
			0, 0, -1.0002, -1,
			0, 0, -0.20002, 0);

		_projection.set(dummy_projection_matrix_test);

		//you can also setup the distorsion parameters
		//_distortion.set(val1,val2,val3,val4);

	
		return true;
	}

	inline void setSize(int width, int height)
	{
		osg::notify() << "Dummy Camera Configuration resize '" <<
			width << " x " << height << std::endl;

		//you can adjust the image resolution keeping similar projection matrix
		//you need to modify your camera projection matrix


	}

};

class ARToolKitPlusTracker;


/**
* \class SingleTarget
* \brief A target consisting of a single pattern.
*/
class SingleTarget : public osgART::Target
{
public:       

	/** 
		* \brief default constructor.
		*
		*/
	SingleTarget(ARToolKitPlus::TrackerMultiMarker * _PlusTracker);
	
	/** 
		* \brief Init the pattern.
		* \param pattFile => path of the file that contain the pattern to load. If pattFile contains only a number, then we have a ID pattern.
		* \param width => width of the pattern.
		* \param center => relative position of the center of the pattern.
		*/	
	bool initialise(const std::string& pattFile, double width, double center[2]);

	/**
		* \brief Activate/Desactivate the target.
		* \param status.
		* \note Activation is not yet reachable in ARToolkitPlus.
		* \todo Use ARToolkit plus activation.
		*/
	virtual void setActive(bool a);

	/**
		* \brief Update the pattern position.
		* \param target info corresponding to the target.
		*/
	void update(ARToolKitPlus::ARMarkerInfo* markerInfo);
	
	/**
		* \brief Get Pattern ID.
		*/
	int		getPatternID();
	
	/**
		* \brief Get Pattern width.
		*/
	double	getPatternWidth();
	
	/**
		* \brief Get Pattern center.
		*/
	double* getPatternCenter();
	
	/**
		* \brief Get Pattern type.
		*/
	osgART::Target::TargetType getType() const;

	
protected:

	/**
	* \brief destructor.
	*/
	virtual ~SingleTarget();	    

	ARToolKitPlus::TrackerMultiMarker *	m_ParentTracker;		//!< Artoolkit tracker this marker is linked to
	
	
	int             m_patt_id;				//!< Pattern ID.
	double          m_patt_width;			//!< Pattern width.
	double          m_patt_border_width;	//!< Not Used...
	double          m_patt_center[2];		//!< Pattern center.
	double          m_patt_trans[3][4];		//!< Marker transformation matrix.
	double			m_confidence;			//..Still used???
	
	friend class ARToolKitPlusTracker;	
};


//==============================================================================
//==============================================================================
//==============================================================================
//				MultiTarget definition			
//==============================================================================
//==============================================================================
//==============================================================================
/**
* \class MultiTarget
* \brief A target consisting of many patterns which can be treated as one.
*/
class MultiTarget : public osgART::Target
{
public:       

	/** 
	* \brief default constructor.
	*
	*/
	MultiTarget(ARToolKitPlus::TrackerMultiMarker* _PlusTracker, ARToolKitPlus::MARKER_MODE markerMode	);
	
	bool initialise(const std::string& multiFile);
	
	virtual void setActive(bool a);
	
	void update(ARToolKitPlus::ARMarkerInfo* markerInfo, int markerCount);
	
	/**
	* \brief return the type of the target.
	* return if the target is a single or multi-pattern target.
	* \return the type of the target
	*/
	osgART::Target::TargetType getType() const;
	
protected: 

	/**
	* \brief destructor.
	*/
	virtual ~MultiTarget();
	
	//we need both tracker, cause At compile time we don't know which one wil be used
	ARToolKitPlus::TrackerMultiMarker										* m_ParentTracker;		//!< ArtoolkitPlus tracker this marker is linked to
	ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_TEMPLATE> * m_ParentTrackerTemplate;	//!<ArtoolkitPlus template tracker this marker is linked to
	ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_SIMPLE>	* m_ParentTrackerSimple;	//!<ArtoolkitPlus ID tracker this marker is linked to
	
	
	ARToolKitPlus::ARMultiMarkerInfoT* m_multi;
	friend class ARToolKitPlusTracker;	
		
};

/**
	* \class ARToolKitPlusTracker
	* \brief A tracker based on ARToolKit Plus 2.1.1. 
	* 
	* This tracker supports single and multi-markers.
	* \todo Add Undistortion map loading.
	//* \todo Add setUndistortionMode mode.
	* \todo Add Switch between arDetectMarker/arDetectMarkerLite.
	//* \todo Add setPoseEstimator(RPP/ORIGINAL) switch.
	* \todo Add setNumAutoThresholdRetries
	*/
class ARToolKitPlusTracker : public osgART::VisualTracker
{

public:

	ARToolKitPlusTracker();

	ARToolKitPlusTracker(const ARToolKitPlusTracker &, const osg::CopyOp& copyop);

	virtual ~ARToolKitPlusTracker();

	//we overload this function as we define our own camera configuration
	osgART::CameraConfiguration* getOrCreateCameraConfiguration();

	virtual bool init();

	virtual void setImage(osg::Image* image,bool useInternalVideo=false);

	osgART::Target* addTarget(const std::string& config);

	bool update(osg::NodeVisitor* nv = 0L);

	int ConvertOSGARTPixelFormatToART(osgART::PixelFormatType format) const;
	osgART::PixelFormatType ConvertARTPixelFormatToOSGART(int format)const;

	void	PrintOptions()const;

	
//===================================
//	Accessing Functions Get/Set......
//===================================
	/** 
	* \brief set the openGL parameters for the projection matrix.
	* 
	* Initialize internal parameters computation for delivers an openGL Matrix (modify
	* nothing to openGL state machine). 
	* \param n near field of view
	* \param f far field of view
	*/
	void setProjection(const double n, const double f);
	
	/** 
	* \brief Set the threshold value for the tracking algorithm.
	* 
	* Define the threshold used to binarize the image in ARToolkit.
	* \param thresh integer value between 0 (dark) and 255 (bright) 
	*/
	void setThreshold(const int &thresh);
	
	/**
	* \brief Get the current threshold value for the tracking algorithm.
	*
	* \return The current threshold.
	*/
	int getThreshold()const ;		


	/**
	* \brief Set the target processing mode of ARToolkitPlus tracker. 
	*
	* \param target mode value(ARToolKitPlus::MARKER_MODE = {MARKER_TEMPLATE,MARKER_ID_SIMPLE,MARKER_ID_BCH}).
	* \todo The target mode mst be set before the init() is called, add a control flag for that!!! or reset the tracker and init a new one
	* \sa m_markerMode
	*/	
	void	 setTargetMode(const int &_mode);
	
	/**
	* \brief Get the target processing mode of ARToolkitPlus tracker. 
	* \return current target mode(ARToolKitPlus::MARKER_MODE).
	* \sa m_markerMode
	*/
	int		 getTargetMode()const ;

	/**
	* \brief Set the tracker debugging mode. Only affect the plugin and not the ARToolkitPlus library. 
	* \param debugging value.
	*/
	void	setDebugMode(const bool &d);
	
	/**
	* \brief Get the tracker debugging mode. Only affect the plugin and not the ARToolkitPlus library. 
	* \return debugging value.
	*/
	bool	getDebugMode()const ;	
	
	/**
	* \brief Set the auto threshold value. 
	* \param auto-threshold.
	*/
	void	setAutoThreshold(const bool &_val);
	
	/**
	* \brief Get the auto threshold value. 
	* \return auto-threshold.
	*/
	bool	getAutoThreshold()const;
	
	void	setImgProcMode(const int & _imgMode);
	int		getImgProcMode()const;
	
	void	setPosEstimMode(const int & _imgMode);
	int		getPosEstimMode()const;
	
	void	setUndistortionMode	(const int & _mode);
	int		getUndistortionMode	()const;
	
	void	setAutoThresholdRetries	(const int & _nbr);
	int 	getAutoThresholdRetries	()const;
	
	void	setUseDetectLite(const bool& _mode);
	bool 	getUseDetectLite()const;

protected:

		/** 
	* \brief Register a single target with the tracker.
	* 
	* \param pattFile the pattern file name
	* \param width the width of the physical target
	* \param center the x-y coordinates of the center of the target (usually 0,0)
	* \return the internal ID of the new target, or -1 on failure 
	*/
	int addSingleTarget(const std::string& pattFile, 
		double width, double center[2]);
	
	/** 
	* \brief Register a multi-target with the tracker
	* 
	* \param multiFile the multi-target configuration file name
	* \return the internal ID of the new target, or -1 on failure
	*/
	int addMultiTarget(const std::string& multiFile);

	void	setPixelFormat		(ARToolKitPlus::PIXEL_FORMAT _format);
	void	setBorderWidth		(float _border);
	void	setPatternWidth		(float _width);

	bool	CreateTracker(ARToolKitPlus::MARKER_MODE _markerMode, int _Width, 	int _Height);
	bool	setupTargets(const std::string& patternListFile);
	bool	LoadCameraFile(std::string _file, float _nearClip, float _farClip);

	ARToolKitPlus::TrackerMultiMarker*	m_PlusTracker;			//!< ARToolkit plus tracker object.

	float				m_NearClip;								//!< Near clipping used for the projeciton matrix.
	float				m_FarClip;								//!< Far clipping used for the projeciton matrix.
	ARToolKitPlus::MARKER_MODE m_markerMode;					//!< Flag to define the current running mode, you should not change the mode after the function init() is called!!
	std::string 		m_cparamName;							//!< Camera parameters file name.
	int					m_width;								//!< Image width.
	int					m_height;								//!< Image height.
	int					m_pattern_width;						//!< Pattern width.
	int					m_threshold;							//!< Threshold value.
	bool 				m_debugMode;							//!< Debug mode flag.
	int					m_marker_num;							//!< Number of potential markers found in the previous image.

	int getARPixelFormatForImage(const osg::Image& _image) const;
	int getGLPixelFormatForARPixelFormat(const int arPixelFormat, GLenum *internalformat_GL, GLenum *format_GL, GLenum *type_GL) const;


	//ARToolKit plus features
	ARToolKitPlus::IMAGE_PROC_MODE		m_imageProcMode;		//!< Image processing mode: {IMAGE_HALF_RES,IMAGE_FULL_RES}.
	int									m_AutoThreshold;		//!< Automatic threshold flag.
	int									m_AutoThresholdRetriesNbr;		//!< Number of try with different threshold value, when no marker is found. Used only when m_AutoThreshold is set.
	ARToolKitPlus::POSE_ESTIMATOR		m_poseEstimMode;		//!< Pose estimation mode : {POSE_ESTIMATOR_ORIGINAL,POSE_ESTIMATOR_RPP}.
	ARToolKitPlus::UNDIST_MODE			m_undistortMode;		//!< Undistortion mode : {UNDIST_NONE, UNDIST_STD, UNDIST_LUT}.
	bool								m_useDetectLite;		//!< Flag to specify if we use the lite version of arDetectMarker.
	//=======================

};

/* Implementation */

//==============================================================================
//==============================================================================
//==============================================================================
//				SingleMarker definition			
//==============================================================================
//==============================================================================
//==============================================================================
	SingleTarget::SingleTarget(ARToolKitPlus::TrackerMultiMarker * _PlusTracker) 
		:	Target(),
			m_ParentTracker(_PlusTracker),
			m_patt_border_width(0.25f),
			m_patt_width(80.f),
			m_patt_id(-1)
	{
		if (!m_ParentTracker)
		{
			osg::notify(osg::FATAL) << "Could not create ARToolkitplus SingleMarker, no ARToolkitplus tracker." << std::endl;
			exit(-1);
		}
		_fields["confidence"] = new osgART::TypedField<double>(&m_confidence);
		_fields["patt_id"] = new osgART::TypedField<int>(&m_patt_id);
		//_fields["patt_code"] = new TypedField<int>(&m_patt_artag_code);
	}

	SingleTarget::~SingleTarget() {
		// jcl64: Free the pattern
		/*if (m_patt_id > 0) 
			m_ParentTracker->arFreePatt(m_patt_id);
		*///m_ParentTracker sould be a osg_ref...???
	}

	osgART::Target::TargetType SingleTarget::getType() const {
		return osgART::Target::TARGET_SINGLE;
	}

	bool SingleTarget::initialise(const std::string& pattFile, double width, double center[2])
	{
		sscanf(pattFile.c_str(), "%d", &m_patt_id);
		if (m_patt_id==-1)
		//template mode
			m_patt_id = m_ParentTracker->arLoadPatt((char *)pattFile.c_str());
			
		//else
		//id mode, nothing to do
		
		if (m_patt_id < 0) return false;
		m_patt_width = width;
		m_patt_center[0] = center[0];
		m_patt_center[1] = center[1];
		setName(pattFile);
		setActive(true);
		return true;
	}

	void SingleTarget::update(ARToolKitPlus::ARMarkerInfo* markerInfo) {		
		if (markerInfo == NULL) {
			_valid = false;
		} else {
			ARFloat patt_center_fl[2]		= {m_patt_center[0],m_patt_center[1]};
			ARFloat patt_trans_fl[3][4];
			m_ParentTracker->arGetTransMat(markerInfo, patt_center_fl, m_patt_width, patt_trans_fl);
			m_confidence = markerInfo->cf;
			m_ParentTracker->executeSingleMarkerPoseEstimator(markerInfo, patt_center_fl, m_patt_width, patt_trans_fl);	
			_valid = true;

			double modelView[16];
			
			for (int i=0;i<4; i++)
			{
				modelView[i*4]   = patt_trans_fl[0][i];
				modelView[i*4+1] = patt_trans_fl[1][i];
				modelView[i*4+2] = patt_trans_fl[2][i];
				modelView[i*4+3] = 0;
			}
			modelView[15] = 1;
			
			osg::Matrix tmp(modelView);

			//convert from Left Handed view to Right Handed
			tmp = tmp * osg::Matrixd::scale(-1,-1,1);
			tmp = tmp * osg::Matrixd::rotate(osg::inDegrees(180.), 0.,1.,0.);
			
			updateTransform(tmp);
		}
		
	}

	void SingleTarget::setActive(bool a) {
		_active = a;
		//no function to activate...
	}

	int SingleTarget::getPatternID() {
		return m_patt_id;
	}

	double SingleTarget::getPatternWidth() {
		return m_patt_width;
	}
		
	double* SingleTarget::getPatternCenter() {
		return m_patt_center;
	}
	

//==============================================================================
//==============================================================================
//==============================================================================
//				MultiMarker definition			
//==============================================================================
//==============================================================================
//==============================================================================	
	
	MultiTarget::MultiTarget(ARToolKitPlus::TrackerMultiMarker * _PlusTracker, ARToolKitPlus::MARKER_MODE markerMode	)
		: Target(),
		m_ParentTracker(_PlusTracker),
		m_ParentTrackerTemplate(NULL),
		m_ParentTrackerSimple(NULL),
		m_multi(NULL)
	{
		if (!m_ParentTracker)
		{
			osg::notify(osg::FATAL) << "Could not create ARToolkitplus MultiMarker, no ARToolkitplus tracker." << std::endl;
			exit(-1);
		}

		switch (markerMode)
		{	
			case ARToolKitPlus::MARKER_ID_SIMPLE :
				m_ParentTrackerSimple = dynamic_cast< ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_SIMPLE> * >(_PlusTracker);
				break;
			case ARToolKitPlus::MARKER_TEMPLATE :
				m_ParentTrackerTemplate = dynamic_cast< ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_TEMPLATE> * >(_PlusTracker);
				break;
			default:
				osg::notify(osg::WARN) << "Unkown marker mode in MultiMarker" << std::endl;
		}
		
//		_fields["confidence"] = new TypedField<double>(&m_confidence);
	}

	MultiTarget::~MultiTarget() {   
		// jcl64: Free the multimarker
		if (m_multi)
		{
	/*		if (ParentTrackerSimple)
				ParentTrackerSimple->arMultiFreeConfig(m_multi);
			else if (ParentTrackerTemplate)
				ParentTrackerTemplate->arMultiFreeConfig(m_multi);
	*///crash always..??	
		}
	}

	/* virtual */
	osgART::Target::TargetType MultiTarget::getType() const {
		return osgART::Target::TARGET_SINGLE;
	}

	bool 
	MultiTarget::initialise(const std::string& multiFile) {
		
		// Check if multifile exists		
		if (m_ParentTrackerSimple)
			m_multi =  m_ParentTrackerSimple->arMultiReadConfigFile(multiFile.c_str());
		else if (m_ParentTrackerTemplate)
			m_multi =  m_ParentTrackerTemplate->arMultiReadConfigFile(multiFile.c_str());
		else
			osg::notify(osg::WARN) << "No Tracker in MultiMarker, could not initialise." << std::endl;
		
		if (m_multi == NULL) return false;
		
		setName(multiFile);
		setActive(false);
		
		return true;
	}

	void
	MultiTarget::setActive(bool a) {
		_active = a;	
		//no function to activate...
	}

	//get all the marker detected and look if one of them are in the multimarker group.??
	void 
	MultiTarget::update(ARToolKitPlus::ARMarkerInfo* markerInfo, int markerCount) 
	{
		if (markerInfo == NULL) {
			_valid = false;
		} else {

			_valid = (m_ParentTracker->arMultiGetTransMat(markerInfo, markerCount, m_multi) >= 0);
			
			double modelView[16];

		//m_multi[]...
		//arglCameraViewRH(m_multi->trans, modelView, 1.0); // scale = 1.0.
			//ParentTracker->executeMultiMarkerPoseEstimator(markerInfo, markerCount, m_multi, patt_trans_fl);
			
			for (int i=0;i<4; i++)
			{
				modelView[i*4]   = m_multi->trans[0][i];
				modelView[i*4+1] = m_multi->trans[1][i];
				modelView[i*4+2] = m_multi->trans[2][i];
				modelView[i*4+3] = 0;
			}
			modelView[15] = 1;
						
			osg::Matrix tmp(modelView);
			//convert from Left Handed view to Right Handed
			tmp = tmp * osg::Matrixd::scale(-1,-1,1);
			tmp = tmp * osg::Matrixd::rotate(osg::inDegrees(180.), 0.,1.,0.);
			updateTransform(tmp);
		}
	}

ARToolKitPlusTracker::ARToolKitPlusTracker() :
	osgART::VisualTracker(),
		m_threshold		(_ART_PLUS_DFLT_THRESHOLD),
		m_debugMode		(false),
		m_marker_num	(0),
		m_FarClip		(_ART_PLUS_DFLT_FARCLIP),
		m_NearClip		(_ART_PLUS_DFLT_NEARCLIP),
		m_markerMode	(_ART_PLUS_DFLT_MARKERMODE),
		m_imageProcMode	(_ART_PLUS_DFLT_IMG_PROC_MODE),
		m_poseEstimMode	(_ART_PLUS_DFLT_POSE_ESTIM),
		m_undistortMode	(_ART_PLUS_DFLT_UNDISTORT_MODE),
		m_AutoThresholdRetriesNbr(_ART_PLUS_DFLT_THRESHOLD_AUTO_RETRY_NBR),
		m_AutoThreshold	(_ART_PLUS_DFLT_THRESHOLD_AUTO),
		m_useDetectLite	(_ART_PLUS_DFLT_USE_LITE),
		m_PlusTracker	(NULL)
{
	
	//version and name of the tracker
	_name		= "ARToolkitPlus";
	_version	= "2.1";
	//		__AR_DO_PROFILE(m_version+="(Prf)");

	//attach the field to the corresponding values
	_fields["nearclip"]	= new osgART::TypedField<float>(&m_NearClip);
	_fields["farclip"]		= new osgART::TypedField<float>(&m_FarClip);
	_fields["markercount"] = new osgART::TypedField<int>(&m_marker_num);

	//attach the field to the corresponding callbacks
	_fields["threshold"] = new osgART::CallbackField<ARToolKitPlusTracker,int>(this,
		&ARToolKitPlusTracker::getThreshold,
		&ARToolKitPlusTracker::setThreshold);

	_fields["autothreshold"] = new osgART::CallbackField<ARToolKitPlusTracker,bool>(this,
		&ARToolKitPlusTracker::getAutoThreshold,
		&ARToolKitPlusTracker::setAutoThreshold);

	_fields["autothreshold_retries"] = new osgART::CallbackField<ARToolKitPlusTracker,int>(this,
		&ARToolKitPlusTracker::getAutoThresholdRetries,
		&ARToolKitPlusTracker::setAutoThresholdRetries);

	_fields["target_mode"] = new osgART::CallbackField<ARToolKitPlusTracker,int>(this,
		&ARToolKitPlusTracker::getTargetMode,
		&ARToolKitPlusTracker::setTargetMode);

	_fields["img_proc_mode"] = new osgART::CallbackField<ARToolKitPlusTracker,int>(this,
		&ARToolKitPlusTracker::getImgProcMode,
		&ARToolKitPlusTracker::setImgProcMode);

	_fields["pos_estim_mode"] = new osgART::CallbackField<ARToolKitPlusTracker,int>(this,
		&ARToolKitPlusTracker::getPosEstimMode,
		&ARToolKitPlusTracker::setPosEstimMode);

	_fields["undistort_mode"] = new osgART::CallbackField<ARToolKitPlusTracker,int>(this,
		&ARToolKitPlusTracker::getUndistortionMode,
		&ARToolKitPlusTracker::setUndistortionMode);

	_fields["use_detect_lite"] = new osgART::CallbackField<ARToolKitPlusTracker,bool>(this,
		&ARToolKitPlusTracker::getUseDetectLite,
		&ARToolKitPlusTracker::setUseDetectLite);

	_fields["debug"]		= new osgART::CallbackField<ARToolKitPlusTracker,bool>(this,
		&ARToolKitPlusTracker::getDebugMode,
		&ARToolKitPlusTracker::setDebugMode);


	//set the internal format of ARToolkit Plus, defined at compile time
	//see setImageRaw() for conversion
//#if YCK_GENERIC_TRACKER
//	m_arInternalFormat	= ConvertARTPixelFormatToOSGART( ARToolKitPlus::PIXEL_FORMAT_BGR);
//#endif

}

ARToolKitPlusTracker::ARToolKitPlusTracker(const ARToolKitPlusTracker &, const osg::CopyOp& copyop):	osgART::VisualTracker()
{
	if (m_PlusTracker)
		delete m_PlusTracker;
}


ARToolKitPlusTracker::~ARToolKitPlusTracker()
{
	//clean up the tracker

}

//this call allow to access intrinsics parameters
//for optical tracker
//it's used also for 
inline osgART::CameraConfiguration*
ARToolKitPlusTracker::getOrCreateCameraConfiguration()
{
	//you have two choices here

	//1. you create a specific Camera Configuration class, that can hold
	//more parameters for your tracker
	if (!_cameraConfiguration.valid()) _cameraConfiguration = new DummyCameraConfiguration;

	//2.you only create a default Camera Configuration
	//and you setup the proj matrix
	//_cameraconfiguration=new osgART::CameraConfiguration();
	//_cameraconfiguration._prjection..etc 
	return osgART::Tracker::getOrCreateCameraConfiguration();
}

inline void
ARToolKitPlusTracker::setImage(osg::Image* image,bool useInternalVideo)
{

	if (!image) {
		osg::notify() << "osgART::ARToolKitPlusTracker::setImage() called with NULL image" << std::endl;
		return;
	}

	osgART::VisualTracker::setImage(image);

	this->getOrCreateCameraConfiguration()->setSize(*image);

	//you can initialize debug image here

	// Initialise debug image to match video image		
	//m_debugimage->allocateImage(image->s(), image->t(), 1, image->getPixelFormat(), image->getDataType());


}

inline osgART::Target*
ARToolKitPlusTracker::addTarget(const std::string& config)
{
	//add it to the list
//	_targetList.push_back(target);
	
	return 0L;
}

bool ARToolKitPlusTracker::CreateTracker(
			ARToolKitPlus::MARKER_MODE _markerMode,
			int _Width, int _Height)
	{
		//create the tracker depending on the mode
		switch(_markerMode)
		{
			case ARToolKitPlus::MARKER_ID_SIMPLE:
				m_PlusTracker = (ARToolKitPlus::TrackerMultiMarker *) (new ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_SIMPLE>());
				break;
			case ARToolKitPlus::MARKER_TEMPLATE:
				m_PlusTracker = (ARToolKitPlus::TrackerMultiMarker *) (new ARToolKitPlus::TrackerMultiMarkerImpl<_ART_PLUS_TRACKER_PARAM_TEMPLATE>());//
				break;
			case ARToolKitPlus::MARKER_ID_BCH:
				osg::notify(osg::FATAL) << "ARToolKitPlus::CreateTracker() :  target mode 'MARKER_ID_BCH' not implement" << std::endl;	
				exit(-1);
				break;
			default:
				osg::notify(osg::FATAL) << "ARToolKitPlus::CreateTracker() :  unknown target mode '"<< _markerMode << "'" << std::endl;	
				exit(-1);
		}

		if (!m_PlusTracker)
			return false;

		//size of the marker is done in init function...
		
		printf("ARToolKitPlus compile-time information:\n%s\n\n",	m_PlusTracker->getDescription());	
		return true;
	}

	bool ARToolKitPlusTracker::init()
	{
		//
		int _xsize;
		int _ysize;
		const std::string _pattlist_name;
		const std::string _camera_name;


		m_width = _xsize;
		m_height = _xsize; 
		if (!CreateTracker	(m_markerMode, _xsize,_ysize))
		{
			osg::notify(osg::FATAL) << "ARToolKitPlus::init() :  could not create Tracker!" << std::endl;	
			exit(-1);
		}


		//init them
			setTargetMode(m_markerMode);
// #if YCK_GENERIC_TRACKER
			setPixelFormat(ARToolKitPlus::PIXEL_FORMAT_BGR);
			
			// ConvertOSGARTPixelFormatToART(m_arInternalFormat));
// #endif			
			
			//m_PlusTracker->setLoadUndistLUT(true);
			
			switch(m_markerMode)
			{
				case ARToolKitPlus::MARKER_TEMPLATE: setBorderWidth(0.25f); break;
				case ARToolKitPlus::MARKER_ID_SIMPLE:
				default: setBorderWidth(0.125f);
			}
			setPatternWidth(80.);
		

			//just because the tracker need to be refreshed
			setDebugMode(getDebugMode());	
			setThreshold(getThreshold());
			setImgProcMode(getImgProcMode());
			setAutoThreshold(getAutoThreshold());
			setAutoThresholdRetries(getAutoThresholdRetries());
			setPosEstimMode(getPosEstimMode());
			setUseDetectLite(getUseDetectLite());
			setUndistortionMode(getUndistortionMode());
			//==============================================

			//m_PlusTracker->activateIdBasedMarkers(true);	
			
			
		//==========================

		// Set the initial camera parameters.
			m_cparamName = _camera_name;
			m_width = _xsize;
			m_height= _ysize;

			osg::notify() << "Loading camera param file : " << m_cparamName << std::endl;
			
			if (!LoadCameraFile((char*)m_cparamName.c_str(), m_NearClip, m_FarClip))
			{
				osg::notify(osg::FATAL) << "Can not load camera file : "<< m_cparamName << std::endl;	
				exit(-1);
			}

			m_PlusTracker->changeCameraSize(_xsize,_ysize);
			setProjection(m_NearClip, m_FarClip);
		
			if (!setupTargets(_pattlist_name))
			{
				osg::notify(osg::FATAL) << "Could not load the pattern list file :" << _pattlist_name << std::endl;
			}
		
		//tracker->init("data/LogitechPro4000.dat", 1.0f, 1000.0f);
		return true;
	}

//===================================
//	Accessing Functions Get/Set......
//===================================


/*protected*/
	bool	ARToolKitPlusTracker::LoadCameraFile(std::string _file, float _nearClip, float _farClip)
	{
		if (m_PlusTracker)
		{
			if (!m_PlusTracker->loadCameraFile((char*)m_cparamName.c_str(), _nearClip, _farClip))
				return false;
		}		
		return true;	
	}
	 
	void	ARToolKitPlusTracker::setPixelFormat(ARToolKitPlus::PIXEL_FORMAT _format)
	{
		if (m_PlusTracker)
			m_PlusTracker->setPixelFormat(_format);
	}
	 
	void	ARToolKitPlusTracker::setBorderWidth(float _border)
	{
		if (m_PlusTracker)
			m_PlusTracker->setBorderWidth(_border);
	}
	  	//??, in config file??

	void	ARToolKitPlusTracker::setPatternWidth(float _width)
	{//???
		m_pattern_width = _width;
		/*if (m_PlusTracker)
			m_PlusTracker->set setPatternWidth(_width);
			*/
	}

	void	ARToolKitPlusTracker::setUseDetectLite(const bool& _mode)
	{
		m_useDetectLite = _mode;
		if (m_PlusTracker)
			m_PlusTracker->setUseDetectLite(m_useDetectLite);
	}
	bool 	ARToolKitPlusTracker::getUseDetectLite()const
	{	return m_useDetectLite;	}
	 
	 
	//ARToolKitPlus::MARKER_MODE 
	void ARToolKitPlusTracker::setTargetMode(const int &_mode)
	{
		m_markerMode = (ARToolKitPlus::MARKER_MODE )_mode;
		if (m_PlusTracker)
			m_PlusTracker->setMarkerMode(m_markerMode);
	}
/*public*/
	 
	//ARToolKitPlus::MARKER_MODE 
	int ARToolKitPlusTracker::getTargetMode() const
	{
		return m_markerMode;	
	}	

	//Auto Threshold
	void	ARToolKitPlusTracker::setAutoThreshold(const bool &_val)
	{
		if (m_PlusTracker)
			m_PlusTracker->activateAutoThreshold(_val);
	}

	bool	ARToolKitPlusTracker::getAutoThreshold()const
	{
		return m_PlusTracker->isAutoThresholdActivated();
	}

	//Auto threshold retries
	void	ARToolKitPlusTracker::setAutoThresholdRetries	(const int & _nbr)
	{
		m_AutoThresholdRetriesNbr = _nbr;
		if (m_PlusTracker)
			m_PlusTracker->setNumAutoThresholdRetries(_nbr);	
	}

	int 	ARToolKitPlusTracker::getAutoThresholdRetries	()const
	{	return m_AutoThresholdRetriesNbr;	}

	//Threshold
	void ARToolKitPlusTracker::setThreshold(const int &thresh)	{
		// jcl64: Clamp to 0-255, hse25: use osg func
		m_threshold = osg::clampBetween(thresh,0,255);
		if (m_PlusTracker)
			m_PlusTracker->setThreshold(m_threshold);	
	}

	int ARToolKitPlusTracker::getThreshold()const
	{ 	return m_threshold;	}

	//undistortion mode
	void	ARToolKitPlusTracker::setUndistortionMode	(const int & _mode)
	{
		m_undistortMode = (ARToolKitPlus::UNDIST_MODE) _mode;
		if (m_PlusTracker)
			m_PlusTracker->setUndistortionMode(m_undistortMode);	
	}
	
	int ARToolKitPlusTracker::getUndistortionMode	()const
	{	return m_undistortMode;	}
	

	//debug mode
	void ARToolKitPlusTracker::setDebugMode(const bool &d) 
	{ 	m_debugMode = d;}

	bool ARToolKitPlusTracker::getDebugMode()const  
	{ 	return m_debugMode;}
	 
	 
	void ARToolKitPlusTracker::setProjection(const double n, const double f)
	{
		m_NearClip	= n;
		m_FarClip	= f;
		bool res = false;
		ARFloat projectionMatrix_fl[16];//used for the convertion

		switch(m_markerMode)
		{
			case ARToolKitPlus::MARKER_ID_SIMPLE:
				res = ARToolKitPlus::TrackerImpl<_ART_PLUS_TRACKER_PARAM_SIMPLE >::calcCameraMatrix(m_cparamName.c_str(), m_width, m_height, n, f, projectionMatrix_fl);
				break;
			case ARToolKitPlus::MARKER_TEMPLATE:
				res = ARToolKitPlus::TrackerImpl<_ART_PLUS_TRACKER_PARAM_TEMPLATE >::calcCameraMatrix(m_cparamName.c_str(), m_width, m_height, n, f, projectionMatrix_fl);
				break;
				case ARToolKitPlus::MARKER_ID_BCH:
				osg::notify(osg::FATAL) << "ARToolKitPlus::setProjection() :  target mode 'MARKER_ID_BCH' not implementt" << std::endl;	
				exit(-1);
				break;
			default:
				osg::notify(osg::FATAL) << "ARToolKitPlus::setProjection() :  unknown target mode '"<< m_markerMode << "'" << std::endl;	
				exit(-1);
		}

		if(!res)
		{
			osg::notify(osg::FATAL) << "Could not get camera calibration : " << m_cparamName << std::endl;
			exit(-1);
		}

		const ARFloat *projectionMatrix_fl2 = m_PlusTracker->getProjectionMatrix();
	
		//convert from float to Double
		for (int i=0;i<4; i++)
			for (int j=0;i<4; i++)
			_projectionMatrix(i,j)= projectionMatrix_fl[i];

		//if (m_debugMode)
		//	 PrintMatrix("SetProjectionMatrix() before conversion", osg::Matrix(_projectionMatrix));
		//

		//convert matrix from left hand to right hand
		//see arglCameraFrustumRH for details
		//_projectionMatrix(0 + 2 *4] *= -1; //	q[0][2] = ((2.0 * p[0][2] / (width - 1))  - 1.0);
		//_projectionMatrix(1 + 1 *4] *= -1; //	q[1][1] = (2.0 * p[1][1] / (height - 1));
		//_projectionMatrix(1 + 2 *4] *= -1; //	q[1][2] = ((2.0 * p[1][2] / (height - 1)) - 1.0);
		//_projectionMatrix(2 + 2 *4] *= -1; //	q[2][3] = -2.0 * focalmax * focalmin / (focalmax - focalmin);
		//_projectionMatrix(3 + 2 *4] *= -1; //	q[3][2] = 1.0;

		//if (m_debugMode)
		//	 PrintMatrix("SetProjectionMatrix() aftet right hand conversion", osg::Matrix(_projectionMatrix));
	}


	//image processing mode
	void	ARToolKitPlusTracker::setImgProcMode(const int & _imgMode)
	{
		m_imageProcMode = (ARToolKitPlus::IMAGE_PROC_MODE) _imgMode;
		if (m_PlusTracker)
			m_PlusTracker->setImageProcessingMode(m_imageProcMode);
	}

	int ARToolKitPlusTracker::getImgProcMode()const
	{	return m_imageProcMode;	}


	//pose estimation mode
	void	ARToolKitPlusTracker::setPosEstimMode(const int & _Mode)
	{
		m_poseEstimMode = (ARToolKitPlus::POSE_ESTIMATOR)_Mode;
		if (m_PlusTracker)
			m_PlusTracker->setPoseEstimator(m_poseEstimMode);
	}

	int ARToolKitPlusTracker::getPosEstimMode()const
	{	return m_poseEstimMode;	}
	
	void ARToolKitPlusTracker::PrintOptions()const
	{
//		std::cout << "===== TRACKER OPTIONS : " << getLabel() <<   std::endl;
		std::cout << "* Threshold value	: " << m_threshold <<   std::endl;
		std::cout << "* Threshold auto	: " ;
			if (getAutoThreshold())
				std::cout <<"ON"<<  std::endl;
			else
				std::cout <<"OFF"<<  std::endl;
		std::cout << "* Image processing mode : ";
			if (m_imageProcMode)
				std::cout <<"FULL" <<   std::endl;
			else
				std::cout <<"HALF" <<   std::endl;
		std::cout << "* arDetectLite mode : ";
			if (m_useDetectLite)
				std::cout << "LITE"  <<   std::endl;
			else
				std::cout << "NORMAL" <<   std::endl;
		std::cout << "* Pose Estimation : ";
			if (m_poseEstimMode == ARToolKitPlus::POSE_ESTIMATOR_ORIGINAL)
				std::cout << "POSE_ESTIMATOR_ORIGINAL"  <<   std::endl;
			else if (m_poseEstimMode == ARToolKitPlus::POSE_ESTIMATOR_ORIGINAL_CONT)
				std::cout << "POSE_ESTIMATOR_ORIGINAL_CONT"  <<   std::endl;
			else if (m_poseEstimMode == ARToolKitPlus::POSE_ESTIMATOR_RPP)
				std::cout << "POSE_ESTIMATOR_RPP"  <<   std::endl;
			else
                std::cout << "Unknown" <<   std::endl;
		std::cout << "* Undistortion mode : ";
			if (m_undistortMode == ARToolKitPlus::UNDIST_LUT)
				std::cout << "UNDIST_LUT"  <<   std::endl;
			else if (m_undistortMode == ARToolKitPlus::UNDIST_NONE)
				std::cout << "UNDIST_NONE"  <<   std::endl;
			else if (m_undistortMode == ARToolKitPlus::UNDIST_STD)
				std::cout << "UNDIST_STD"  <<   std::endl;
			else
                std::cout << "Unknown" <<   std::endl;
	}

//++++++++++++++++++++++++++++++++++++
//	Accessing Functions Get/Set.......
//++++++++++++++++++++++++++++++++++++

	bool ARToolKitPlusTracker::setupTargets(const std::string& patternListFile)
	{
		osg::notify() << "Setting up markers from file : " << patternListFile << std::endl;
		std::ifstream targetFile;

		// Need to check whether the passed file even exists
		targetFile.open(patternListFile.c_str());

		// Need to check for error when opening file
		if (!targetFile.is_open())
		{
			osg::notify(osg::WARN) << "File does not exist : " << patternListFile << std::endl;
			return false;
		}

		bool ret = true;

		int patternNum = 0;
		targetFile >> patternNum;

		std::string patternName, patternType;

		// Need EOF checking in here... atm it assumes there are really as many targets as the number says

		for (int i = 0; i < patternNum; i++)
		{
			// jcl64: Get the whole line for the target file (will handle spaces in filename)
			patternName = "";
			while (osgART::trim(patternName) == "" && !targetFile.eof()) {
				getline(targetFile, patternName);
			}
			
			// Check whether targetFile exists?

			targetFile >> patternType;

			if (patternType == "SINGLE")
			{
				double width, center[2];
				targetFile >> width >> center[0] >> center[1];
				if (addSingleTarget(patternName, width, center) == -1) {
					std::cerr << "Error adding single pattern: " << patternName << std::endl;
					ret = false;
					break;
				}
			}
			else if (patternType == "MULTI")
			{
				if (addMultiTarget(patternName) == -1) {
					std::cerr << "Error adding multi-target pattern: " << patternName << std::endl;
					ret = false;
					break;
				}
			} 
			else 
			{
				std::cerr << "Unrecognized pattern type: " << patternType << std::endl;
				ret = false;
				break;
			}
		}
		targetFile.close();

		return ret;
	}

	//Single
	int ARToolKitPlusTracker::addSingleTarget(const std::string& pattFile, double width, double center[2]) {

		osgART::Target* singleTarget = new SingleTarget(m_PlusTracker);
		if (!static_cast<SingleTarget*>(singleTarget)->initialise(pattFile, width, center))
		{
			singleTarget->unref();
			return -1;
		}

		_targetList.push_back(singleTarget);

		return _targetList.size() - 1;//return the ID of the pattern
	}

	//Multi
	int ARToolKitPlusTracker::addMultiTarget(const std::string& multiFile) 
	{
		//we add directly the TrackerMultiMarker  object
		osgART::Target* multiMarker = new MultiTarget(m_PlusTracker,m_markerMode );

		if (!static_cast<MultiTarget*>(multiMarker)->initialise(multiFile))
		{
			multiMarker->unref();
			return -1;
		}

		_targetList.push_back(multiMarker);

		return _targetList.size() - 1;

	}

	 

    
	/*
	inline bool
		ARToolKitPlusTracker::update(osg::NodeVisitor* nv)
	{
		const osg::FrameStamp* framestamp = (nv) ? nv->getFrameStamp() : 0L;

		//check first if we get a new image to update
		//the tracking value
		if (_imagesource->valid() && 
			_imagesource->getModifiedCount() != _modifiedCount && 
			_imagesource->data() != NULL) {

				osg::notify(osg::DEBUG_INFO) << "osgART::ARToolKitPlusTracker::update()" << std::endl;

				osg::Timer t;

				t.setStartTick();

				{
					//lock the video image
					//OpenThreads::ScopedLock<OpenThreads::Mutex> _lock(_imagesource->getMutex());

					//potentially buffer it to a temporary data structure
					//and update your tracker

					//my_external_code->update(_imagesource->data());

				}

				//detect, compute pose of your targets
				//

				//report stats
				if (framestamp && _stats.valid())
				{
					_stats->setAttribute(framestamp->getFrameNumber(),
						"Tracking time taken", t.time_m());

					//_stats->setAttribute(framestamp->getFrameNumber(),
					//	"Possible candidates", m_target_num);
				}

				//

				//once it's done, you can update your specific targets with processed information

				for (osgART::Tracker::TargetListType::iterator iter = _targetList.begin();
					iter != _targetList.end();
					iter++)
				{

					if (DummyTarget* target = dynamic_cast<DummyTarget*>((*iter).get())) {

						//for this example we create a dummy transformation
						//and pass that as argument
						osg::Matrix dummy_mat_for_test;


						dummy_mat_for_test.identity();
						dummy_mat_for_test.set(
							-0.931019,-0.155908,0.329994,0,
							0.224063,0.46955, 0.854,0,
							-0.288095,0.86903,-0.40223,0,
							3.29111,4.70607,-74.1661,1);
						// dummy_mat_for_test.translate(0.,0.,1.);

						dummy_mat_for_test=osg::Matrix::rotate(osg::Quat(osg::DegreesToRadians(-90.0),osg::Vec3(1,0,0)))*dummy_mat_for_test;

						target->update(dummy_mat_for_test);

					}
				}
				//update the counter of the processed image
				_modifiedCount = _imagesource->getModifiedCount();
		}
		return true;
	}
	*/

	/*virtual*/ 	
	bool ARToolKitPlusTracker::update(osg::NodeVisitor* nv)
	{	
		//if(m_debugMode)
		//	osg::notify() <<  "->" <<  getLabel() << "::update()" << std::endl;
		
		if (!_imagesource.valid())
		{
			osg::notify(osg::WARN) << "No connected image source for the tracker" << std::endl;
			return false;
		}

		// Do not update with a null image.
		if (!_imagesource->valid())
		{
			osg::notify(osg::WARN) << "osgart_artoolkit_tracker: received NULL pointer as image"
				<< std::endl;
			return false;
		}

		// hse25: performance measurement: only update if the image was modified
		if (_imagesource->getModifiedCount() == _modifiedCount)
		{
			return false; 
		}
		
		// update internal modified count
		_modifiedCount = _imagesource->getModifiedCount();


		// \TODO: hse25: check here for the moment, the function needs to be extended
		if (m_PlusTracker->getPixelFormat() != getARPixelFormatForImage(*_imagesource.get()))
		{
			//check if format is compatible:
			int NewVideoFormat = getARPixelFormatForImage(*_imagesource.get());
			if(NewVideoFormat > 0)
			{
				setPixelFormat((ARToolKitPlus::PIXEL_FORMAT) NewVideoFormat);
			}
			else
			{
				osg::notify(osg::WARN) << "osgart_artoolkit_tracker::update() Incompatible pixelformat!" << std::endl;
                return false;
			}
		}

		// lock agains video updates
		//Video* video = dynamic_cast<Video*>(_imagesource.get());
		//if (video)
		//{
		//	video->getMutex().lock();
		//}

	ARToolKitPlus::ARMarkerInfo    *marker_info;
	float confidence = 0.0f;  

    // detect the markers in the video frame

	if(m_PlusTracker->arDetectMarker(const_cast<unsigned char*>(_imagesource->data()), m_threshold, &marker_info, &m_marker_num) < 0)
	{
			// TODO: unlock the mutex for a graceful shutdown
		return false;
	}

		osg::notify() << "arDetectMarker() => Markerdetected = " <<m_marker_num<<std::endl;

		// Check through the marker_info array for highest confidence
		// visible marker matching our preferred pattern.
		for (osgART::Tracker::TargetListType::iterator iter= _targetList.begin(); 
				iter != _targetList.end(); 
				iter++)		
		{
			SingleTarget*	singleTarget = dynamic_cast<SingleTarget*>((*iter).get());
			MultiTarget*	multiTarget  = dynamic_cast<MultiTarget*>((*iter).get());

			if (singleTarget)
			{
				// find best visible marker
				int j, k = -1;
				for(j = 0; j < m_marker_num; j++)
				{
					if (singleTarget->getPatternID() == marker_info[j].id)
					{
						if (k == -1) k = j; // First marker detected.
						else 
						if(marker_info[j].cf > marker_info[k].cf) k = j; // Higher confidence marker detected.
					}
				}

				if(k != -1) 
				{					
					singleTarget->update(&marker_info[k]);
					//__AR_DO_PROFILE(RecordMarkerStats(singleMarker, true));
				} 
				else 
				{
					singleTarget->update(NULL);
					//__AR_DO_PROFILE(RecordMarkerStats(singleMarker, false));
				}

				confidence = marker_info[k].cf;


				/////////////////////////////////////////////////////////////////////////
				//
				//       corner refinement begin
				//
			/*	if(false)
				{
					const unsigned int roi_radius = 4;
					for(unsigned int i=0; i<4; i++)
					{
						ARFloat edge_x, edge_y;
						int c_ret = refineCorner(edge_x, edge_y,
							marker_info[k].vertex[i][0],
							marker_info[k].vertex[i][1],
							roi_radius, (void*) nImage,
							arCamera->xsize, arCamera->ysize);

						if(c_ret == 1)
						{
							marker_info[k].vertex[i][0] = edge_x;
							marker_info[k].vertex[i][1] = edge_y;
						}
					}
				}*/
				//
				//       corner refinement end
				//
				/////////////////////////////////////////////////////////////////////////
				}
				else if (multiTarget)
				{
					multiTarget->update(marker_info, m_marker_num);
				}
				else {
					osg::notify(osg::WARN)<< "ARToolKitPlusTracker::update() : Unknown marker type id!" << std::endl;
				}
			}

		//if (video)
		//{
		//	video->getMutex().unlock();
		//}

		//if(m_debugMode)
		//	osg::notify() << "<-Stop" <<  getLabel() << "::update()" << std::endl;

		return true;
	}

	int ARToolKitPlusTracker::ConvertOSGARTPixelFormatToART(osgART::PixelFormatType format)const
	{
		using namespace ARToolKitPlus;
		switch (format)
		{
		case osgART::VIDEOFORMAT_RGB24: return PIXEL_FORMAT_RGB;
		case osgART::VIDEOFORMAT_BGR24:	return PIXEL_FORMAT_BGR;
		case osgART::VIDEOFORMAT_BGRA32:return PIXEL_FORMAT_BGRA;
		case osgART::VIDEOFORMAT_RGBA32:return PIXEL_FORMAT_RGBA;
		case osgART::VIDEOFORMAT_ABGR32:return PIXEL_FORMAT_ABGR;
		case osgART::VIDEOFORMAT_Y8:
		case osgART::VIDEOFORMAT_GREY8:
			return PIXEL_FORMAT_LUM;
		default:
			osg::notify(osg::WARN) << "ConvertOSGARTPixelFormatToART() : Unknown pixel format!" << std::endl;
			return 0;
		}        
		return 0;
	}

	osgART::PixelFormatType ARToolKitPlusTracker::ConvertARTPixelFormatToOSGART(int format)const
	{
		using namespace ARToolKitPlus;
		switch (format)
		{
		case PIXEL_FORMAT_RGB : return osgART::VIDEOFORMAT_RGB24;
		case PIXEL_FORMAT_BGR : return osgART::VIDEOFORMAT_BGR24;
		case PIXEL_FORMAT_BGRA :return osgART::VIDEOFORMAT_BGRA32;
		case PIXEL_FORMAT_RGBA :return osgART::VIDEOFORMAT_RGBA32;
		case PIXEL_FORMAT_ABGR :return osgART::VIDEOFORMAT_ABGR32;
		case PIXEL_FORMAT_LUM :return osgART::VIDEOFORMAT_Y8;//or VIDEOFORMAT_GREY8:
		default:
			osg::notify(osg::WARN) << "ConvertARTPixelFormatToOSGART() : Unknown pixel format!" << std::endl;

		}        
		return osgART::VIDEOFORMAT_ANY;
	}


	int ARToolKitPlusTracker::getARPixelFormatForImage(const osg::Image& _image) const
	{
		int format = 0, size = 0;
		
		if (_image.valid()) {
			switch (_image.getPixelFormat()) {
				case GL_RGBA:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = ARToolKitPlus::PIXEL_FORMAT_RGBA;
						size = 4;
					}
					break;
				case GL_ABGR_EXT:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = ARToolKitPlus::PIXEL_FORMAT_ABGR;
						size = 4;
					}
					break;
				case GL_BGRA:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = ARToolKitPlus::PIXEL_FORMAT_BGRA;
						size = 4;
					}
#if 0
	#ifdef AR_BIG_ENDIAN
					else if (_image.getDataType() == GL_UNSIGNED_INT_8_8_8_8_REV) {
						format = ARToolKitPlus::PIXEL_FORMAT_ARGB;
						size = 4;
					}
	#else
					else if (_image.getDataType() == GL_UNSIGNED_INT_8_8_8_8) {
						format = ARToolKitPlus::PIXEL_FORMAT_ARGB;
						size = 4;
					}
	#endif
#endif
					break;
				case GL_RGB:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = ARToolKitPlus::PIXEL_FORMAT_RGB;
						size = 3;
					}
					break;
				case GL_BGR:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = ARToolKitPlus::PIXEL_FORMAT_BGR;
						size = 3;
					}
					break;
#if 0
				case GL_YCBCR_422_APPLE:
				case GL_YCBCR_MESA:
	#ifdef AR_BIG_ENDIAN
					if (_image.getDataType() == GL_UNSIGNED_SHORT_8_8_REV_APPLE) {
						format = AR_PIXEL_FORMAT_2vuy; // N.B.: GL_UNSIGNED_SHORT_8_8_REV_APPLE = GL_UNSIGNED_SHORT_8_8_REV_MESA
						size = 2;
					} else if (_image.getDataType() == GL_UNSIGNED_SHORT_8_8_APPLE) {
						format = AR_PIXEL_FORMAT_yuvs; // GL_UNSIGNED_SHORT_8_8_APPLE = GL_UNSIGNED_SHORT_8_8_MESA
						size = 2;
					}
	#else
					if (_image.getDataType() == GL_UNSIGNED_SHORT_8_8_APPLE) {
						format = AR_PIXEL_FORMAT_2vuy;
						size = 2;
					} else if (_image.getDataType() == GL_UNSIGNED_SHORT_8_8_REV_APPLE) {
						format = AR_PIXEL_FORMAT_yuvs;
						size = 2;
					}
	#endif
					break;
#endif
				case GL_LUMINANCE:
					if (_image.getDataType() == GL_UNSIGNED_BYTE) {
						format = ARToolKitPlus::PIXEL_FORMAT_LUM;
						size = 1;
					}
					break;
				default:
					break;
			}
		}
		return (format);
	}

	int ARToolKitPlusTracker::getGLPixelFormatForARPixelFormat(const int arPixelFormat, GLenum *internalformat_GL, GLenum *format_GL, GLenum *type_GL) const
	{
		// Translate the internal pixelformat to an OpenGL texture2D triplet.
		switch (arPixelFormat) {
			case ARToolKitPlus::PIXEL_FORMAT_RGB:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_RGB;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case ARToolKitPlus::PIXEL_FORMAT_BGR:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_BGR;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case ARToolKitPlus::PIXEL_FORMAT_RGBA:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_RGBA;
				*type_GL = GL_UNSIGNED_BYTE;
			case ARToolKitPlus::PIXEL_FORMAT_BGRA:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_BGRA;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
#if 0
			case AR_PIXEL_FORMAT_ARGB:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_BGRA;
#ifdef AR_BIG_ENDIAN
				*type_GL = GL_UNSIGNED_INT_8_8_8_8_REV;
#else
				*type_GL = GL_UNSIGNED_INT_8_8_8_8;
#endif
				break;
#endif
			case ARToolKitPlus::PIXEL_FORMAT_ABGR:
				*internalformat_GL = GL_RGBA;
				*format_GL = GL_ABGR_EXT;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
			case ARToolKitPlus::PIXEL_FORMAT_LUM:
				*internalformat_GL = GL_LUMINANCE8;
				*format_GL = GL_LUMINANCE;
				*type_GL = GL_UNSIGNED_BYTE;
				break;
#if 0
			case AR_PIXEL_FORMAT_2vuy:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_YCBCR_422_APPLE; //GL_YCBCR_MESA
#ifdef AR_BIG_ENDIAN
				*type_GL = GL_UNSIGNED_SHORT_8_8_REV_APPLE; //GL_UNSIGNED_SHORT_8_8_REV_MESA
#else
				*type_GL = GL_UNSIGNED_SHORT_8_8_APPLE; //GL_UNSIGNED_SHORT_8_8_MESA
#endif
				break;
			case AR_PIXEL_FORMAT_yuvs:
				*internalformat_GL = GL_RGB;
				*format_GL = GL_YCBCR_422_APPLE; //GL_YCBCR_MESA
#ifdef AR_BIG_ENDIAN
				*type_GL = GL_UNSIGNED_SHORT_8_8_APPLE; //GL_UNSIGNED_SHORT_8_8_MESA
#else
				*type_GL = GL_UNSIGNED_SHORT_8_8_REV_APPLE; //GL_UNSIGNED_SHORT_8_8_REV_MESA
#endif
				break;
#endif
			default:
				return (-1);
				break;
		}
		return (0);
	}


// initializer for dynamic loading
osgART::PluginProxy<ARToolKitPlusTracker> g_dummytracker("osgart_tracker_dummytracker");


