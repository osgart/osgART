#ifndef ColorTemperatureEffect_h
#define ColorTemperatureEffect_h

#include <osg/Node>
#include <osg/Timer>
#include <string>
#include <iostream>
using namespace std;

#include "ShaderEffect.h"

class ColorTemperatureEffect : public ShaderEffect
{

public:
	ColorTemperatureEffect();
	virtual ~ColorTemperatureEffect();

	virtual void init(float _timeInMillisec);
	virtual void whenOn(float t);
	virtual void whenOff(float t);

	virtual void linkTo( osg::Node *);

	void setStEdTemp( float _stTemp, float _edTemp )
	{
		stTemp = _stTemp;
		edTemp = _edTemp;
	};	
protected:

	float stTemp, edTemp;
	osg::ref_ptr<osg::Uniform> colorTemp;

};


#endif