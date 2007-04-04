#ifndef ShaderEffectHandler_h
#define ShaderEffectHandler_h

#include <vector>
using namespace std;
#include "ShaderEffect.h"


class ShaderEffectHandler
{
public:
	ShaderEffectHandler();
	~ShaderEffectHandler();

	void init(osg::Node *);

	void addEffect( osg::ref_ptr<ShaderEffect> effect );
	void switchToEffect(int id);

	void switchToNextEffect()
	{
		int id = index+1;
		if ( id > (int)(effects.size() - 1))
			id = 0;

		switchToEffect( id );
	};
	void update();
	
	void linkTo(int id)
	{
		effects.at(id)->linkTo( node );
	};
	
	void turnOnOff();

private:
	vector< osg::ref_ptr<ShaderEffect> > effects;
	int index;
	int targetIndex;

	osg::Node *node;
};


#endif