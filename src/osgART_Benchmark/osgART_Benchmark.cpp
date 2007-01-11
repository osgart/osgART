/*
 *
 * Simple Example to demonstrate OSGART
 *
 * Copyright (c) 2005-2006
 * Yannick Allusse, Julian Looser, Philip Lamb, Rapha‘l Grasset, Hartmut Seichter.
 *
 */
#include <Producer/RenderSurface>
#include <osgProducer/Viewer>
#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Projection>
#include <osg/AutoTransform>
#include <osg/ShapeDrawable>
#include <osg/Geometry>
#include <osg/Material>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/FrontFace>
#include <osgART/Foundation>
#include <osgART/VideoManager>
#include <osgART/ARTTransform>
#include <osgART/TrackerManager>
#include <osgART/VideoBackground>
#include <osgART/VideoPlane>
//#include "../../src/osgART/Video/DummyImage/DummyImageVideo"

#include "../../src/osgART/Tracker/ARToolKitProfiler/ARToolKitTrackerProfiler"
#include "VideoBench"
#include "Keyboard"
#include "ProfilerTools"
//#pragma comment(lib ,"osgart_artoolkit_tracker_profiler.lib")

#if AR_TRACKER_PROFILE
	#define YCK_BENCH_VIDEO	1
	#include <SG_TLS_xml_container.h>
	#define YCK_ARTPLUGIN_MAXN_Nbr 10
	#define RUN_VIDEO_BENCH(FCT_1, FCT_2)  {FCT_1;}
#else
	#define YCK_ARTPLUGIN_MAXN_Nbr 10
	#define YCK_BENCH_VIDEO	0
	#define RUN_VIDEO_BENCH(FCT_1, FCT_2)	{FCT_2;}
#endif

	std::string ARPluginInfo[YCK_ARTPLUGIN_MAXN_Nbr][4];


#define ADD_AR_PLUGIN(PLUGIN_LIST, PLUGIN_ID, PLUGIN_DLL, MARKER_LIST, CAM_PARAM, PLUGIN_OPTION){\
	if (YCK_ARTPLUGIN_MAXN_Nbr > PLUGIN_ID+1){\
		PLUGIN_LIST[PLUGIN_ID][0]= PLUGIN_DLL;\
		PLUGIN_LIST[PLUGIN_ID][1]= MARKER_LIST;\
		PLUGIN_LIST[PLUGIN_ID][2]= CAM_PARAM;\
		PLUGIN_LIST[PLUGIN_ID][3]= PLUGIN_OPTION;\
		PLUGIN_ID++;\
	}\
}

#define AR_VIDEO_WIN32_DIRECTSHOW_2_71

#ifdef _WIN32
#  ifdef AR_VIDEO_WIN32_DIRECTSHOW_2_71
#    define MY_VCONF "data/WDM_camera_flipV.xml"
#  else
#    define MY_VCONF "showDlg,flipV"
#  endif
#elif defined(__linux)
// # define MY_VCONF "videotestsrc ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=artoolkit ! fakesink"
# define MY_VCONF "v4l2src use-fixed-fps=false ! decodebin ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=artoolkit ! fakesink"
#else
#  define MY_VCONF ""
#endif

#define DRAW_WIREFRAME_CUBE		1
#define USE_VIRTUAL_CAM			0	
#define USE_OPENCV_PROJECTION	0
#define SHARE_PROJECTION_MATRIX 0 //use the projection matrix of the first tracker

#define Verbose (Text){Text;}
int PlugInNbr = 0;
std::string VideoConfigString = MY_VCONF;
	
