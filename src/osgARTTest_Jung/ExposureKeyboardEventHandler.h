
#ifndef KeyboardEventHandler_H
#define KeyboardEventHandler_H


#include "Exposure.h"

class ExposureKeyboardEventHandler : public osgGA::GUIEventHandler
{
public:
    
        ExposureKeyboardEventHandler() {}
		
		void setExposure( Exposure *_ex )
		{
			ex = _ex;
		};

        virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
        {
            switch(ea.getEventType())
            {
                case(osgGA::GUIEventAdapter::KEYDOWN):
                {
                    switch (ea.getKey())
                    {
                        case '-':
                            ex->decIndex();
                            break;
                        case '=':
                            ex->incIndex();
                            break;
                        case '9':
                            ex->decRange();
                            break;
                        case '0':
                            ex->incRange();
                            break;
                        case '7':
                            ex->decDelta();
                            break;
                        case '8':
                            ex->incDelta();
                            break;
                        default:
                            return false;
                    }

                    return true;
                }

                default:
                    return false;
            }
        }

        virtual void accept(osgGA::GUIEventHandlerVisitor& v)
        {
            v.visit(*this);
        }
private:
	Exposure *ex;
};

#endif