#ifndef ShaderEffect_h
#define ShaderEffect_h

#include <osg/Node>
#include <osg/Timer>
#include <string>
#include <iostream>
using namespace std;

class ShaderEffect : public osg::NodeCallback
{
public:
	ShaderEffect();
	virtual ~ShaderEffect();

	virtual void init(float _timeInMillisec);

	bool turnOn(bool);
	bool isAnimationOn()
	{
		return animationOn;
	};
	
	void turnOnOff()
	{		
		turnOn( !isOn );
	};
	virtual void whenOn(float t)=0;
	virtual void whenOff(float t)=0;
	
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);
	virtual void linkTo( osg::Node *);

protected:
   	void update();

	// with no debug and returns shader program
	bool loadShaderSource(osg::Shader* obj, const string fileName );
	osg::Program* createVertexAndFragmentShaderFromFile(string vertexFileName, 
											string fragmentFileName);
	osg::Program* createFragmentShaderFromFile(string fragmentFileName);

	
	bool animationOn;
	bool isOn;

	osg::ref_ptr<osg::Program> shaderProgram;
	osg::Timer* effectTimer;
	float total; // time

	float timeInMillisec;
	osg::Timer_t lastTime;
};




#endif