//return the number of tracker loaded
int InitARTracker(osg::ref_ptr<osgART::GenericVideo> _video)
{
//==================================================
//ADD the tracker settings to the ARPluginInfo table
//==================================================		


		std::string WoWeeFile = "WoWee/bench_multi.dat";
		//ARToolkit 2.7 
		std::string ART273MarkerList;
		//ART273MarkerList= "Data/multi_template_list.dat";
		//ART273MarkerList = "Data/markers_list.2.71.3.dat";//single template
		//ART273MarkerList = "Data/markers_list.plus.multi_template.dat";//multiple template
		ART273MarkerList = WoWeeFile;
	ADD_AR_PLUGIN (ARPluginInfo, PlugInNbr, "osgart_artoolkit_tracker", ART273MarkerList, "Data/camera_para.2.71.3.dat", "");
		

		//ARTAG has expired in end of 2006
//		std::string ARTagMarkerList;
//		ARTagMarkerList = "Data/markers_list.artag.dat";//single id
		//ARTagMarkerList = "Data/markers_list.artag.multi.dat";//multiple id
//	ADD_AR_PLUGIN (ARPluginInfo, PlugInNbr, "osgart_artag_tracker",	ARTagMarkerList, "Data/camera_para.2.71.3.dat", "");
		

		//ARTplus
		std::string ARTPlusMarkerList;
		std::string ARTPlusMode = "template";
		//ARTPlusMarkerList = "Data/markers_list.plus.single_template.dat";//single template
//		ARTPlusMarkerList = "Data/markers_list.plus.single_id.dat";ARTPlusMode = "id";//single ID
//		ARTPlusMarkerList = "Data/markers_list.plus.multi_template.dat";//multiple template
//		ARTPlusMarkerList = "Data/markers_list.plus.multi_id.dat";ARTPlusMode = "id";//multiple ID
		ARTPlusMarkerList = WoWeeFile;
	ADD_AR_PLUGIN (ARPluginInfo, PlugInNbr, "osgart_artoolkitplus_tracker",ARTPlusMarkerList, "Data/camera_para.plus.2.1.dat", ARTPlusMode);

		//ARToolkit 4
	//	std::string ARToolkit4MarkerList;// = "Data/multi_template_list.dat";
	//	ARToolkit4MarkerList = "Data/markers_list.4.dat";//single template
	//	ARToolkit4MarkerList = "Data/markers_list.4.multi.dat";//single ID
	//ADD_AR_PLUGIN (ARPluginInfo, PlugInNbr, "osgart_artoolkit4_tracker",	ARToolkit4MarkerList, "Data/camera_para.4.dat", "");
		
//==========================================================
//	Init and load all the trackers
//==========================================================
	std::cout <<"============== Start tracker(s)        =====================" << std::endl;
	osg::ref_ptr<osgART::GenericTracker> tracker = NULL;
		
	for (int i =0; i < PlugInNbr; i++ )
	{
		std::cout << "===========================================================" << std::endl;
		std::cout << "		Start tracker plugin :" <<  i +1 << std::endl;
		SG_Assert(ARPluginInfo[i][0]!="", "Wrong plugin number or plugin name list to small!");
		
		tracker = osgART::TrackerManager::createTrackerFromPlugin(ARPluginInfo[i][0]);
		
		SG_Assert(tracker.valid(),"Could not initialize tracker plugin!"); 
		std::cout << "Init tracker plugin settings :" <<  i +1 << std::endl;

		osg::ref_ptr< osgART::TypedField<std::string> > _Version = 
		dynamic_cast< osgART::TypedField<std::string>* >(tracker->get("version"));
		if (_Version.valid()) std::cout << "plugin " << i+1 << " is version : '" << _Version.get()->get() << "'"<< std::endl;

		//process specific options...
		if (ARPluginInfo[i][3]!="")
		{
			std::cout << "We have options for the Tracker : " << ARPluginInfo[i][3] << std::endl; 
			//artoolkit Plus
			if (ARPluginInfo[i][3]== "template")
			{
				osg::ref_ptr< osgART::TypedField<int> > _MarkerMode= 
				dynamic_cast< osgART::TypedField<int>* >(tracker->get("markermode"));
				if (_MarkerMode.valid())
					_MarkerMode->set(0);//ARToolKitPlus::MARKER_TEMPLATE
			}
			else if (ARPluginInfo[i][3]== "id")
			{
				osg::ref_ptr< osgART::TypedField<int> > _MarkerMode= 
				dynamic_cast< osgART::TypedField<int>* >(tracker->get("markermode"));
				if (_MarkerMode.valid())
					_MarkerMode->set(1);//ARToolKitPlus::MARKER_ID_SIMPLE
			}
			//========================
		}

		//
		SG_Assert(tracker->init(_video->getWidth(), _video->getHeight(), ARPluginInfo[i][1],ARPluginInfo[i][2]), "tracker->init failed!");

		/* RFC: this how you would get any type in and out through the plugin system */
		//threshold must be called after init, see ARToolkit4.
		osg::ref_ptr< osgART::TypedField<int> > _threshold = 
			dynamic_cast< osgART::TypedField<int>* >(tracker->get("threshold"));

		/* values can only be accessed through a get()/set() mechanism */
		
		if (_threshold.valid())
			_threshold->set(100);
		
		std::cout << "Number of marker loaded(after init) : "<< tracker->getMarkerCount() <<  std::endl;
		std::cout << "===========================================================" << std::endl <<std::endl<<std::endl;
	}
	SG_NOTICE_LOG("============== All tracker(s) initialised=====================\n");

	return i;
}

