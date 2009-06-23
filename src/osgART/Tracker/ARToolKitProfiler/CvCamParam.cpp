#include "CvCamParam"


#if AR_TRACKER_PROFILE
#include "PosRecord"
#include <iomanip>

#include <iostream>
#include <fstream>
#include "ar\param.h"//used to get the ARParam struct
#include "ar\gsub_lite.h"//used to get the arglCameraFrustumRH


namespace osgART {

CL_OpenCVCamParam::CL_OpenCVCamParam(std::string _FileName)
	:CL_XML_BASE_OBJ<std::string>(_FileName, "OpenCVCam"),
	PosMngr(NULL, "PosMngr", "PosObj", XML_TAG_VAL_ID)	
{}

CL_OpenCVCamParam::~CL_OpenCVCamParam()
{}

double* CL_OpenCVCamParam::GetProjectionMatrix(float nearClip, float farClip)
{
	//we use the function arglCameraFrustumRH to get the camera calibration transform
	//
	ARParam arCalibParam;

	//osg::Matrix3	m_CVMat;
	//osg::Vec4d		m_CVDistorsion;

	//Distortion
	//ARToolkit 2.71
		//convert osg::Vec4d to dist_factor[4]
		arCalibParam.dist_factor[0] = m_CVDistorsion[0];
		arCalibParam.dist_factor[1] = m_CVDistorsion[1];
		arCalibParam.dist_factor[2] = m_CVDistorsion[2];
		arCalibParam.dist_factor[3] = m_CVDistorsion[3];

	//ARToolkit 4
		//convert osg::Vec4d to dist_factor[6]?? what to do...
	//========================================

	//convert from mat3x3 to mat[3][4], both 2.7 and 4.0
	arCalibParam.mat[0][0] = m_CVMat(0,0);
	arCalibParam.mat[0][1] = m_CVMat(0,1);
	arCalibParam.mat[0][2] = m_CVMat(0,2);
	arCalibParam.mat[0][3] = 0;
	arCalibParam.mat[1][0]= m_CVMat(1,0);
	arCalibParam.mat[1][1] = m_CVMat(1,1);
	arCalibParam.mat[1][2] = m_CVMat(1,1);
	arCalibParam.mat[1][3] = 0;
	arCalibParam.mat[2][0] = m_CVMat(2,0);
	arCalibParam.mat[2][1] = m_CVMat(2,1);
	arCalibParam.mat[2][2]= m_CVMat(2,2);
	arCalibParam.mat[2][3]= 1;
	arCalibParam.xsize = 640;
	arCalibParam.xsize = 480;
	//double ProjectionMatrix[16];
	arglCameraFrustumRH((ARParam*)&arCalibParam, nearClip, farClip, m_projectionMatrix);

	std::cout << "Base Matrix" << std::endl;
	for (int i=0; i < 3; i++)
	{
		std::cout << arCalibParam.mat[i][0] 
		<< "	" << arCalibParam.mat[i][1] 
		<< "	" << arCalibParam.mat[i][2] 
		<< "	" << arCalibParam.mat[i][3]		
		<< std::endl;
	}

	PrintMatrix("OpenCVProjectionMatrix Before...", osg::Matrix(m_projectionMatrix));
	m_projectionMatrix[0*4+ 0] = 2.19;
	m_projectionMatrix[1*4+ 1] = 3.0;
	m_projectionMatrix[2*4+ 0] = 0.09;
	m_projectionMatrix[2*4+ 1] = 0.08;

	m_projectionMatrix[3*4+ 0] = 0;
	m_projectionMatrix[3*4+ 1] = 0;
	m_projectionMatrix[3*4+ 3] = 0;
	PrintMatrix("OpenCVProjectionMatrix After...", osg::Matrix(m_projectionMatrix));

	return m_projectionMatrix;
}


osg::Matrix CL_OpenCVCamParam::GetIRTransform()
{
	CL_PosRecord * IRCam    = PosMngr.Get("IR_CAM");
	CL_PosRecord * IRCVPatt = PosMngr.Get("IR_CV_PATT");

	SG_Assert(IRCam,	"IRCam empty");
	SG_Assert(IRCVPatt,	"IRCVPatt empty");


	osg::Matrix IRCamMatt		= IRCam->GetTransform();	//in m
	osg::Matrix IRCVPattMatt	= IRCVPatt->GetTransform();	//in m

	SG_NOTICE_LOG("Difference between IRCVPattMatt and IRCamMatt");
	osg::Matrix IRDiff(DiffMatrix(IRCVPattMatt, IRCamMatt));
	PrintMatrix	("IRDiff", IRDiff);

	osg::Matrix ScaleMatt;
	ScaleMatt.makeScale(100,100,100);
	IRDiff.set(IRDiff * ScaleMatt);
	PrintMatrix	("IRDiff after scale", IRDiff);

	return IRDiff;
}

osg::Matrix CL_OpenCVCamParam::GetCVTransform()
{
	CL_PosRecord * CVCam	= PosMngr.Get("CV_CAM");
	SG_Assert(CVCam,	"CVCam empty");

	osg::Matrix CVCamMatt	= CVCam->GetTransform();	//in cm
	PrintMatrix	("CVCamMatt", CVCamMatt);
	
	return CVCamMatt;
}


osg::Matrix CL_OpenCVCamParam::GetVirtualCameraTransform()
{
/*	CL_PosRecord * CVCam	= PosMngr.Get("CV_CAM");
	CL_PosRecord * IRCam    = PosMngr.Get("IR_CAM");
	CL_PosRecord * IRCVPatt = PosMngr.Get("IR_CV_PATT");
	
	SG_Assert(CVCam,	"CVCam empty");
	SG_Assert(IRCam,	"IRCam empty");
	SG_Assert(IRCVPatt,	"IRCVPatt empty");
*/

	osg::Matrix CVTransform		= GetCVTransform();	//in cm
	osg::Matrix IRTransform		= GetIRTransform();	//in m

/*	PrintMatrix	("CVCamMatt", CVCamMatt);
	PrintMatrix	("IRCVPattMatt", IRCVPattMatt);
	PrintMatrix	("IRCamMatt", IRCamMatt);

	osg::Matrix ScaleMatt;
	ScaleMatt.makeScale(100,100,100);
	
	SG_NOTICE_LOG("Difference between IRCVPattMatt and IRCamMatt");
	osg::Matrix IRDiff(DiffMatrix(IRCVPattMatt, IRCamMatt));
	PrintMatrix	("IRDiff", IRDiff);

	IRDiff.set(IRDiff * ScaleMatt);
	PrintMatrix	("IRDiff after scale", IRDiff);
*/
	SG_NOTICE_LOG("Difference between IRTransform and CVTransform");
	osg::Matrix CamDiff(DiffMatrix(IRTransform, CVTransform));
	PrintMatrix	("CamDiff", CamDiff);
	
	osg::Matrix ScaleMatt;
	ScaleMatt.makeScale(100,100,100);
	osg::Matrix Result(CamDiff*ScaleMatt); 
	PrintMatrix	("CamDiff After Scale*100", Result);
	
	return Result;
}

/*\warning The cvFilterCalib filter from opencv has been modified to obtain the correct outuput file. Be sure to use the right version???
*/
bool CL_OpenCVCamParam::ReadParamFile(std::string _FileName)
{
	std::ifstream ParamFile;

	// Need to check whether the passed file even exists
	ParamFile.open(_FileName.c_str());

	// Need to check for error when opening file
	if (!ParamFile.is_open()) return false;

	bool ret = true;
	char sectionNbr = 0;
	std::string Section = "";
	std::string SectionPart = "";
	double		MatrixValue;
	std::string MatrixName;

	osg::Vec3d CamRot, CamTrans;

	while(sectionNbr <4)
	{
		ParamFile >> Section;
		std::cout << "Section is : " << Section << std::endl;
		SectionPart = "";
		MatrixValue = 0;
		MatrixName = "";
		//read CameraMatrix
		if (Section == "CameraMatrix:")
		{
			float Mat[3*3];
			for( int i = 0; i < 3*3 ; i++)
			{
				/*while (trim(SectionPart) == "" && !ParamFile.eof()) {
					getline(ParamFile, SectionPart);
				}
*/
				//we have a matrix line, get the matrix value :
				ParamFile >> MatrixName >> Mat[i];
				std::cout << "Reading : " << MatrixName << " = " << Mat[i] <<endl;
			}
			m_CVMat.set(Mat);
			sectionNbr++;
		}
	
		//read Distortion Vector
		else if (Section == "Distortion:")
		{
			double Mat4[4];
			for( int i = 0; i < 4 ; i++)
			{
/*				while (trim(SectionPart) == "" && !ParamFile.eof()) {
					getline(ParamFile, SectionPart);
				}
*/
				//we have a matrix line, get the matrix value :
				ParamFile >> MatrixName >> Mat4[i];
				std::cout << "Reading : " << MatrixName << " = " << Mat4[i]<<endl;
			}
			m_CVDistorsion.set(Mat4[0], Mat4[1], Mat4[2], Mat4[3]);
			sectionNbr++;
		}
		//read translation vector
		else if (Section == "CameraTransVect:")
		{
			double Mat3[3];
			for( int i = 0; i < 3 ; i++)
			{
/*				while (trim(SectionPart) == "" && !ParamFile.eof()) {
					getline(ParamFile, SectionPart);
				}
*/
				//we have a matrix line, get the matrix value :
				ParamFile >> MatrixName >> Mat3[i];
				std::cout << "Reading : " << MatrixName << " = " << Mat3[i]<<endl;
			}
			CamTrans.set(Mat3[0], Mat3[1], Mat3[2]);
			sectionNbr++;
		}

		//read rotation vector
		else if (Section == "CameraRotVect:")
		{
			double Mat3[3];
			for( int i = 0; i < 3 ; i++)
			{
/*				while (trim(SectionPart) == "" && !ParamFile.eof()) {
					getline(ParamFile, SectionPart);
				}
*/
				//we have a matrix line, get the matrix value :
				ParamFile >> MatrixName >> Mat3[i];
				std::cout << "Reading : " << MatrixName << " = " << Mat3[i]<<endl;
			}
			CamRot.set(Mat3[0], Mat3[1], Mat3[2]);
			sectionNbr++;
		}
		else
		{
			std::cout << "No Section available" << std::endl;
		}
	}

	CL_PosRecord * CVCamPos = PosMngr.Add("CV_CAM");
	SG_Assert(CVCamPos, "CVCamPos is empty");
	CVCamPos->SetTransform(CamTrans, CamRot);

	ParamFile.close();

	return ret;
}

TiXmlElement* CL_OpenCVCamParam::XMLLoad(TiXmlElement* _XML_ROOT)
{
	TiXmlElement*  XML_Elm = CL_XML_BASE_OBJ<std::string>::XMLLoad(_XML_ROOT);
	SG_Assert(XML_Elm, "CL_OpenCVCamParam::XMLLoad() XML_Elm is empty");

	//read camera Matrix
	SGE::CLT_Vector<float>	XMLMat (3*3,0.);
	XMLMat.XMLLoad(XML_Elm, "CVCameraMatrix");
	m_CVMat.set(XMLMat.GetPtr(0));

	//read Distortion
	SGE::CL_Vector4Dd	XMLVec4;
	XMLVec4.XMLLoad(XML_Elm, "CVDistorsion");
	m_CVDistorsion.set(XMLVec4[0], XMLVec4[1], XMLVec4[2], XMLVec4[3]);
	
	//Read all the recorded position
	PosMngr.XMLLoad(XML_Elm);

	return XML_Elm;
}

TiXmlElement* CL_OpenCVCamParam::XMLSave(TiXmlElement* _XML_ROOT)
{
	//check why we can't use CL_XML_BASE_OBJ<std::string>::XMLSave(_XML_ROOT); ?????
	TiXmlElement*  XML_Elm = new TiXmlElement(GetMainTag().c_str());
	SG_Assert(XML_Elm, "CL_OpenCVCamParam::XMLSave() XML_Elm is empty");

	//write camera Matrix
	SGE::CLT_Vector<float>	XMLMat (3*3, m_CVMat.ptr());
	XMLMat.XMLSave(XML_Elm, "CVCameraMatrix");

	//write Distortion
	SGE::CLT_Vector<double>	XMLVec4(4, m_CVDistorsion.ptr());
	XMLVec4.XMLSave(XML_Elm, "CVDistorsion");

	//save all the recorded position
	PosMngr.XMLSave(XML_Elm);

	_XML_ROOT->InsertEndChild(*XML_Elm);//Check it ..???

	return XML_Elm;
}

void CL_OpenCVCamParam::PrintAllMatrix()
{
	CL_PosRecord * CVCam	= PosMngr.Get("CV_CAM");
	CL_PosRecord * IRCam    = PosMngr.Get("IR_CAM");
	CL_PosRecord * IRCVPatt = PosMngr.Get("IR_CV_PATT");
	
	SG_Assert(CVCam,	"CVCam empty");
	SG_Assert(IRCam,	"IRCam empty");
	SG_Assert(IRCVPatt,	"IRCVPatt empty");

	osg::Matrix CVCamMatt		= CVCam->GetTransform();	//in cm
	osg::Matrix IRCamMatt		= IRCam->GetTransform();	//in m
	osg::Matrix IRCVPattMatt	= IRCVPatt->GetTransform();	//in m

	std::cout << "All matrix from OpenCVParam=========" << std::endl;
	PrintMatrix	("CVCamMatt", CVCamMatt);
	PrintMatrix	("IRCVPattMatt", IRCVPattMatt);
	PrintMatrix	("IRCamMatt", IRCamMatt);
}

};//namespace osgART
#endif //AR_TRACKER_PROFILE