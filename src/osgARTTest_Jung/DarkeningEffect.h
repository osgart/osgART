#ifndef DarkeningEffect_h
#define DarkeningEffect_h

#include <osg/Node>
#include <osg/Timer>
#include <string>
#include <iostream>
using namespace std;

#include "ShaderEffect.h"

class DarkeningEffect : public ShaderEffect
{
public:
	DarkeningEffect();
	virtual ~DarkeningEffect();

	virtual void init(float _timeInMillisec);
	virtual void whenOn(float t);
	virtual void whenOff(float t);

	virtual void linkTo( osg::Node *);
private:
	osg::ref_ptr<osg::Uniform> weightControl;
};

#endif