osg::ref_ptr<osgART::GenericVideo> InitVideo(std::string VideoConfig, int _BenchMode=0, osgART::CL_VideoBench * _Bench =NULL)
{
	/* load a video plugin */
		std::cout << "Start video plugin..." << std::endl;
		osgART::VideoConfiguration cfg;
		cfg.deviceconfig = (char*)VideoConfig.c_str();
		osg::ref_ptr<osgART::GenericVideo> video = NULL;

#if YCK_BENCH_VIDEO
		if (_Bench)
			video = _Bench->InitVideo(VideoConfig);
		else		
		{
			video = osgART::VideoManager::createVideoFromPlugin("osgart_artoolkit", cfg);
			video->open();
		}
#else
		video = osgART::VideoManager::createVideoFromPlugin("osgart_artoolkit", cfg);		
		video->open();
#endif
		SG_Assert(video.valid(), "Video plugin not valid!");

		/* open the video */
		//cout << "Open video..." << std::endl;
		return video;
}

osg::Group* create3DText(const std::string & text, const osg::Vec3& center, const osg::Vec4& color, float radius)
{

    osg::Geode* geode  = new osg::Geode;

////////////////////////////////////////////////////////////////////////////////////////////////////////
//    
// Examples of how to set up axis/orientation alignments
//

    float characterSize=radius*0.2f;
    
    osg::Vec3 pos(center.x()-radius*.5f,center.y()-radius*.5f,center.z()-radius*.5f);

    osgText::Text* text1 = new osgText::Text;
    text1->setFont("fonts/times.ttf");
    text1->setCharacterSize(characterSize);
    text1->setPosition(pos);
    text1->setAxisAlignment(osgText::Text::XY_PLANE);
    text1->setText(text);
	text1->setColor(color);
    geode->addDrawable(text1);
/*
    osgText::Text* text2 = new osgText::Text;
    text2->setFont("fonts/times.ttf");
    text2->setCharacterSize(characterSize);
    text2->setPosition(pos);
    text2->setAxisAlignment(osgText::Text::YZ_PLANE);
    text2->setText("YZ_PLANE");
    geode->addDrawable(text2);

    osgText::Text* text3 = new osgText::Text;
    text3->setFont("fonts/times.ttf");
    text3->setCharacterSize(characterSize);
    text3->setPosition(pos);
    text3->setAxisAlignment(osgText::Text::XZ_PLANE);
    text3->setText("XZ_PLANE");
    geode->addDrawable(text3);


    osgText::Text* text4 = new osgText::Text;
    text4->setFont("fonts/times.ttf");
    text4->setCharacterSize(characterSize);
    text4->setPosition(center);
    text4->setAxisAlignment(osgText::Text::SCREEN);

    osg::Vec4 characterSizeModeColor(1.0f,0.0f,0.5f,1.0f);

    osgText::Text* text5 = new osgText::Text;
    text5->setColor(characterSizeModeColor);
    text5->setFont("fonts/times.ttf");
    //text5->setCharacterSize(characterSize);
    text5->setCharacterSize(32.0f); // medium
    text5->setPosition(center - osg::Vec3(0.0, 0.0, 0.2));
    text5->setAxisAlignment(osgText::Text::SCREEN);
    text5->setCharacterSizeMode(osgText::Text::SCREEN_COORDS);
    text5->setText("CharacterSizeMode SCREEN_COORDS(size 32.0)");
    geode->addDrawable(text5);

    osgText::Text* text6 = new osgText::Text;
    text6->setColor(characterSizeModeColor);
    text6->setFont("fonts/times.ttf");
    text6->setCharacterSize(characterSize);
    text6->setPosition(center - osg::Vec3(0.0, 0.0, 0.4));
    text6->setAxisAlignment(osgText::Text::SCREEN);
    text6->setCharacterSizeMode(osgText::Text::OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT);
    text6->setText("CharacterSizeMode OBJECT_COORDS_WITH_MAXIMUM_SCREEN_SIZE_CAPPED_BY_FONT_HEIGHT");
    geode->addDrawable(text6);

    osgText::Text* text7 = new osgText::Text;
    text7->setColor(characterSizeModeColor);
    text7->setFont("fonts/times.ttf");
    text7->setCharacterSize(characterSize);
    text7->setPosition(center - osg::Vec3(0.0, 0.0, 0.6));
    text7->setAxisAlignment(osgText::Text::SCREEN);
    text7->setCharacterSizeMode(osgText::Text::OBJECT_COORDS);
    text7->setText("CharacterSizeMode OBJECT_COORDS (default)");
    geode->addDrawable(text7);

#if 1
    // reproduce outline bounding box compute problem with backdrop on.
    text4->setBackdropType(osgText::Text::OUTLINE);
    text4->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX);
#endif

    text4->setText("SCREEN");
    geode->addDrawable(text4);
*/
/*
    osg::ShapeDrawable* shape = new osg::ShapeDrawable(new osg::Sphere(center,characterSize*0.2f));
    shape->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::ON);
    geode->addDrawable(shape);
*/
    osg::Group* rootNode = new osg::Group;
    rootNode->addChild(geode);

    return rootNode;    
}

