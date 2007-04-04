#ifndef SimpleAnimatorCallback_h
#define SimpleAnimatorCallback_h

#include <osg/Node>
#include <osg/Timer>
#include <string>
#include <iostream>
using namespace std;


class SimpleAnimatorCallback : public osg::NodeCallback
{
public:
	SimpleAnimatorCallback();
	virtual ~SimpleAnimatorCallback();

	virtual void init(float _timeInMillisec, bool _loop);

	virtual void update(float t)=0;
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);
	
	bool isAnimationOn()
	{
		return animationOn;
	};

	virtual bool setAnimationOn(bool b)
	{
		animationOn = b;
		lastTime = effectTimer->tick();
		return animationOn;
	};
	virtual void linkTo( osg::Node *);

protected:
		
	bool animationOn;
	bool loop;
	
	osg::Timer* effectTimer;
	osg::Timer_t lastTime;

	float total; 
	float timeInMillisec;
};

class RipplingValueAnimationCallback : public SimpleAnimatorCallback
{
public:
	RipplingValueAnimationCallback(){};
	virtual ~RipplingValueAnimationCallback(){};

	virtual void init(float _timeInMillisec, bool _loop);
	virtual void update(float t);

	virtual void linkTo( osg::Node *);
protected:
	
	osg::ref_ptr<osg::Uniform> timeStamp;
	
};

class ZoomingValueAnimationCallback : public SimpleAnimatorCallback
{
public:
	ZoomingValueAnimationCallback(){};
	virtual ~ZoomingValueAnimationCallback(){};

	virtual void init(float _timeInMillisec, bool _loop);
	virtual void update(float t);

	virtual void linkTo( osg::Node *); 
protected:
	
	osg::ref_ptr<osg::Uniform> zoomScale;
	osg::ref_ptr<osg::Uniform> textureIndex;
	int counter;
};

#endif