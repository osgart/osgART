#include "SimpleAnimatorCallback.h"


SimpleAnimatorCallback::SimpleAnimatorCallback()
{
	animationOn = false;
	total = 0;
}
SimpleAnimatorCallback::~SimpleAnimatorCallback()
{
	delete effectTimer;
}

void SimpleAnimatorCallback::init(float _timeInMillisec, bool _loop)
{
	timeInMillisec = _timeInMillisec;
	loop = _loop;
	effectTimer = new osg::Timer(); 

	double sss = effectTimer->getSecondsPerTick ();

	//timeInMillisec = timeInMillisec * sss;
	//std::cout << timeInMillisec << std::endl;
}


void SimpleAnimatorCallback::operator()(osg::Node *nd, osg::NodeVisitor* nv)
{
	
	if ( animationOn )
	{
		osg::Timer_t curTime = effectTimer->tick();
		float timePassed = effectTimer->delta_m(lastTime, curTime);
		float t;
				
		total = total + timePassed ;
		t = total / timeInMillisec;

		update(t);
		
		if ( t >= 1.0 )
		{
			if ( !loop )
			{
				animationOn = false;
			}
			total = 0;
		}
		lastTime = curTime;
	}

	traverse(nd,nv);
}

void SimpleAnimatorCallback::linkTo( osg::Node *nd)
{
	osg::StateSet* nodeState = nd->getOrCreateStateSet();	
	nd->setUpdateCallback( this );
}
//////

void RipplingValueAnimationCallback::init(float _timeInMillisec, bool _loop)
{
	SimpleAnimatorCallback::init(_timeInMillisec, _loop);
	timeStamp = new osg::Uniform("timeStamp", 0.0f);
}


void RipplingValueAnimationCallback::update(float t)
{
	timeStamp->set(t*3.14f * 2.0f);
}

void RipplingValueAnimationCallback::linkTo( osg::Node *nd)
{
	SimpleAnimatorCallback::linkTo( nd );
	nd->getOrCreateStateSet()->addUniform( timeStamp.get() );
}

/////
void ZoomingValueAnimationCallback::init(float _timeInMillisec, bool _loop)
{
	SimpleAnimatorCallback::init(_timeInMillisec, _loop);
	zoomScale = new osg::Uniform("zoomScale", 0.0f);
	textureIndex = new osg::Uniform("zoomScale",  0 );

	counter = 0;
}


void ZoomingValueAnimationCallback::update(float t)
{
	//zoomScale->set( -0.5f * t + 1.0f);
	//zoomScale->set( (1.0f - t) * 0.5f );
	zoomScale->set( t );
	
	textureIndex->set( counter );
	
	//std::cout << counter << std::endl;

	if ( t >= 1.0 )
	{
		counter++;
		if ( counter > 3 )
		{
			counter = 0;
		}
	}
}

void ZoomingValueAnimationCallback::linkTo( osg::Node *nd)
{
	SimpleAnimatorCallback::linkTo( nd );
	nd->getOrCreateStateSet()->addUniform( zoomScale.get() );
}