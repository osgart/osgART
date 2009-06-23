#include "FrameRecord"

#if AR_TRACKER_PROFILE
//#include <osgART\LineGeometry>


namespace osgART {

CL_FrameRecordGrp::CL_FrameRecordGrp(unsigned int _GrpID)
	:CL_XML_BASE_OBJ<unsigned int>(_GrpID),
	FrameMngr(NULL, "FrameMngr", "Frame", XML_TAG_VAL_ID)
{
	
}

CL_FrameRecordGrp::~CL_FrameRecordGrp()
{

}

#if _SG_TLS_XML
TiXmlElement* CL_FrameRecordGrp::XMLLoad(TiXmlElement* _XML_ROOT)
{
	SG_Assert(_XML_ROOT, "CL_FrameRecordGrp::XMLLoad() _XML_ROOT is empty");
	TiXmlElement*  XML_Elm = CL_XML_BASE_OBJ<unsigned int>::XMLLoad(_XML_ROOT);

	SG_Assert(XML_Elm, "CL_FrameRecordGrp::XMLLoad() XML_Elm is empty");

	return FrameMngr.XMLLoad(XML_Elm);
}

TiXmlElement* CL_FrameRecordGrp::XMLSave(TiXmlElement* _XML_ROOT)
{
	SG_Assert(_XML_ROOT, "CL_FrameRecordGrp::XMLSave() _XML_ROOT is empty");
	TiXmlElement*  XML_Elm = CL_XML_BASE_OBJ<unsigned int>::XMLSave(_XML_ROOT);
	SG_Assert(XML_Elm, "CL_FrameRecordGrp::XMLSave() XML_Elm is empty");

	return FrameMngr.XMLSave(XML_Elm);
}
#endif


//===========================================================
//					CL_FrameRecord 
//===========================================================
CL_FrameRecord::CL_FrameRecord(const unsigned int _FrameID,
						std::string _tagname,
						std::string _subtagname, 
						std::string _IDtagname)
	:CL_XML_BASE_OBJ<unsigned int>(_FrameID),
	PosMngr(NULL, _tagname, _subtagname, _IDtagname)
{}

CL_FrameRecord::~CL_FrameRecord()
{}

const SG_TRC::CL_TimerVal &	
CL_FrameRecord::CalculateTime(const SG_TRC::CL_TimerVal & _StartTime)
{
	Time.Run();//get the system time of the execution...	
	Time -= _StartTime;
	return Time;
}

void 
CL_FrameRecord::SetFileName(const std::string _FileName){	FileName = _FileName; }

const std::string & 
CL_FrameRecord::GetFileName() const
{	return FileName; }

/*
void CL_FrameRecord::SetARmatrix(osg::Matrix _ArMatrix)
{
	m_IRARPattTransVector.set	(_ArMatrix.getTrans()*10);
	m_IRARPattRotVector.set		(_ArMatrix.getRotate().asVec3());
}
*/

TiXmlElement* 
CL_FrameRecord::XMLLoad(TiXmlElement* _XML_ROOT)
{
	TiXmlElement*  XML_Elm = CL_XML_BASE_OBJ<unsigned int>::XMLLoad(_XML_ROOT);
	SG_Assert(XML_Elm, "CL_FrameRecord::XMLLoad() XML_Elm is empty");

	XMLReadVal		(XML_Elm, "FrameFile", FileName);
	_SG_DEBUG_XML(SG_NOTICE_LOG("Reading Frame " << FileName));
	_SG_DEBUG_XML(cout << std::endl);
	Time.XMLLoad	(XML_Elm,	"Time");

	PosMngr.XMLLoad(XML_Elm);

/*
	CL_XML_MNGR<CL_PosRecord, std::string>::iterator iPos;
	for (iPos = PosMngr.GetFirstIter(); iPos != PosMngr.GetLastIter(); iPos++)
	{
		(*iPos).second->Print("PosRecord");
	}
*/
	return XML_Elm;
}

TiXmlElement* 
CL_FrameRecord::XMLSave(TiXmlElement* _XML_ROOT)
{
	TiXmlElement*  XML_Elm = CL_XML_BASE_OBJ<unsigned int>::XMLSave(_XML_ROOT);
	SG_Assert(XML_Elm, "CL_FrameRecord::XMLSave() XML_Elm is empty");

	XMLWriteVal		(XML_Elm, "FrameFile", FileName);
	_SG_DEBUG_XML(SG_NOTICE_LOG("Writing Frame " << FileName));
	_SG_DEBUG_XML(cout << std::endl);
	PosMngr.XMLSave(XML_Elm);
	Time.XMLSave	(XML_Elm,	"Time");

	return XML_Elm;
}

osg::Matrix CL_FrameRecord::CalculateIRTransform()
{
	CL_PosRecord * IRCam	= PosMngr.Get("IRTrackerCam");
	CL_PosRecord * IRMarker = PosMngr.Get("patt_0");

	SG_Assert(IRCam,"IRCam empty");
	SG_Assert(IRMarker,"IRMarker empty");

	osg::Matrix CamMatt		= IRCam->GetTransform();
	osg::Matrix MarkerMatt	= IRMarker->GetTransform();

	osg::Matrix Result(DiffMatrix(MarkerMatt, CamMatt));
	PrintMatrix	("CalculateIRTransform Before Scale*100", Result);
	osg::Matrix ScaleMatt;
	ScaleMatt.makeScale(100,100,100);
	Result.set(Result*ScaleMatt); 
	PrintMatrix	("CalculateIRTransform After Scale*100", Result);
	
	return Result;
}

/*
osg::Matrix CL_FrameRecord::CalculateARTransform()
{
	CL_PosRecord * IRCam	= PosMngr.Get("IRTrackerCam");
	CL_PosRecord * IRMarker = PosMngr.Get("patt_0");

	SG_Assert(IRCam,"IRCam empty");
	SG_Assert(IRMarker,"IRMarker empty");

	osg::Matrix CamMatt		= IRCam->GetTransform();
	osg::Matrix MarkerMatt	= IRMarker->GetTransform();

	
	osg::Matrix Result = CamMatt.inverse(CamMatt) * MarkerMatt;
	PrintMatrix("CalculateIRTransform", Result);
	return Result;
}
*/

osg::Vec3d CL_FrameRecord::CalculateRMS(const std::string VersionName, osg::Matrix & _CamCalib)
{
	CL_PosRecord * Marker = PosMngr.Get("patt_0");
	SG_Assert(Marker,"IRMarker empty");

	//get the lib from the version.
	SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> * ARTlib = Marker->GetARRecord(VersionName);
	if (!ARTlib)
		return osg::Vec3d(0.,0.,0.);

	//get the specific parameter set of the lib.
	ARTlib->Process();
	CL_TRACER_RECORD<osgART::CL_ARTracerVal> * Records = ARTlib->TracerRecorded->GetFirstObj();
	if (!Records)
	{
		if ( ARTlib->TracerRecorded->GetCount()==0)
			std::cout << "CL_FUNCT_TRC is empty" << std::endl;
		return osg::Vec3d(0.,0.,0.);
	}

	//get the IRtransformation
	osg::Matrix IRTransform(CalculateIRTransform());
	//get all the records for the parameter set.
	//actually, only the first one to test...???
	CL_ARTracerVal * CurrRecord = NULL;
	for (unsigned int i = 0; i< Records->TimeRecorded.size(); i++)
	{
		CurrRecord = Records->TimeRecorded[i];
		if (!CurrRecord)
		{
			if ( Records->TimeRecorded.size()==0 )
				std::cout << "CL_TRACER_RECORD is empty" << std::endl;
			return osg::Vec3d(0.,0.,0.);
		}

		//get ART matrix and scale it by 0.1
		osg::Matrix ARTransform(CurrRecord->GetTransform());
		ARTransform.set(ARTransform.inverse(ARTransform));
		osg::Matrix ScaleMatt;
		ScaleMatt.makeScale(0.1,0.1,0.1);
		ARTransform.set(ARTransform*ScaleMatt); 
		PrintMatrix	("ARTransform After Scale*0.1", ARTransform);
		//-----------------------------------

		//apply the CamCalibre matrix to the IRMatrix
		PrintMatrix("IRTransform", IRTransform );
		PrintMatrix("ARTransform", ARTransform );
		PrintMatrix("CamCalibre matrix", _CamCalib );

		osg::Matrix Diff(DiffMatrix(_CamCalib, IRTransform));
		PrintMatrix("Diff matrix(_CamCalib, IRTransform)", Diff );
		
		//get the diff between IR and ART
		osg::Matrix Result(DiffMatrix(Diff,ARTransform));
		PrintMatrix("Final transform dif((Diff,ARTransform))", Result );

		CurrRecord->m_RMSTrans  = Result.getTrans();
		PrintVector3D("Result translate vector", CurrRecord->m_RMSTrans);

		CurrRecord->RMS_Error = CurrRecord->m_RMSTrans.length();

		std::cout << "RMS error = "<< CurrRecord->RMS_Error << std::endl;
	}
	return CurrRecord->m_RMSTrans;
}

/*
osg::Group* CL_FrameRecord::CreateViewerObjs()
{
	osgText::Text * Title = new osgText::Text();
	osg::Text TextManager->
}

void CL_FrameRecord::ReleaseViewerObjs()
{
}
*/
void CL_FrameRecord::Update(osg::Group* FrameNode)
{//add the text to the frame Node.

	if(!FrameNode)
		return;

	//default values
 	osgText::Font* font = osgText::readFontFile("fonts/arial.ttf");
	osg::Vec4	layoutColor(1.0f,1.0f,0.0f,1.0f);
    float	layoutCharacterSize = 10.0f;    
	float	windowHeight	= 480.0f;
    float	windowWidth		= 640.0f;
    float	margin			= 30.0f;
	float	interline		= 1.5f;

	osg::Geode* geode  = new osg::Geode();
	FrameNode->addChild(geode);

	//start position
	osg::Vec3 textPos	(margin,windowHeight-margin - 100,0.);
	osg::Vec3 OffsetPos	(0,-layoutCharacterSize*interline,0.);

	//show frame file
	geode->addDrawable(CreateText(FileName.c_str(), textPos, layoutColor, layoutCharacterSize, font, osgText::Text::LEFT_TO_RIGHT));
		
	//show other details
		CL_XML_MNGR<CL_PosRecord ,std::string>::iterator	itPos;
		CL_PosRecord	*	iPos;
		CL_XML_MNGR<SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> ,std::string>::iterator		itARLib;
		CL_XML_MNGR<CL_TRACER_RECORD<CL_ARTracerVal> , std::string>::iterator					itARRecord;
		SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal>		*ARLib;
		std::string TempStr;

		for (itPos = PosMngr.GetFirstIter(); itPos != PosMngr.GetLastIter(); itPos++)
		{
			//show PosRecord Name
			textPos += OffsetPos;
			iPos = (*itPos).second;
			geode->addDrawable(CreateText(iPos->GetIDStr().c_str(), textPos, layoutColor, layoutCharacterSize, font, osgText::Text::LEFT_TO_RIGHT));
			
			for (itARLib = iPos->GetFirstIter(); itARLib != iPos->GetLastIter(); itARLib++)
			{//show lib record
				textPos += OffsetPos;
				ARLib = (*itARLib).second;
				ARLib->Process();
				TempStr = ARLib->GetIDStr();
				geode->addDrawable(CreateText(TempStr.c_str(), textPos, layoutColor, layoutCharacterSize, font, osgText::Text::LEFT_TO_RIGHT));
				
				for (itARRecord = ARLib->TracerRecorded->GetFirstIter(); itARRecord != ARLib->TracerRecorded->GetLastIter(); itARRecord++)
				{
					std::string Temp2;
					if ((*itARRecord).second->TimeRecorded[0])
					{
						textPos += OffsetPos;
						TempStr = (*itARRecord).second->GetIDStr();
						TempStr += ", Conf=";
						TempStr += ToCharStr((*itARRecord).second->TimeRecorded[0]->Coef, Temp2);
						geode->addDrawable( CreateText(TempStr.c_str(), textPos, layoutColor, layoutCharacterSize, font, osgText::Text::LEFT_TO_RIGHT));
						textPos += OffsetPos;
						TempStr = "Pos=(";
						TempStr += " | x:";
						TempStr += ToCharStr((*itARRecord).second->TimeRecorded[0]->m_storedPosition[0], Temp2);
						TempStr += " | y:";
						TempStr += ToCharStr((*itARRecord).second->TimeRecorded[0]->m_storedPosition[1], Temp2);
						TempStr += " | z:";
						TempStr += ToCharStr((*itARRecord).second->TimeRecorded[0]->m_storedPosition[2], Temp2);
						TempStr += ")";
						geode->addDrawable( CreateText(TempStr.c_str(), textPos, layoutColor, layoutCharacterSize, font, osgText::Text::LEFT_TO_RIGHT));

						textPos += OffsetPos;
						TempStr = "RMS=(l:";
						TempStr += ToCharStr((*itARRecord).second->TimeRecorded[0]->RMS_Error, Temp2);
						if ((*itARRecord).second->TimeRecorded[0]->RMS_Error)
						{
							TempStr += " | x:";
							TempStr += ToCharStr((*itARRecord).second->TimeRecorded[0]->m_RMSTrans[0], Temp2);
							TempStr += " | y:";
							TempStr += ToCharStr((*itARRecord).second->TimeRecorded[0]->m_RMSTrans[1], Temp2);
							TempStr += " | z:";
							TempStr += ToCharStr((*itARRecord).second->TimeRecorded[0]->m_RMSTrans[2], Temp2);
						}
						TempStr += ")";

						geode->addDrawable( CreateText(TempStr.c_str(), textPos, layoutColor, layoutCharacterSize, font, osgText::Text::LEFT_TO_RIGHT));
					}
				}
			}
		}


/*		textPos += OffsetPos;
		CurrText = new osgText::Text();
		CurrText->setFont(font);
		CurrText->setColor(layoutColor);
		CurrText->setCharacterSize(layoutCharacterSize);
		CurrText->setLayout(osgText::Text::LEFT_TO_RIGHT);
		CurrText->setText("Test2");//FileName
		CurrText->setPosition(textPos);
		geode->addDrawable(CurrText);
*///create a title	
}
};//namespace osgART {

#endif //AR_TRACKER_PROFILE