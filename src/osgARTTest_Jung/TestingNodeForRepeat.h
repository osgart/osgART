#ifndef TestingNodeForeRepeat
#define TestingNodeForeRepeat


#include <osg/Node>
#include <osg/Group>

#include <osg/GL>
#include <osg/Viewport>
#include <osg/Texture>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/MatrixTransform>
#include <osg/FrameBufferObject>
#include <osg/Node>
#include <osg/CameraNode>


class TestingNodeForRepeat : public osg::Group
{
public:
	TestingNodeForRepeat()
	{
		numCalled = 0;		
	} ;
	virtual ~TestingNodeForRepeat(){};
		
	void calledByCallBack()
	{
		numCalled++;
		 
		std::cout << "I'm called " << numCalled << std::endl;
	};

private:
	int numCalled;
};

class TestingNodeForRepeatCallback : public osg::NodeCallback
{
public:
	TestingNodeForRepeatCallback(){};
	virtual ~TestingNodeForRepeatCallback(){};
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv)
	{
		TestingNodeForRepeat *mbm = dynamic_cast<TestingNodeForRepeat*>( nd ); 
		mbm->calledByCallBack();
	};
};


#endif