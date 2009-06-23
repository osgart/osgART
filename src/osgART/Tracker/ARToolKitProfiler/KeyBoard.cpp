#include "Keyboard"
namespace osgART{
             
VideoBenchKeyboardEventHandler::VideoBenchKeyboardEventHandler(CL_VideoBench * _videoBench):
    m_VideoBench(_videoBench) 
{	
	//video bench management
	AddKeyLink(osgGA::GUIEventAdapter::KEY_Left,		osgART::CL_VideoBench::ACT_GET_PREVIOUS_FRAME);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_Page_Down,	osgART::CL_VideoBench::ACT_GET_PREVIOUS_FRAME_GRP);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_Right,		osgART::CL_VideoBench::ACT_GET_NEXT_FRAME);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_Page_Up,		osgART::CL_VideoBench::ACT_GET_NEXT_FRAME_GRP);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_Space,		osgART::CL_VideoBench::ACT_PAUSE);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_Home,		osgART::CL_VideoBench::ACT_GET_FIRST_FRAME);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_F5,			osgART::CL_VideoBench::ACT_REFRESH_FRAME);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_F1,			osgART::CL_VideoBench::ACT_SET_IDLE);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_F2,			osgART::CL_VideoBench::ACT_SET_RECORDING);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_F3,			osgART::CL_VideoBench::ACT_SET_BENCHMARKING);

	//tracker tweaking
	AddKeyLink(osgGA::GUIEventAdapter::KEY_KP_Subtract,	osgART::CL_VideoBench::ACT_THRESHOLD_DOWN);
	AddKeyLink(osgGA::GUIEventAdapter::KEY_KP_Add,		osgART::CL_VideoBench::ACT_THRESHOLD_UP);
	AddKeyLink('t',										osgART::CL_VideoBench::ACT_THRESHOLD_AUTO_SWITCH);
	AddKeyLink('p',										osgART::CL_VideoBench::ACT_POSE_ESTIM_SWITCH);
	AddKeyLink('i',										osgART::CL_VideoBench::ACT_IMG_PROC_MODE_SWITCH);
	AddKeyLink('l',										osgART::CL_VideoBench::ACT_DETECTLITE_SWITCH);
	AddKeyLink('u',										osgART::CL_VideoBench::ACT_UNDISTORT_MODE);
}

void VideoBenchKeyboardEventHandler::AddKeyLink(int _Key, CL_VideoBench::VideoBenchAction _Action)
{	
	m_keyMap[_Key] = _Action;
}

bool VideoBenchKeyboardEventHandler::handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
{
    switch(ea.getEventType())
    {
        case(osgGA::GUIEventAdapter::KEYDOWN):
        {
            return true;
        }
        case(osgGA::GUIEventAdapter::KEYUP):
        {//start action only on Key Up
			KeyMap::iterator itr = m_keyMap.find(ea.getKey());                    
			if (itr!=m_keyMap.end() && 	m_VideoBench)
				m_VideoBench->AddAction((*itr).second);
            return true;
        }

        default:
            return false;
    }
}

void VideoBenchKeyboardEventHandler::accept(osgGA::GUIEventHandlerVisitor& v)
{
    v.visit(*this);
}

};//namespace osgART

//keyboard Management	


