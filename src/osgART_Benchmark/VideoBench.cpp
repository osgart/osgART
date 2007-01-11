#include "VideoBench"
#include "ARTracerVal"
#include "PosRecord"
#include "ProfilerTools"
#include <iomanip>
#include <osgART/TrackerManager>
#include <osgART/VideoManager>
//#include "../../Video/DummyImage/DummyImageVideo"
//#pragma comment(lib ,"osgart_DummyImage.lib")
#include "../../src/osgART/Tracker/ARToolKitProfiler/ARToolKitTrackerProfiler"

namespace osgART {

//-----------------------------------------------------------------------------
//==========================================
//==========================================
//==========================================
CL_VideoBench::CL_VideoBench(std::string _Name)
	: CL_XML_BASE_OBJ<std::string>(_Name, "Container"),
		FrameGrpMngr(NULL, "FrameGrpMngr", "FrameGrp", XML_TAG_VAL_ID),
		HeaderMngr("Headers"),
		CVCamParam(NULL),
#if YCK_BENCH_IR_TRAKER
		IRFlyButton(NULL),
		IRFlyCam(NULL),
		IRTriggerDown(false),
#endif
		BenchMode(Idle),
		CurrentTempScreenShotID(-1),
		CurrentFrame(NULL), 
		CurrentFrameGrp(NULL),
		CurrentFrameID(-1),
		CurrentFrameGrpID(-1),
		CurrentFrameNode(NULL),
		VideoBenchNode(NULL),
		VideoBenchGrp(NULL),
		CurrentState(ACT_STOP),
		NewFrame(false),
		ShowNewFrame(false),
		m_TTWK_Threshold(100),
		m_TTWK_ThresholdAuto(0),
		m_TTWK_PoseEstimMode(0),
		m_TTWK_DetectLite(0),
		m_TTWK_UndistortMode(0),
		m_TTWK_ImageProcMode(0)
	{
	}

CL_VideoBench::~CL_VideoBench()
{
}

void CL_VideoBench::SetWorkingPath(const std::string _Path)
{
	WorkingPath = _Path;
}


const std::string & CL_VideoBench::GetWorkingPath()const
{
	return WorkingPath;
}

void CL_VideoBench::StartTimer()
{
	StartTime.Run();
	CurrentTempScreenShotID = -1;
}


bool CL_VideoBench::InitIRSpace()
{
#if YCK_BENCH_IR_TRAKER
	IRFlyCam	= new osgVRPN::Tracker("DTrack@hitlvstracker", 0);
	IRFlyPatt	= new osgVRPN::Tracker("DTrack@hitlvstracker", 1);
	IRFlyButton	= new osgVRPN::Button("DTrack@hitlvstracker");
	IRTriggerDown = false;

	if (IRFlyCam && IRFlyPatt && IRFlyButton)
		return true;
#endif
	return false;
}

CL_FrameRecordGrp *
CL_VideoBench::AddNewFrameGrp()
{
	CurrentFrameGrpID++;
	CurrentFrameID = -1;
	CurrentFrameGrp = FrameGrpMngr.Add(CurrentFrameGrpID);

	return CurrentFrameGrp;
}



CL_FrameRecordGrp *	CL_VideoBench::SetFrameGrp(int GrpID)
{
	//std::cout  << "FrameGrpID = " << GrpID << std::endl;
	if(GrpID < 0)
		return NULL;

	if (FrameGrpMngr.GetCount()==0)
	{
		return NULL;
	}

	if (GrpID < FrameGrpMngr.GetCount())
	{
		CurrentFrameGrp = FrameGrpMngr.Get(GrpID);
		CurrentFrameGrpID = GrpID;
	}
	else
	{
		CurrentFrameGrp = NULL;//all frame grp have been processed
	}
	return CurrentFrameGrp;
}

CL_FrameRecord *	CL_VideoBench::SetFrame(int FrmID)
{
	//std::cout  << "FrameID = " << FrmID << std::endl;
	if(FrmID < 0)
		return NULL;

	if (!CurrentFrameGrp)//no active FrameGrp
	{//try to find one
		CurrentFrameID = -1;
		return NULL;
	}
			
	if (CurrentFrameGrp->FrameMngr.GetCount()==0)
	{
		osg::notify(osg::WARN) << "The frame manager is empty";
		return NULL;
	}

	if (FrmID < CurrentFrameGrp->FrameMngr.GetCount())
	{
		CurrentFrame = CurrentFrameGrp->FrameMngr.Get(FrmID);		
		CurrentFrameID = FrmID;
		if (CurrentFrame->GetFileName() !="")
		{
			SetField<osgART::GenericVideo, std::string>(m_videoFile, "image_file", CurrentFrame->GetFileName(), "Can not set new image file");
			//static_cast<DummyImageVideo*>(m_videoFile.get())->openNewImage(CurrentFrame->GetFileName());
		}
			
		else
			std::cout << std::endl << "Current Frame as no filename!!" << std::endl;

		NewFrame = true;
	}
	else
	{
		CurrentFrame = NULL;//all frame have been processed
		CurrentFrameID = -1;
		NewFrame = false;
	}
	return CurrentFrame;
}

CL_FrameRecordGrp *	
CL_VideoBench::GetNextFrameGrp()
{
	SetFrameGrp(CurrentFrameGrpID +1);
	SetFrame(0);
	return CurrentFrameGrp;
}


CL_FrameRecordGrp *	
CL_VideoBench::GetPreviousFrameGrp()
{
	SetFrameGrp(CurrentFrameGrpID -1);
	SetFrame(CurrentFrameGrp->FrameMngr.GetCount()-1);
	return CurrentFrameGrp;
}

CL_FrameRecord *
CL_VideoBench::AddNewFrame()
{
	if (CurrentFrameGrp)
	{
		//LastFrameID++;
		CurrentFrameID++;
		//create new filename
			//get roots name
			std::string FileName = WorkingPath + "\\" + XMLFileName.substr(0, XMLFileName.size()-4)+ "_";
			//add frameNumber
			FileName += ToCharStr(CurrentFrameGrpID,std::string());
			FileName += "_";
			FileName += ToCharStr(CurrentFrameID,std::string());

			//add Type
			FileName += ".bmp";
		//------------

			CurrentFrame = CurrentFrameGrp->FrameMngr.AddObj(new osgART::CL_FrameRecord(CurrentFrameID, "RecordMngr", "Record", XML_TAG_VAL_ID));
			CurrentFrame->SetFileName(FileName);
			CurrentFrame->CalculateTime(StartTime);
			NewFrame = true;
	}
	else
		CurrentFrame = NULL;

	//we clear the Frame node used to print details on the viewer.
	

	//============================================================
	return CurrentFrame;
}

CL_FrameRecord *	
CL_VideoBench::GetNextFrame()
{
	SetFrame(CurrentFrameID+1);
	if (!CurrentFrame)
		GetNextFrameGrp();

	return CurrentFrame;
}


CL_FrameRecord *	
CL_VideoBench::GetPreviousFrame()
{
	SetFrame(CurrentFrameID-1);
	
	if (!CurrentFrame)
		GetPreviousFrameGrp();

	return CurrentFrame;
}

void CL_VideoBench::ClearCurrentFrameNode()
{
	if (CurrentFrameNode)
	{
		CurrentFrameNode->removeChildren(0, CurrentFrameNode->getNumChildren());
	}
}

osg::Group* CL_VideoBench::CreateCurrentFrameNode()
{
	if (VideoBenchGrp && !CurrentFrameNode)
	{
		CurrentFrameNode = new osg::Group();
		CurrentFrameNode->setName("CurrentFrame");
		VideoBenchGrp->addChild(CurrentFrameNode);
	}
	return CurrentFrameNode;
}

osg::Group* CL_VideoBench::CreateVideoBenchNode(const std::string _name)
{
	if (!VideoBenchNode)
	{
		//create the node
		//osg::Projection * PNode 
		VideoBenchNode = new osg::Projection(osg::Matrixd::ortho2D(0, 640, 0, 480));
		//set the transform
		osg::MatrixTransform * MatTrans = new osg::MatrixTransform();
		MatTrans->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
		VideoBenchNode->addChild(MatTrans);

		//create the group
		VideoBenchGrp = new osg::Group();
		VideoBenchGrp->setName(_name);
		MatTrans->addChild(VideoBenchGrp);


		//
		float windowHeight = 640.0f;
		float windowWidth = 480.0f;
		osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");
		osg::Vec4	layoutColor(1.0f,1.0f,0.0f,1.0f);
		float	layoutCharacterSize = 10.0f;    
		float	margin			= 30.0f;
		float	interline		= 1.5f;

		osg::Geode* geode  = new osg::Geode;

		//Show tracker legends
#if 0
			osg::ref_ptr<osgART::GenericTracker> tracker = NULL;
			osg::ref_ptr< osgART::TypedField<string>	>  _version; 
		
			//start position
			osg::Vec3 textPos	(margin,windowHeight-margin,0.);
			osg::Vec3 OffsetPos	(0,-layoutCharacterSize*interline,0.);

				
			
			for (int i =0; i < osgART::TrackerManager::getInstance()->GetCount(); i++ )
			{
				tracker = osgART::TrackerManager::getInstance()->getTracker(i);	

				if (tracker.valid())
				{
					textPos += OffsetPos;
					layoutColor = tracker->getColor();

					_version = dynamic_cast< osgART::TypedField<std::string>* >(tracker->get("version"));
					if (_version.valid())
						geode->addDrawable(CreateText(_version.get().c_str(), textPos, layoutColor, layoutCharacterSize, font, osgText::Text::LEFT_TO_RIGHT));
				}
			}
		//=====================================
#endif
		NewFrame = true;//force the update
	}
	else
	{
		osg::notify(osg::DEBUG_INFO) << "Video bench already has a osg::group"<< std::endl;
	}
	return VideoBenchNode;	
}


osgART::CL_FrameRecord * CL_VideoBench::GetFirstFrameRecord()
{
	CL_XML_MNGR<osgART::CL_FrameRecordGrp , unsigned int>::iterator FirstFrameGrp = FrameGrpMngr.GetFirstIter(); 
	
	if (FirstFrameGrp==FrameGrpMngr.GetLastIter())
	{
		osg::notify(osg::WARN) << "CL_VideoBench::GetFirstFrameRecord() : No FrameGrp to process." << endl;
		return NULL;
	}

	CL_XML_MNGR<osgART::CL_FrameRecord , unsigned int>::iterator FirstFrame = (*FirstFrameGrp).second->FrameMngr.GetFirstIter(); 
	if (FirstFrame!= (*FirstFrameGrp).second->FrameMngr.GetLastIter())
	{
		return (*FirstFrame).second;
	}
	else
		return NULL;
}

void		CL_VideoBench::Update()
{
	if (!ShowNewFrame)
		return;
	
	CreateCurrentFrameNode();		//create only one node
	ClearCurrentFrameNode();		//clear the existing node
	if (CurrentFrame)
		CurrentFrame->Update(CurrentFrameNode);	//update it
	
	ShowNewFrame = false;
}

TiXmlElement* 
CL_VideoBench::XMLLoad(TiXmlElement* _XML_ROOT)
{
	TiXmlElement*  XML_Elm = CL_XML_BASE_OBJ<std::string>::XMLLoad(_XML_ROOT);
	SG_Assert(XML_Elm, "CL_VideoBench::XMLLoad() XML_Elm is empty");

	HeaderMngr.XMLLoad(XML_Elm);

	TiXmlElement* XMLFrameMngr = FrameGrpMngr.XMLLoad(XML_Elm);
	
	//if (FrameMngr)
		//XMLReadVal(_XML_ROOT, "LastFrameID", LastFrameID);

	//CVCamParam
	TiXmlElement* XMLCVCam = XMLGetElement(XML_Elm, "OpenCVCam");
	
	if (XMLCVCam)
	{
		CVCamParam = new CL_OpenCVCamParam("OpenCVCamParameters");
		CVCamParam->XMLLoad(XMLCVCam);
	}
	//============================================================

	return XML_Elm;
}

TiXmlElement* 
CL_VideoBench::XMLSave(TiXmlElement* _XML_ROOT)
{
	TiXmlElement*  XML_Elm = CL_XML_BASE_OBJ<std::string>::XMLSave(_XML_ROOT);
	SG_Assert(XML_Elm, "CL_VideoBench::XMLSave() XML_Elm is empty");

	HeaderMngr.XMLSave(XML_Elm);
	
	if(CVCamParam)
	{
		//TiXmlElement* XMLCVCam = new TiXmlElement( CVCamParam->GetMainTag().c_str());
		CVCamParam->XMLSave(XML_Elm);
		//_XML_ROOT->InsertEndChild(*XMLCVCam);
	}

	if (FrameGrpMngr.GetCount())
	{
		TiXmlElement* XMLFrameMngr = FrameGrpMngr.XMLSave(XML_Elm);
		//XMLWriteVal(_XML_ROOT, "LastFrameID", LastFrameID);
	}

	return XML_Elm;
}




std::string 
CL_VideoBench::ConsoleInit(std::string &_XMLFileName, bool _KeepSettings)
{
/*	if (BenchMode == Idle)
		return "";
*/

	bool loaded = false;
	XMLFileName = _XMLFileName;
	if (XMLFileName !="")
		if (XMLLoadFromFile(XMLFileName))
			loaded = true;

	std::cout << "===========Initialising OSGART benchmark tool==========" << std::endl;

	//??? test is we create the original bench, or if we just add ARToolkit benchs records
	if (loaded)
		std::cout << "* XML bench file '" << XMLFileName <<" loaded."<< std::endl;
	else
	{//we ask for a filename
		bool FileNameOK = false;
		while(!FileNameOK)
		{
			std::cout << "* Please enter the filename for this benchmark?"<< std::endl;
			std::cin >> XMLFileName;
			//XMLFileName = "new.xml"; //?????? remove it!!!!!!!!

			//check if a file exist
			if (CL_BASE_OBJ_FILE::FileExists(XMLFileName))
			{
				std::cout << "* File exists! Load it(r), re-write it(w), change file name (c)?"<< std::endl;
				std::string EraseFile;
				std::cin >> EraseFile;

				if (EraseFile == "r")//we load it 
				{	
					if (XMLLoadFromFile(XMLFileName))
						loaded = true;
					FileNameOK = true;
				}
				else if (EraseFile == "w")//erase it... do nothing
				{
					FileNameOK = true;
				}
			}
			else
				FileNameOK = true;
		}
	}	

	//make a backup of the file
	std::string BackupName = XMLFileName;
	BackupName +=".bak"; 
	XMLSaveToFile(BackupName);
	//==========================


	//if(BenchMode == RecordingVideo)
	{
		//=======================================
		//calibration start =====================
		//=======================================
#if YCK_BENCH_IR_TRAKER
		if (!loaded)
		{
			//ask to generate the OpenCV calibration file
			std::cout << "* Now calibrating the Camera and the IR tracking system." << std::endl << "Please run the OpencvCalibration filter."<< std::endl;
			//launch it automatically..??
			//include into the application??
			std::cout << "* When the calibration is done, before saving the file you should do the following steps."<< std::endl;
			std::cout << "	1- Set the camera/IrTracker in a stable position, the camera must be looking at the OpenCV pattern and the IRTracker must be visible from the IRTracking system."<< std::endl;
			std::cout << "  2- Click on the save button from calib filter and choose a filename."<< std::endl;
			std::cout << "  3- Press the first button from the IR FlyingStick."<< std::endl;
			std::cout << "Warning, it is important that the camera/IRTracker don't move between these two steps, otherwise all the measures will be false."<< std::endl;


			//wait for the user to press the firebutton : 
			bool DoRecord = false;
			osg::Matrix IRCVMatrix;
			while(!DoRecord)
			{

				IRFlyCam->update();
				IRFlyPatt->update();
				IRFlyButton->update();
				if (!IRTriggerDown && IRFlyButton->getButtonState(0)) 
				{			
					cout << "Flying Stick button pressed!! ======================" << endl;
					DoRecord = true;
					IRCVMatrix = IRFlyCam->getMatrix();
					IRTriggerDown = 1;
					Sleep(2000);//to avoid the button to be still pressed for the comming calibration
				}
				IRTriggerDown = IRFlyButton->getButtonState(0);
			}
		//as	k for the OpenCV param File : 
		//we ask for a filename
			std::cout << "* Please enter the OpenCV camera parameters file when ?"<< std::endl;
			std::string CVFileName;
			CVFileName = "d:\\calib.txt";//???????!!!!! remove it !!!!!!
			//std::cin >> CVFileName;

			if (CVFileName!="")
			{
				if (!CVCamParam)
					CVCamParam = new CL_OpenCVCamParam("OpenCVCamParameters");
				CVCamParam->ReadParamFile(CVFileName);
				//CVCamParam->ReadParamFile("data\\cvcam.txt");
				//store initiale position of the FlyStick and camera
				CL_PosRecord * TmpPos = CVCamParam->PosMngr.Add("IR_CAM");
				SG_Assert(TmpPos, "IR_CAM not found");
				TmpPos->SetTransform(IRCVMatrix);
				
				//ask for CV pattern calibration :
			
				TmpPos = CVCamParam->PosMngr.Add("IR_CV_PATT");
				SG_Assert(TmpPos, "IR_CV_Patt not found");
				TmpPos->SetTransform(CalibrateSquarePattern(2));
			}
			else
				std::cout << "* Empty FileName, calibration not terminated...please do it again...!" << std::endl;
		}
		else
		{//loaded, we just process the matrix for testing...
			
			osg::Matrix Diff;
			
			CL_PosRecord * TmpPos = CVCamParam->PosMngr.Get("IR_CAM");
			osg::Matrix Cam(TmpPos->GetTransform());
			
			TmpPos = CVCamParam->PosMngr.Get("IR_CV_PATT");			
			osg::Matrix patt(TmpPos->GetTransform());
			
			Diff.set(DiffMatrix(patt, Cam));

			PrintMatrix("IRCam Matrix from XML",		Cam);
			PrintMatrix("IRCVPatt Matrix from XML",		patt);	
			PrintMatrix("Calculated difference between both", Diff);
		}


		//ask for AR pattern calibration :
		std::cout << "* Please calibrate the ARToolkit pattern."<< std::endl;
		Current_IR_ARPattern_Mat = CalibrateSquarePattern(2);
#else
		osg::notify(osg::WARN) << endl << "NO VRPN support please set YCK_BENCH_IR_TRAKER to 1! Calibration can not be done..."<< endl;
#endif

		//=======================================
		//calibration end =======================
		//=======================================
		SGE::CL_XML_Header		* CurrentHeader = NULL;
		bool KeepSettings = _KeepSettings;
	/*
		if (loaded)
		{
			std::cout << "* Would you like to keep the following camera settings (0/1)?"<< std::endl;
			//list camera settings
			//???>....
			std::cin >> KeepSettings;
		}
	*/

		//Camera settings
		if (loaded && !_KeepSettings)//&& BenchMode == RecordingVideo)//keep camera settings
		{
			CurrentHeader = (SGE::CL_XML_Header*) HeaderMngr.Get("Camera");
			if (CurrentHeader)
			{
				std::cout << "* Would you like to keep the existing camera settings (0/1)?"<< std::endl;
				//list camera settings
			//	CurrentHeader->PrintAllObjects();
				std::cin >> KeepSettings;
				if(!KeepSettings)
					HeaderMngr.Delete("Camera");
				CurrentHeader = NULL;
			}
		}

		if(!KeepSettings )
		{
			//add new camera settings
			CurrentHeader = (SGE::CL_XML_Header*) HeaderMngr.AddObj(new SGE::CL_XML_Header("Camera"));
			SG_Assert(CurrentHeader, "No container");

			//add all the fields we want : 
			CurrentHeader->FlagString->AddObj	(new CL_PARAM::TypeStringFlg	("Brand"));
			CurrentHeader->FlagString->AddObj	(new CL_PARAM::TypeStringFlg	("Model"));
			CurrentHeader->FlagInt->AddObj		(new CL_PARAM::TypeIntFlg		("size_X"));
			CurrentHeader->FlagInt->AddObj		(new CL_PARAM::TypeIntFlg		("size_Y"));
			CurrentHeader->FlagInt->AddObj		(new CL_PARAM::TypeIntFlg		("frequency"));
			CurrentHeader->FlagString->AddObj	(new CL_PARAM::TypeStringFlg	("format"));
			CurrentHeader->FlagString->AddObj	(new CL_PARAM::TypeStringFlg	("light_type"));//hallogen, natural...
			CurrentHeader->FlagBool->AddObj		(new CL_PARAM::TypeBoolFlg		("auto_white_lvl"));
			CurrentHeader->FlagBool->AddObj		(new CL_PARAM::TypeBoolFlg		("auto_exposure"));
			CurrentHeader->FlagDouble->AddObj	(new CL_PARAM::TypeDoubleFlg		("Brightness"));
			CurrentHeader->FlagDouble->AddObj	(new CL_PARAM::TypeDoubleFlg		("Sharpen"));
			//then ask to process them :
			CurrentHeader->AskForValueInput();
		}
		//======================================================


		//Experience settings
		KeepSettings = _KeepSettings;
		if (loaded && !_KeepSettings)//
		{
			CurrentHeader = (SGE::CL_XML_Header*) HeaderMngr.Get("Experience");
			if (CurrentHeader)
			{
				std::cout << "* Would you like to keep the existing experience settings (0/1)?"<< std::endl;
				//list experience settings
				//CurrentHeader->PrintAllObjects();
				std::cin >> KeepSettings;
				
				if(!KeepSettings)
					HeaderMngr.Delete("Experience");
				CurrentHeader = NULL;
			}
		}

		if(!KeepSettings) 
		{
			//add new camera settings
			CurrentHeader = (SGE::CL_XML_Header*) HeaderMngr.AddObj(new SGE::CL_XML_Header("Experience"));
			SG_Assert(CurrentHeader, "No container");
			//add all the fields we want : 
			CurrentHeader->FlagString->AddObj(new CL_PARAM::TypeStringFlg	("Date"));
			CurrentHeader->FlagString->AddObj(new CL_PARAM::TypeStringFlg	("Time"));
			CurrentHeader->FlagString->AddObj(new CL_PARAM::TypeStringFlg	("Author"));
			CurrentHeader->FlagString->AddObj(new CL_PARAM::TypeStringFlg	("Mail"));
			CurrentHeader->FlagString->AddObj(new CL_PARAM::TypeStringFlg	("Comment"));
			//then ask to process them :
			CurrentHeader->AskForValueInput();
		}
		//======================================================


		//Frame records
		KeepSettings = _KeepSettings;
		if (loaded && !_KeepSettings )//
		{
			std::cout << "* Would you like to keep all the frame records(0/1)? Keeping post records means that new records will be added at the end of the file."<< std::endl;
			//list experience settings
			//???>....
			std::cin >> KeepSettings;
			
			if(!KeepSettings)
			{
				std::cout << "* Please confirm the suppression(0 to cancel/1)?"<< std::endl;
				std::cin >> KeepSettings;
				if(KeepSettings)//
					this->FrameGrpMngr.DeleteAllLocal();
			}
		}
	}//recordingmode



	//ready to add new records...	
	std::cout << "Saving existing settings..." << std::endl;	
	XMLSaveToFile(	XMLFileName);
	std::cout << "Ready to add new records..." << std::endl;	
	//======================================================

	return XMLFileName;
}
//==========================================
//==========================================
//==========================================

int 
	CL_VideoBench::GetMode()
	{
		return BenchMode;
	}

void 
	CL_VideoBench::SetMode(int mode)
	{
		//cout << "CL_VideoBench::SetMode()" << endl;
		BenchMode = mode;
	#if YCK_BENCH_IR_TRAKER
		if (IRFlyCam == NULL)
			InitIRSpace();
	#endif
		CurrentTempScreenShotID = -1;

		switch(BenchMode)
		{//select the right video source...
			case Idle:
			case RecordingVideo: SetVideo(m_videoStreaming);
				NewFrame = true;
				break;
			case BenchMarking:	 SetVideo(m_videoFile);
				NewFrame = true;
				break;
		}
	}

void 
	CL_VideoBench::SetState(VideoBenchAction _newState)
	{
		CurrentState = _newState;
	}

CL_VideoBench::VideoBenchAction 
	CL_VideoBench::GetState()
	{
		return CurrentState;
	}

CL_OpenCVCamParam * CL_VideoBench::GetOpenCVParam()const
{
	return CVCamParam;
}

void CL_VideoBench::SetVideo(osg::ref_ptr<osgART::GenericVideo> video)
{
	m_video = video;
}

osg::ref_ptr<osgART::GenericVideo> CL_VideoBench::GetCurrentVideo()
{
	return m_video;
}

osg::ref_ptr<osgART::GenericVideo> CL_VideoBench::InitVideo(std::string _VideoConf)
{
	/* load a video plugin */
	//	cout << "Start video plugin..." << endl;
		osgART::VideoConfiguration cfg;

	//init video for mode BenchMarking
		//we read the screenshots
		//load image manager, we will load the screenshots one by one in the main loop
		//get the first Image from the bench
		osgART::CL_FrameRecord * FirstFrame = GetFirstFrameRecord();
		SG_Assert(FirstFrame, "No first frame available in the BenchMark mananger" );
		cfg.deviceconfig = (char*)FirstFrame->GetFileName().c_str();

		m_videoFile = osgART::VideoManager::createVideoFromPlugin("osgart_dummyimage", cfg);

		SetField<osgART::GenericVideo, bool>(m_videoFile, "flip_vertical", true, "Can not set Flip vertical");

	/*	osg::ref_ptr<DummyImageVideo> DumImage = dynamic_cast<DummyImageVideo*> (m_videoFile.get());
		if (DumImage.valid())
		{
			osg::ref_ptr< osgART::TypedField<bool> > FlipV = 
				dynamic_cast< osgART::TypedField<bool>* >(DumImage->get("flip_vertical"));

			if (FlipV.valid())
				FlipV->set(true);
		}
	*/
		m_videoFile->open();

	//===========================
	
	//init video for mode Idle and Recording
		//we process normal video stream
		cfg.deviceconfig = (char *)_VideoConf.c_str();
		m_videoStreaming = osgART::VideoManager::createVideoFromPlugin("osgart_artoolkit", cfg);
		m_videoStreaming->open();
		//=======================================
	
	//select the current source
	if ( GetMode() == BenchMarking)
		m_video = m_videoFile;
	else
		m_video = m_videoStreaming;
        
	SG_Assert(m_video.valid(), "Video plugin not valid!");
	/* open the video */
	//cout << "Open video..." << endl;

	return m_video;
}

bool 
CL_VideoBench::preUpdate()
{
	//if first time?
	if (CurrentFrameID == -1 && CurrentFrameGrpID == -1 )
		GetNextFrameGrp();	//Select First FrameGrp : 

	ProcessActions();
	
	switch(GetMode())
	{
		case Idle: break;
		case RecordingVideo :
			//wait the user to shoot at the marker
				#if YCK_BENCH_IR_TRAKER
					// Update from VRPN
					IRFlyCam->update();
					IRFlyPatt->update();
					IRFlyButton->update();

					if (!IRTriggerDown && IRFlyButton->getButtonState(0)) 
					{			
						cout << "Flying Stick button pressed!! ======================" << endl;
						CurrentTempScreenShotID = 0;//reset Frame Counter to 0, so we will record the next N frames
					}
					IRTriggerDown = IRFlyButton->getButtonState(0);
				#else

				#endif
			break; 
		case BenchMarking :
			if (CurrentState == ACT_PLAY )
			{
				//grab existing frame	
				if (!GetNextFrame())
				{
					SG_NOTICE_LOG("No more frame to process");
					SetState(ACT_STOP);//stop the profiling
				}
			}
			//else we are paused or stoped and do nothing

				//------------------------------------
			break;
	}

	if (NewFrame)
		ShowNewFrame = true;
	return true;
}

bool 
CL_VideoBench::postUpdate(void)
{
	osgART::CL_PosRecord *		FLyRecord = NULL;
	
	switch(GetMode())
	{
		case Idle: break;
		case RecordingVideo :

			if (CurrentTempScreenShotID ==0)
				AddNewFrameGrp();//we create a new grp of frame

			if (CurrentTempScreenShotID>=0 && CurrentTempScreenShotID<10)
			{
				//add a new frame 
					CurrentFrame = AddNewFrame();
					std::cout << "Taking snapShot to file : " << CurrentFrame->GetFileName() << std::endl;
					m_video->takeSnapShot(CurrentFrame->GetFileName());
				//----------------------------------
				#if YCK_BENCH_IR_TRAKER
						cout << "Taking New Record!! ======================" << endl;
					//Add IR-TRACKER Camera val
						FLyRecord = CurrentFrame->PosMngr.Add("IRTrackerCam");
						IRFlyCam->update();
						FLyRecord->SetTransform(IRFlyCam->getMatrix());
					//==================================

					//Add FirstMarker val
						FLyRecord = CurrentFrame->PosMngr.Add("patt_0");
						//IRFlyPatt->update();
						FLyRecord->SetTransform( Current_IR_ARPattern_Mat);// IRFlyPatt->getMatrix());
					//==================================
				#else //we simulate positions
				//Add IR-TRACKER Camera val
					FLyRecord = CurrentFrame->PosMngr.Add("IRTrackerCam");
					FLyRecord->SetTransform( osg::Matrix(0,01,02,03,10,11,12,13,20,21,22,23,30,31,32,33));
				//==================================

				//Add FirstMarker val
					FLyRecord = CurrentFrame->PosMngr.Add("patt_0");
					FLyRecord->SetTransform( osg::Matrix(0,01,02,03,10,11,12,13,20,21,22,23,30,31,32,33));
				//==================================
		
				#endif
				CurrentTempScreenShotID++;
			}
			break;
		case BenchMarking :
			if (CurrentState != ACT_STOP)
			{
				//the pattern stats recording is done into each ARTplugins,see ARToolKitTrackerProfiler::RecordMarkerStats()
				//FrameID++;
				//We calculate the RMS of the lib.
				static osg::Matrix CamCalib (CVCamParam->GetProjectionMatrix());
				if (CurrentFrame && NewFrame)
				{
					osg::Vec3d RMS = CurrentFrame->CalculateRMS("4.1.0", CamCalib);
				}
			}
			break;
	}

	NewFrame = false;

	return true;
}

void 
CL_VideoBench::updateTracker(osg::ref_ptr<osgART::GenericTracker> _tracker)
{
	if (GetMode() == BenchMarking && NewFrame)//CurrentState != ACT_STOP)
	{
		//check if the tracker is a ARToolKitTrackerProfiler and update it
		osgART::ARToolKitTrackerProfiler<GenericTracker>* TempTracker = dynamic_cast<osgART::ARToolKitTrackerProfiler<GenericTracker>*>(_tracker.get());

#if AR_TRACKER_PROFILE
		if (TempTracker)
			TempTracker->setCurrentBenchFrame(CurrentFrame);
#endif
		//if (CurrentState == ACT_PLAY || NewFrame == true)
		//{
			_tracker->setImage(m_video.get());
			_tracker->update();
		//}
	}
	else if (GetMode() == Idle)
	{
		_tracker->setImage(m_video.get());
		_tracker->update();
	}
}

void CL_VideoBench::AddAction(VideoBenchAction _Action)
{
	//change it into a FiFO and not a LILO (last in last out)
	ActionBuffer.push_back(_Action);
}

void CL_VideoBench::ProcessActions()
{
	while(ActionBuffer.size()!=0)
	{
		VideoBenchAction CurAction = ActionBuffer[ActionBuffer.size()-1];
//		osg::ref_ptr<osgART::GenericTracker> tracker = NULL;
		switch(CurAction)
		{
			//VideoBench Management
			case ACT_NOTHING :
			case ACT_GET_NEXT_FRAME :
				GetNextFrame();
				break;
			case ACT_GET_PREVIOUS_FRAME:
				GetPreviousFrame();
				break;			
			case ACT_GET_NEXT_FRAME_GRP:
				GetNextFrameGrp();
				//GetNextFrame();
				break;
			case ACT_GET_PREVIOUS_FRAME_GRP:
				GetPreviousFrameGrp();
				//GetNextFrame();
				break;
			case ACT_PLAY:
			case ACT_PAUSE:
				if (CurrentState == ACT_PLAY)
					SetState(ACT_PAUSE);
				else
					SetState(ACT_PLAY);
				PrintAllTrakersOptions();
				break;
			case ACT_STOP:
				SetState(ACT_STOP);
				break;
			case ACT_GET_FIRST_FRAME:
				SetFrameGrp(0);
				SetFrame(0);
				SetState(ACT_STOP);
				break;
			case ACT_REFRESH_FRAME:
				NewFrame = true;
				break;
			case ACT_SET_IDLE:
				SetMode(Idle);
				break;
			case ACT_SET_RECORDING:
				SetMode(RecordingVideo);
				break;
			case ACT_SET_BENCHMARKING:
				SetMode(BenchMarking);
				break;
			//Tracker Tweaking
			case ACT_THRESHOLD_DOWN:
				SetTrackerThreshold(m_TTWK_Threshold - 10);
				NewFrame = true;
				break;
			case ACT_THRESHOLD_UP:
				SetTrackerThreshold(m_TTWK_Threshold + 10);
				NewFrame = true;
				break;			
			case ACT_THRESHOLD_AUTO_SWITCH:
				m_TTWK_ThresholdAuto =!m_TTWK_ThresholdAuto;
				SetTrackersValue<bool>("autothreshold", m_TTWK_ThresholdAuto);
				NewFrame = true;
				break;			
			case ACT_IMG_PROC_MODE_SWITCH:
				m_TTWK_ImageProcMode =!m_TTWK_ImageProcMode;
				SetTrackersValue<int>("img_proc_mode", m_TTWK_ImageProcMode);
				NewFrame = true;
				break;
			case ACT_POSE_ESTIM_SWITCH:
				m_TTWK_PoseEstimMode +=1;
				if (m_TTWK_PoseEstimMode > _VIDEO_BENCH_TTWK_POS_ESTIM_MAX)
					m_TTWK_PoseEstimMode =0;
				SetTrackersValue<int>("pos_estim_mode", m_TTWK_PoseEstimMode);
				NewFrame = true;
				break;
			case ACT_DETECTLITE_SWITCH:
				m_TTWK_DetectLite =!m_TTWK_DetectLite;
				SetTrackersValue<bool>("use_detect_lite", m_TTWK_DetectLite);
				NewFrame = true;
				break;
			case ACT_UNDISTORT_MODE:
				m_TTWK_UndistortMode +=1;
				if (m_TTWK_UndistortMode > _VIDEO_BENCH_TTWK_UNDISTORT_MODE_MAX)
					m_TTWK_UndistortMode =0;
				SetTrackersValue<int>("undistort_mode", m_TTWK_UndistortMode);
				NewFrame = true;
				break;
			default : 
				osg::notify(osg::WARN) <<"Unkown action '" <<  CurAction << "'" << std::endl;
		}
		ActionBuffer.pop_back();
	}
}

#if YCK_BENCH_IR_TRAKER
osg::Matrix 
CL_VideoBench::CalibrateSquarePattern(int CalibrationNbr)
{
	
	//check that the pattern calibration is already stored in the XML file

	//create/update it?

	//create...

	//do the calibration 'CalibrationNbr' times
	bool DoRecord = false;
	osg::Vec3d	tmpVect[4], SumVect[4], AvgVect[4];
	osg::Matrix tmpMatrix;
	IRTriggerDown = 1;

	for(int i=0; i<CalibrationNbr ; i ++)
	{
		for(int j=0; j< 4 ; j ++)//calibrate the 4 corners of the the CV pattern
		{	//start on upper left, then process clockwise
			//take only he position

			DoRecord = false;
			cout << "Calibrate the corner '"<< j <<"'" << endl;
			while(!DoRecord)
			{
				IRFlyCam->update();
				IRFlyPatt->update();
				IRFlyButton->update();
				if (!IRTriggerDown && IRFlyButton->getButtonState(0)) 
				{			
					cout << "Flying Stick button pressed!! ======================" << endl;
					DoRecord = true;
				}
				IRTriggerDown = IRFlyButton->getButtonState(0);
			}
			//take the position of the flyingstick
			//Add IR-TRACKER Camera val
				tmpMatrix = IRFlyCam->getMatrix();
				tmpVect[j] = tmpMatrix.getTrans();
				SumVect[j] += tmpVect[j];
				PrintMatrix("", tmpMatrix);
			//==================================
		}
	}

	//make average of them
	cout << "Process average!!"<< endl;
	for(int i=0; i<4 ; i ++)
	{
		AvgVect[i] = SumVect[i]/CalibrationNbr;
		//PrintVector3D("", AvgVect[i]);
	}

	//find center, top and right
	osg::Vec3d	pattCenter, pattYVec, pattXVec, pattZVec;
	osg::Vec3d	RotVect;
	pattCenter	= (AvgVect[0] + AvgVect[1] + AvgVect[2] + AvgVect[3])/4;
	pattYVec	= (AvgVect[0] + AvgVect[1] )/2;		//top points
	pattXVec	= (AvgVect[1] + AvgVect[2] )/2;	//right points
	
	//move top and right to a new repere with origine pattCenter.
	pattYVec	-= pattCenter;
	pattXVec	-= pattCenter;

	//convert the unit..?
	//pattXVec = pattXVec *10;
	//pattYVec = pattYVec *10;

	pattZVec	= pattXVec^pattYVec ;

	PrintVector3D("pattCenter", pattCenter);
	PrintVector3D("pattXVec", pattXVec);
	PrintVector3D("pattYVec", pattYVec);
	PrintVector3D("pattZVec", pattZVec);
	
	//get transform matrix...
		int i = 0;
		osg::Matrix PMat(
		//x rot=======================
		pattXVec[0],
		pattXVec[1],
		pattXVec[2],
		0,
		//Y rot
		pattYVec[0],
		pattYVec[1],
		pattYVec[2],
		0,
		//z rot
		pattZVec[0],
		pattZVec[1],
		pattZVec[2],
		0,
		//position 
		pattCenter[0],
		pattCenter[1],
		pattCenter[2],
		1);
		//============================
	
		//PMat.setTrans(pattCenter);
		//PMat.makeRotate(RotVect[0], osg::Vec3d(1,0,0), RotVect[1], osg::Vec3d(0,1,0), RotVect[2], osg::Vec3d(0,0,2));
		PrintMatrix("Pattern", PMat);

		return PMat;
}
#endif//IRTracker




template <typename ValType>void CL_VideoBench::SetTrackersValue(std::string ValName, ValType _val)
{
	osg::ref_ptr<osgART::GenericTracker> tracker = NULL;
	osg::ref_ptr< osgART::TypedField<ValType> > _ValRef = NULL;

	for (int i =0; i < osgART::TrackerManager::getInstance()->GetCount(); i++ )
	{
		tracker = osgART::TrackerManager::getInstance()->getTracker(i);	
		if (tracker.valid())
		{
			_ValRef = dynamic_cast< osgART::TypedField<ValType>* >(tracker->get(ValName));
			if (_ValRef.valid())
				_ValRef->set(_val);
			else
				cout << "Current val '" << ValName << " not found." << endl;

		}
	}
	cout << "Current val '" << ValName << "' set to "<< _val << endl;

	NewFrame = true;//force the update

}

void CL_VideoBench::SetTrackerThreshold(int _val)
{
	m_TTWK_Threshold = _val;
	if (m_TTWK_Threshold < 0 )
		m_TTWK_Threshold = 0;
	else if (m_TTWK_Threshold > 255 )
		m_TTWK_Threshold = 255;

	SetTrackersValue<int>("threshold", m_TTWK_Threshold);
}

void CL_VideoBench::PrintAllTrakersOptions()
{
	osg::ref_ptr<osgART::GenericTracker> tracker = NULL;

	for (int i =0; i < osgART::TrackerManager::getInstance()->GetCount(); i++ )
	{
		tracker = osgART::TrackerManager::getInstance()->getTracker(i);	
		if (tracker.valid())
		{
			tracker->PrintOptions();
		}
	}
	NewFrame = true;//force the update
}

};//namespace