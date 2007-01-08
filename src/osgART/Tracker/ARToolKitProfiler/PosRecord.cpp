#include "PosRecord"

#include <iomanip>
#include <osg\notify>

namespace osgART {
	
CL_PosRecord::CL_PosRecord(std::string _tagname)
	:CL_XML_BASE_OBJ<std::string>(_tagname),
	ARRecordsMngr(NULL)
{}

CL_PosRecord::~CL_PosRecord()
{}


void CL_PosRecord::Print(std::string _Header)
{
	cout << _Header << endl;
	PrintVector3D("VectorPos", m_storedPosition);
	PrintVector3D("VectorRot", m_storedRotation);
}

void CL_PosRecord::SetTransform(const osg::Vec3d  &Trans, const osg::Vec3d  &Rot)
{
	m_storedPosition = Trans;
	m_storedRotation = Rot;
}


void CL_PosRecord::SetTransform(const osg::Matrix &Transform)
{
	std::cout << "Record for " << GetID() << std::endl;
	
	// Get the matrix from the tracker
	m_storedPosition = Transform.getTrans();
	m_storedRotation = Transform.getRotate().asVec3();
}

osg::Matrix CL_PosRecord::GetTransform()
{
	return GenerateMatrix(m_storedPosition, m_storedRotation);
}

SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> * CL_PosRecord::AddARRecord(std::string _LibName)
{
	if (!ARRecordsMngr)
		ARRecordsMngr = new CL_XML_MNGR<SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> ,std::string>(NULL, "ARRcrdMngr", "ARRcrd", XML_TAG_VAL_ID);

	SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> *NewFct = ARRecordsMngr->Add(_LibName);
	NewFct->SetMaxRecords(VIDEO_BENCH_MAX_LIB_RECORDS);
	NewFct->UseMinMaxTot = false;
	return NewFct;
}

SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> * CL_PosRecord::GetARRecord(std::string _LibName)
{
	if (!ARRecordsMngr)
		return NULL;
	return ARRecordsMngr->Get(_LibName);
}


CL_XML_MNGR<SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> ,std::string>::iterator 
	CL_PosRecord::GetFirstIter()
{
	if (ARRecordsMngr)
		return ARRecordsMngr->GetFirstIter();
	return NULL;
}

CL_XML_MNGR<SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> ,std::string>::iterator 
	CL_PosRecord::GetLastIter()
{
	if (ARRecordsMngr)
		return ARRecordsMngr->GetLastIter();
	return NULL;
}

TiXmlElement* 
CL_PosRecord::XMLLoad(TiXmlElement* _XML_ROOT)
{
	TiXmlElement* XMLElem = CL_XML_BASE_OBJ<std::string>::XMLLoad(_XML_ROOT);
	XMLLoadVec3		(XMLElem, "Rot", m_storedRotation);
	XMLLoadVec3		(XMLElem, "Pos", m_storedPosition);

//	Print		("After XMLLoad");
//	PrintMatrix ("Matrix After XMLLoad", GetTransform());

	if (!ARRecordsMngr)
		ARRecordsMngr = new CL_XML_MNGR<SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> ,std::string>(NULL, "ARRcrdMngr", "ARRcrd", XML_TAG_VAL_ID);

	ARRecordsMngr->XMLLoad(XMLElem);
	
	CL_XML_MNGR<SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> ,std::string>::iterator itRecord;
	for (itRecord = ARRecordsMngr->GetFirstIter();  itRecord != ARRecordsMngr->GetLastIter(); itRecord++)
		(*itRecord).second->UseMinMaxTot =false;

	return XMLElem;
}

TiXmlElement* 
CL_PosRecord::XMLSave(TiXmlElement* _XML_ROOT)
{
	TiXmlElement* XMLElem = CL_XML_BASE_OBJ<std::string>::XMLSave(_XML_ROOT);
	XMLSaveVec3		(XMLElem, "Rot", m_storedRotation);
	XMLSaveVec3		(XMLElem, "Pos", m_storedPosition);

	if (ARRecordsMngr)
	{
		CL_XML_MNGR<SG_TRC::CL_FUNCT_TRC<osgART::CL_ARTracerVal> ,std::string>::iterator itRecord;
		for (itRecord = ARRecordsMngr->GetFirstIter();  itRecord != ARRecordsMngr->GetLastIter(); itRecord++)
			(*itRecord).second->UseMinMaxTot =false;

		ARRecordsMngr->XMLSave(XMLElem);
	}
	return XMLElem;
}

};//namespace osgART