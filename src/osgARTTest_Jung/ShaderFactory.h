#ifndef SHADERFACTORY_H
#define SHADERFACTORY_H

#include <osg/Node>

#include <string>
#include <iostream>
using namespace std;

class ShaderFactory
{

public:
	ShaderFactory();
	virtual ~ShaderFactory();

	bool addVertexAndFragmentShaderFromFile(string vertexFileName, 
											string fragmentFileName, 
											osg::Node *node);
	bool addVertexShaderFromFile(string, osg::Node*);
	bool addFragmentShaderFromFile(string, osg::Node*);

private:
	bool loadShaderSource(osg::Shader* obj, const string fileName );

};
#endif