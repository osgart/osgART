#include "ARToolKitTrackerProfiler"
#include <AR/config.h>
#include <AR/video.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>
#include <iostream>
#include <fstream>

namespace osgART {

#if AR_TRACKER_PROFILE
	TTCL_APPLI_TRACER<SG_TRC_Default_Trc_Type> 	*ARToolKitTrackerProfiler::MainARTimeTracer			= NULL;
	TTCL_APPLI_TRACER<CL_ARTracerVal> 			*ARToolKitTrackerProfiler::MainARConfidentTracer	= NULL;
	int ARToolKitTrackerProfiler::NbrOfPluginProfiled = 0;

	CLoggerDebug * ARToolKitTrackerProfiler::Logger = NULL;
#endif


	ARToolKitTrackerProfiler::ARToolKitTrackerProfiler(std::string Name, std::string Version):
		GenericTracker(),
		m_profilerDebug(false),
		m_color(osg::Vec4(1.,1.,1.,1.))
#if AR_TRACKER_PROFILE
		,CurrentFrame(NULL)
		
#endif
	{
		m_name		= Name;
		m_version	= Version;

#if AR_TRACKER_PROFILE
		osg::notify() << "ARToolKitTrackerProfiler()" << endl;
		if(!Logger)
		{
			Logger = new CLoggerDebug();
			Logger->SetLogger(Logger);
		}

		if (!ARToolKitTrackerProfiler::MainARTimeTracer)
		{
			ARToolKitTrackerProfiler::MainARTimeTracer		=  CreateMainTracer<SG_TRC_Default_Trc_Type>();//new TTCL_APPLI_TRACER<SG_TRC_Default_Trc_Type>();
			ARToolKitTrackerProfiler::MainARTimeTracer->XMLLoadFromFile("TimeBench.xml");
		}
			
		if (!ARToolKitTrackerProfiler::MainARConfidentTracer)
		{
			ARToolKitTrackerProfiler::MainARConfidentTracer	=  CreateMainTracer<CL_ARTracerVal>();// new TTCL_APPLI_TRACER<CL_ARTracerVal>();
			ARToolKitTrackerProfiler::MainARConfidentTracer->XMLLoadFromFile("ConfBench.xml");
		}

		//affect local one :
		NbrOfPluginProfiled ++;
		LocalARTimeTracer		= MainARTimeTracer;
		LocalARConfidentTracer	= MainARConfidentTracer;
		//===================
#endif
	}

