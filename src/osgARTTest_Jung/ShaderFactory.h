#ifndef SHADERFACTORY_H
#define SHADERFACTORY_H

#include <osg/Node>
#include <string>
#include <vector>
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

class ShaderPackage
{
public:
	ShaderPackage();
	virtual ~ShaderPackage();

	void init( string _fileName, osg::Shader *_shader);
	bool isModified();
	void update();

private:
	string fileName;
	osg::Shader *shader;
	struct tm fileClock;
};

class ShaderReloadCallback : public osg::NodeCallback
{
public:
	ShaderReloadCallback();
	~ShaderReloadCallback();
	virtual void operator()(osg::Node *nd, osg::NodeVisitor* nv);

	void addShader(string, osg::Shader*);
	void setWrappedCallback(osg::NodeCallback *);
private:
	
	osg::ref_ptr<osg::NodeCallback> wrappedCallback;

	vector<ShaderPackage> shaders;
};


#endif