/**
*\brief Create a cube and link it to the given transformation matrix.
*
*/
bool MarkerTransformDrawCube(osg::ref_ptr<osgART::ARTTransform> MarkerTransForm, float boxSize, osg::Vec4 & Color)
{
	if (!MarkerTransForm.valid())
		return false;

	osg::ShapeDrawable* sd = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0, 0, boxSize / 2.0f), boxSize));
	sd->setColor(Color);

	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(sd);

	//add a wireFrameGrp
	osg::Group* WireFrameGrp = new osg::Group;
	WireFrameGrp->addChild(geode);
	MarkerTransForm->addChild(WireFrameGrp);

#if DRAW_WIREFRAME_CUBE
	//set the wireframe	
	osg::StateSet* stateset = new osg::StateSet;
    osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
    polyoffset->setFactor(-1.0f);
    polyoffset->setUnits(-1.0f);
    osg::PolygonMode* polymode = new osg::PolygonMode;
    polymode->setMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE);
    
	stateset->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
    stateset->setAttributeAndModes(polymode,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);

#if 1
    osg::Material* material = new osg::Material;
    stateset->setAttributeAndModes(material,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
#else
    // version which sets the color of the wireframe.
    osg::Material* material = new osg::Material;
    material->setColorMode(osg::Material::OFF); // switch glColor usage off
    // turn all lighting off 
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,0.0f,0.0f,1.0f));
    // except emission... in which we set the color we desire
    material->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0,1.0f,0.0f,1.0f));
    stateset->setAttributeAndModes(material,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
    stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
#endif
    stateset->setTextureMode(0,GL_TEXTURE_2D,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
  
    WireFrameGrp->setStateSet(stateset);

#endif//draw wire frame

 	return true;
}

/**
*\brief Create an ARTTranform for the given marker from the given tracker, then add a cube to it.
*
*/
osg::ref_ptr<osgART::ARTTransform> CreateMarkerTransform(int PatternID, int TrackerID, float boxSize, osg::Vec4 & Color)
{
	osg::ref_ptr<osgART::ARTTransform> markerTrans = new osgART::ARTTransform(PatternID,TrackerID);

	osg::ref_ptr<osgART::Marker> marker = markerTrans->getMarker();
	if (marker.valid()) 
		marker->setActive(true);

	MarkerTransformDrawCube(markerTrans, boxSize, Color);

	//add the label to the cube :
		//remove the path of the name
			std::string Name = marker->getName();
			int StartNamePos = Name.find_last_of("\\");
			if (StartNamePos==-1)
				StartNamePos = Name.find_last_of("/");
			
			if (StartNamePos>0)
				Name = Name.substr(StartNamePos+1, Name.length() - StartNamePos);


		markerTrans->addChild(create3DText(Name, osg::Vec3(0,100,0), Color, 80.));
	//================================
	return markerTrans;
}
/**
*\brief Create all the prejection/modelview matrix for the given tracker, create the corresponding scene graph.
*
*/

osg::Projection* CreateTrackerSceneGraph(int TrackerID, osg::Group* _SceneGrp, osgART::VideoBackground*_videoBckGrnd,osg::Matrix * _SharedProjectionMatrix )
{
	//get Tracker
	osg::ref_ptr<osgART::GenericTracker> tracker = osgART::TrackerManager::getInstance()->getTracker(TrackerID);
	//get its proj matrix
#if SHARE_PROJECTION_MATRIX
	static osg::Projection* projectionMatrix = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
#else
	osg::Projection* projectionMatrix = NULL;
	if (_SharedProjectionMatrix )
		projectionMatrix = new osg::Projection(*_SharedProjectionMatrix);
	else
		projectionMatrix = new osg::Projection(osg::Matrix(tracker->getProjectionMatrix()));
#endif

	//create ModelView
	osg::MatrixTransform* modelViewMatrix = new osg::MatrixTransform();
	projectionMatrix->addChild(modelViewMatrix);

	//create foreGround
	osg::Group* foregroundGroup	= new osg::Group();
	modelViewMatrix->addChild(foregroundGroup);
	if (_videoBckGrnd)
	{
		foregroundGroup->addChild(_videoBckGrnd);
	}
	foregroundGroup->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");
	

	//Create Scene
	//_SceneGrp = new osg::Group();
	foregroundGroup->addChild(_SceneGrp);
	_SceneGrp->getOrCreateStateSet()->setRenderBinDetails(5, "RenderBin");

	//sceneGroup->addChild(markerTrans1.get());
	return projectionMatrix;
}


/**
*\brief Create a new view depending on the matrix used.
*
*/
#if USE_VIRTUAL_CAM
osg::CameraNode* createRearView(osg::Vec2s &Pos, osg::Vec2s &Size, osg::Matrix &ProjectionMatt,osg::Matrix &ViewMatt, osg::Node* subgraph, const osg::Vec4& clearColour)
{
    osg::CameraNode* camera = new osg::CameraNode;

    // set the viewport
	float size = 200;
	camera->setViewport(0,0,100,100*3/4);//(int)Pos.x,(int)Pos.y,(int)Size.x,(int)Size.y);

    // set the view matrix
    camera->setCullingActive(false);    
    camera->setReferenceFrame(osg::Transform::RELATIVE_RF);
    camera->setTransformOrder(osg::CameraNode::POST_MULTIPLY);

	camera->setProjectionMatrix(ProjectionMatt);//osg::Matrixd::scale(-1.0f,1.0f,1.0f));
	camera->setViewMatrix(ViewMatt);			//osg::Matrixd::rotate(osg::inDegrees(180.0f),0.0f,1.0f,0.0f));

	//camera->setViewMatrixAsLookAt(

    // set clear the color and depth buffer
    camera->setClearColor(clearColour);
    camera->setClearMask(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // draw subgraph after main camera view.
    camera->setRenderOrder(osg::CameraNode::POST_RENDER);

    // add the subgraph to draw.
    camera->addChild(subgraph);
    
    // switch of back face culling as we've swapped over the projection matrix making back faces become front faces.
    camera->getOrCreateStateSet()->setAttribute(new osg::FrontFace(osg::FrontFace::CLOCKWISE));
    
    return camera;
}
#endif
int main(int argc, char* argv[]) {

	std::cout << "Start osgARTTest..." << std::endl;
	try
	{
#if YCK_BENCH_VIDEO
	//==================================================
	//Init benchmark system
	//==================================================
		//create VideoBench Object...
		std::cout << "VideoBench: create obj..." << std::endl;
		osgART::CL_VideoBench * Bench = new osgART::CL_VideoBench("BenchMark");
		Bench->SetWorkingPath("bench");
		//Bench->SetMode(osgART::CL_VideoBench::BenchMarking);
		std::cout << std::endl << "Welcome to ARTXXX test-bed." << std::endl;
		//\n Please choose wich running mode you want to use?" << std::endl;
		std::cout << std::endl << "	f - Frame recording mode." << std::endl;
		std::cout << std::endl << "	b - Benchmarking mode." << std::endl;
		std::cout << std::endl << "	v - load a video file(avi)." << std::endl;
		std::cout << std::endl << "	any key - Normal mode." << std::endl;		

		//default mode
		Bench->SetMode(osgART::CL_VideoBench::Idle);//BenchMarking 
		
		std::string Answer;
		std::cin >> Answer;

		switch (Answer.c_str()[0])
		{
			case 'f':
			case 'F':
				Bench->SetMode(osgART::CL_VideoBench::RecordingVideo);
				break;
			case 'b':
			case 'B':
				Bench->SetMode(osgART::CL_VideoBench::BenchMarking);
				break;
			case 'v':
			case 'V':
				Bench->SetMode(osgART::CL_VideoBench::Idle);
				VideoConfigString = "data/WDM_video_flipV.xml";
				break;
			default :
				Bench->SetMode(osgART::CL_VideoBench::Idle);//the bench object is idle
		}        		
		std::cout << std::endl << std::endl;
		

		if (!Bench->InitIRSpace())
			osg::notify(osg::WARN) << "Could not init the VisionSpace Trackers" << std::endl;

		std::string BenchFile = Bench->ConsoleInit(std::string("new.xml"), true);
#endif
		//===================================================

	//==================================================
	//Init OSGART FrameWork
	//==================================================
		osgARTInit(&argc, argv);

		std::cout << "Start viewer..." << std::endl;
		osgProducer::Viewer viewer;
		viewer.setUpViewer(osgProducer::Viewer::ESCAPE_SETS_DONE);
		viewer.getCullSettings().setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
	
	#if YCK_BENCH_VIDEO
		//link videobench keyboard to main viewer
			osgART::VideoBenchKeyboardEventHandler* keybBench = new osgART::VideoBenchKeyboardEventHandler(Bench);
			viewer.getEventHandlerList().push_front(keybBench);
		//===========================================
	#endif

	#ifndef __linux
		// somehow on Ubuntu Dapper this ends up in a segmentation fault
		viewer.getCamera(0)->getRenderSurface()->fullScreen(false);
	#endif

	#if YCK_BENCH_VIDEO
		osg::ref_ptr<osgART::GenericVideo> video = InitVideo(VideoConfigString,  Bench->GetMode(), Bench);
	#else
		osg::ref_ptr<osgART::GenericVideo> video = InitVideo(VideoConfigString, 0, NULL);
	#endif
		SG_Assert(video.valid(), "Video plugin is not valid!");

		InitARTracker(video);

		osg::ref_ptr<osgART::GenericTracker> tracker = NULL;

		
		//Adding video background
		SG_NOTICE_LOG("============== Adding background video  =====================\n");
		osgART::VideoBackground* videoBackground=new osgART::VideoBackground(video->getId());
		SG_Assert(videoBackground, "failed to get videoBackground!");

		videoBackground->setTextureMode(osgART::GenericVideoObject::USE_TEXTURE_RECTANGLE);
		videoBackground->init();
		SG_NOTICE_LOG("============== Adding background video done! =====================\n");


		SG_NOTICE_LOG("============== Getting projection matrix! =====================\n");
		osg::Projection* projectionMatrix = NULL;
	#if YCK_BENCH_VIDEO
		osgART::CL_OpenCVCamParam* OpenCVParam = Bench->GetOpenCVParam();
		OpenCVParam->PrintAllMatrix();

		osg::Matrix OpenCVProjectionMat;
		if(OpenCVParam)
		{
			OpenCVProjectionMat.set(OpenCVParam->GetProjectionMatrix());
			osgART::PrintMatrix("OpenCV Calib Matrix ", OpenCVProjectionMat);
			osgART::PrintMatrix("OpenCV cam transformation", OpenCVParam->GetVirtualCameraTransform());
			//projectionMatrix = new osg::Projection(TempMatrix);
			//osgART::PrintMatrix("ARToolkit Calib Matrix ", osg::Matrix(tracker->getProjectionMatrix()));
		}
	#endif

		//create all the scenes for the different tracker
		osg::Group* AllLibGrp=new osg::Group();	//contain all the ARTlib scene graph
		osg::Group* CurrentLibGrp=NULL;			//pointer to current ARTlib scene graph

#if USE_VIRTUAL_CAM
		osg::Group* VirtualViewGrp=new osg::Group();//used to store the all the ARTlib scene graph without the projection matrix
#endif
		float boxSize = 40.0f;
		osg::ref_ptr<osg::Group> root = new osg::Group;
		osg::ref_ptr<osgART::ARTTransform> MarkerTrans;
		osg::ref_ptr<osgART::ARTTransform> FirstMarkerTrans;

		//create projection/modelview matrix and group
		for (int i =0; i < PlugInNbr; i++ )
		{			
			CurrentLibGrp = new osg::Group();
			SG_Assert(CurrentLibGrp, "No current group for the tracker graph!");

#if USE_OPENCV_PROJECTION		
			if (i==0)
				projectionMatrix = CreateTrackerSceneGraph(i, CurrentLibGrp, videoBackground, &OpenCVProjectionMat);
			else
				projectionMatrix = CreateTrackerSceneGraph(i, CurrentLibGrp, NULL, &OpenCVProjectionMat);
#else
			if (i==0)
				projectionMatrix = CreateTrackerSceneGraph(i, CurrentLibGrp, videoBackground, NULL);
			else
				projectionMatrix = CreateTrackerSceneGraph(i, CurrentLibGrp, NULL, NULL);
#endif

			tracker = osgART::TrackerManager::getInstance()->getTracker(i);

			osg::ref_ptr< osgART::TypedField<std::string> > _VersionName= 
			dynamic_cast< osgART::TypedField<string>* >(tracker->get("version"));

			if (_VersionName.valid())
				osgART::PrintMatrix(_VersionName->get(), projectionMatrix->getMatrix());
			else
				osgART::PrintMatrix("Empty version", projectionMatrix->getMatrix());

			//add objects to the scene graph
			osg::Vec4 Color;
			switch(i)
			{
				case 0: Color.set(0,0,1,1);break;
				case 1: Color.set(0,1,0,1);break;
				case 2: Color.set(1,0,0,1);break;
				default:Color.set(1,1,1,1);break;
			}
			//set the color legend	
#if 0
			tracker->setColor(Color);
#endif
			//======================
			
			for(int j = 0; j < tracker->getMarkerCount(); j++)
			{
				MarkerTrans = CreateMarkerTransform(j, i, boxSize, Color).get();
				SG_Assert(MarkerTrans, "Could not get marker transformation!");

				CurrentLibGrp->addChild(MarkerTrans.get());
				if (i==0 && j ==0)
					FirstMarkerTrans = MarkerTrans;
			}

			AllLibGrp->addChild(projectionMatrix);
#if USE_VIRTUAL_CAM	
			//================
			//add obj to the VirtualScene
			VirtualViewGrp->addChild(CurrentLibGrp);
			//===================================
#endif
		}
		root->addChild(AllLibGrp);

		SG_NOTICE_LOG("============== Getting projection matrix done! =====================\n");

		RUN_VIDEO_BENCH(root->addChild(Bench->CreateVideoBenchNode("MainBenchNode")));

#if USE_VIRTUAL_CAM		
		//we use the first traker projection matrix to calibrate the view..??
		//get Tracker
		tracker = osgART::TrackerManager::getInstance()->getTracker(0);
		osg::Matrix * VirtualProjectionMatrix = new osg::Matrix;
		if (tracker.valid())
		{
			VirtualProjectionMatrix->set(osg::Matrix(tracker->getProjectionMatrix()));//get its proj matrix
		}

		//add a camera view
		osgART::PrintMatrix("Virtual Projection Matrix",*VirtualProjectionMatrix);
		osg::Matrix CameraProjMat;//(*VirtualProjectionMatrix);
		//CameraProjMat.scale(-1.0f,1.0f,1.0f);
		//osg::Matrix CameraViewMat(osg::Matrixd::rotate(osg::inDegrees(180.0f),0.0f,1.0f,0.0f));
		osg::Matrix CameraViewMat;
		
		//osg::Matrix CameraProjMat(OpenCVProjectionMat);//osg::Matrixd::scale(-1.0f,1.0f,1.0f));
		//osg::Matrix CameraProjMat;//osg::Matrixd::scale(-1.0f,1.0f,1.0f));
		//osg::Matrix CameraViewMat;//(osg::Matrixd::rotate(osg::inDegrees(180.0f),0.0f,1.0f,0.0f));
		
		osg::CameraNode* VirtualCamera =  createRearView(osg::Vec2s(10,10),osg::Vec2s (100,100), CameraProjMat, CameraViewMat,
				VirtualViewGrp, osg::Vec4(0.2,0.2,0.2,0.5));

		 root->addChild(VirtualCamera);
#endif
		viewer.setSceneData(root.get());

		viewer.realize();
		video->start();
		//==============================================
	
	//==================================================
	//Start MainLoop
	//==================================================
		std::cout << "Start main loop..." << std::endl;
		int FrameID = -1;
		
		RUN_VIDEO_BENCH(
			Bench->StartTimer();//used to have the time of each frame
			Bench->SetVideo(video);		
			Bench->SetTrackerThreshold(120);
			);

		while (!viewer.done())
		{
#if AR_TRACKER_PROFILE
			video = Bench->GetCurrentVideo();
			if ((Bench->GetState() != osgART::CL_VideoBench::ACT_STOP)
				&&(Bench->GetState() != osgART::CL_VideoBench::ACT_PAUSE))
#endif
				video->update();
		
			RUN_VIDEO_BENCH(
				if (!Bench->preUpdate()) //prepare the benchmark frame : load/take a screenshot...
					break;
			);
			
			for (int i =0; i < osgART::TrackerManager::getInstance()->GetCount(); i++ )
			{
				tracker = osgART::TrackerManager::getInstance()->getTracker(i);	
				if (tracker.valid())
				{
					RUN_VIDEO_BENCH(
						Bench->updateTracker(tracker);//bench ON
						,
						tracker->setImage(video.get());//bench OFF
						tracker->update();
						);
#if USE_VIRTUAL_CAM					
					if (i==0)
					{
						//get Marker Transform of the first marker
						//FirstMarkerTrans = new osgART::ARTTransform(0,0);
						if (FirstMarkerTrans.valid)
						{
							osg::Matrix FirstMarkerMatrix (FirstMarkerTrans->getMatrix());
							//osgART::PrintMatrix("FirstMarkerMatrix", FirstMarkerMatrix);
							VirtualCamera->setViewMatrixAsLookAt(osg::Vec3(0,0,0), FirstMarkerMatrix.getTrans(), osg::Vec3(1,0,0));
							//VirtualCamera->setViewMatrixAsLookAt(FirstMarkerMatrix.getTrans(), osg::Vec3(0,0,0), osg::Vec3(0,0,1));
						}
					}
#endif
				}
			}
			RUN_VIDEO_BENCH(Bench->postUpdate());
			RUN_VIDEO_BENCH(Bench->Update());

			viewer.sync();
			viewer.update();
			viewer.frame();
		}

RUN_VIDEO_BENCH(
		//SaveBench
			if (BenchFile != "")
			{
				std::cout << "VideoBench: save..." << std::endl;
				Bench->XMLSaveToFile("new.xml");
			}
		//=========================================
		);

		viewer.sync();
		viewer.cleanup_frame();
		viewer.sync();
		video->stop();
		video->close();	
	}//try

	catch(CAssertException &e)
	{
		std::cerr << "=================== Exception catched Start ================="<< std::endl << e.what() << std::endl;
		std::cerr << std::endl<< "=================== Exception catched End ================="<< std::endl;
		Sleep(10000);
		Terminate(-1, "");
	}	
}