	ARToolKitTrackerProfiler::~ARToolKitTrackerProfiler()
	{
#if AR_TRACKER_PROFILE
		NbrOfPluginProfiled --;

		if (NbrOfPluginProfiled <=0)
		{//we save data and delete the profiler

			osg::notify() << "Saving XML files\n" << endl;

			LocalARTimeTracer->XMLSaveToFile("TimeBench.xml");
			LocalARConfidentTracer->XMLSaveToFile("ConfBench.xml");
			//ARConfidentTracer.XMLSaveToFile("ConfidentBench.xml");

			if (ARToolKitTrackerProfiler::MainARTimeTracer)
				delete ARToolKitTrackerProfiler::MainARTimeTracer;
			if (ARToolKitTrackerProfiler::MainARConfidentTracer)
				delete ARToolKitTrackerProfiler::MainARConfidentTracer;
		}
#endif
	}

#if AR_TRACKER_PROFILE
	void ARToolKitTrackerProfiler::RecordMarkerStats(int & _PattID, double &_Coef, osg::Matrix _transform, bool _visible)
	{
		if (m_profilerDebug)
			PrintMatrix("RecordMarkerStats marker Matrix", _transform);

		int Coef = _Coef*100;//_Marker-> _marker->cf * 100;
		
		float dist = _transform.getTrans().length();
		int dist2 = dist /100; /*we are in mm*/
		dist2 = dist2* 10;/*to remove the last number...we are now in cm with a 10cm range*/

#if 0
		//get static pointer to function
		static CL_FUNCT_TRC<CL_ARTracerVal>::TplFctHdle ThisFctConfident	= 
			this->LocalARConfidentTracer->AddFunct	("AR_BENCH_CONFIDANT", 
			CL_FUNCT_TRC<CL_ARTracerVal>::TplFctObj(new CL_FUNCT_TRC<CL_ARTracerVal>("AR_BENCH_CONFIDANT")));

		//get a new parameter with the artoolkit version
		CL_TRACE_BASE_PARAMS * Params = new CL_TRACE_BASE_PARAMS("V", m_versionName);/*create params using version name*/

		//get a tracer
		CL_TRACER<CL_ARTracerVal> *ConfidentTracer = new CL_TRACER<CL_ARTracerVal>(ThisFctConfident, Params);/*do not delete this*/

		ThisFctConfident->AddTracerToProccess(ConfidentTracer);
		
		//PrintVersion();

		//calculate values to trace
		
		Params->AddInt("ID", &_PattID, 1,2);
		Params->AddInt("D", &dist2, 1,4);
		//Params->AddInt("C", &Coef, 3);

		CL_ARTracerVal	*Val = ConfidentTracer->GetResult();
		Val->Distance	= dist2;
		Val->Coef		= Coef;
		Val->PattID		= _PattID;
		ConfidentTracer->SetReady(true);

		osg::notify() << "Dist : " << dist2 << " | Coef :" << Coef << " | ID : " << _PattID << endl;
#endif	
		//=======================================
		//Add bench records into the main bench file
		//=======================================
		CL_ARTracerVal	*Val2 = NULL; //remove Val when the new benchmarks system is ready
		
		if (CurrentFrame)
		{
			std::string pattName = "patt_" + ToCharStr(_PattID, std::string()); 
			CL_PosRecord *PatternRecord = CurrentFrame->PosMngr.Get(pattName);
			if (!PatternRecord)
			{
				osg::notify(osg::WARN) << "ARToolKitTrackerProfiler::RecordMarkerStats() : PatternRecord is empty." << endl;
				return;
			}

			//we got a patternrecord from the prebench records.
			//we add this AR library patternposition...
			//the ARTLib record is equivalent to a function
			CL_FUNCT_TRC<CL_ARTracerVal>	*LibRecord = PatternRecord->AddARRecord(m_versionName);
			if (!LibRecord)
			{
				osg::notify(osg::WARN) << "ARToolKitTrackerProfiler::RecordMarkerStats() : LibRecord is empty." << endl;
				return;
			}
			//get a new parameter with the artoolkit version
			CL_TRACE_BASE_PARAMS * Params = new CL_TRACE_BASE_PARAMS("Optim", std::string("0"));/*Create new parameters depending on the lib settings and optimisations*/
			if (!Params)
			{
				osg::notify(osg::WARN) << "ARToolKitTrackerProfiler::RecordMarkerStats() : Params is empty." << endl;
				return;
			}

			//create a tracer to store the record
			CL_TRACER<CL_ARTracerVal> *ARPattTracer = new CL_TRACER<CL_ARTracerVal>(LibRecord, Params);/*do not delete this*/
			if (!ARPattTracer)
			{
				osg::notify(osg::WARN) << "ARToolKitTrackerProfiler::RecordMarkerStats() : ARPattTracer is empty." << endl;
				return;
			}

			//get the object to store the actual patt record
			CL_ARTracerVal	* pattRecord =  ARPattTracer->GetResult();
			if (!pattRecord)
			{
				osg::notify(osg::WARN) << "ARToolKitTrackerProfiler::RecordMarkerStats() : pattRecord is empty." << endl;
				return;
			}

			ARPattTracer->SetReady(true);
			
			if (_visible)
			{
				pattRecord->SetTransform(_transform);
				pattRecord->Distance	= dist2;
				pattRecord->Coef		= Coef;
				pattRecord->PattID		= _PattID;
			}
			else
			{
				pattRecord->Distance	= 0;
				pattRecord->Coef		= -1;
				pattRecord->PattID		= -1;				
			}
			LibRecord->UseMinMaxTot = false;
			LibRecord->AddTracerToProccess(ARPattTracer);

			//..?? other value to record..?? like time spent in ardetectemarker???
			//that 's all, record saved...
		}
		//=======================================
		//=======================================
	}

	void	ARToolKitTrackerProfiler::setCurrentBenchFrame(osgART::CL_FrameRecord * _CurrentFrame)
	{
		CurrentFrame = _CurrentFrame;
	}

#endif
	void ARToolKitTrackerProfiler::PrintVersion()const
	{
		cout << "Version : " << m_versionName <<  endl;
	}

	void ARToolKitTrackerProfiler::PrintOptions()const
	{
		cout << "Version : " << m_versionName <<  endl;
	}

	osg::Vec4 ARToolKitTrackerProfiler::getColor()const
	{	return m_color;	}

	void ARToolKitTrackerProfiler:: setColor(const osg::Vec4 _color)
	{
		m_color = _color;
	}
/*
	const std::string & ARToolKitTrackerProfiler::getVersion() const
	{
		return m_versionName;
	}
	*/
}; // namespace osgART