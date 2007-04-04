#ifndef SHADERFACTORY_H
#define SHADERFACTORY_H

#include <osg/Node>
#include <string>
#include <iostream>
using namespace std;
#include <sys/stat.h>

#define ShaderDebugMode

class ShaderFactory
{

public:
	ShaderFactory();
	virtual ~ShaderFactory();

	bool addVertexAndFragmentShaderFromFile(string vertexFileName, 
											string fragmentFileName, 
											osg::Node *node,
											bool addReloadCallback = true);
	bool addVertexShaderFromFile(string, osg::Node*, bool addReloadCallback = true);
	bool addFragmentShaderFromFile(string, osg::Node*, bool addReloadCallback = true);

private:
	bool loadShaderSource(osg::Shader* obj, const string fileName );

};


class ShaderReloadCallback : public osg::NodeCallback
{
public:
	ShaderReloadCallback();
	~ShaderReloadCallback();
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);

	void init(string, string);
	void setWrappedCallback(osg::NodeCallback *);
private:

	bool isFileModified(std::string FileName, struct tm* OrigFileClock);

	string vertexFileName, fragmentFileName;
	bool isVertexShaderOn, isFragmentShaderOn;

	struct tm vertFileClock;
	struct tm fragFileClock;

	ShaderFactory sf;
	
	//osg::NodeCallback *wrappedCallback;
	osg::ref_ptr<osg::NodeCallback> wrappedCallback;
};


#endif