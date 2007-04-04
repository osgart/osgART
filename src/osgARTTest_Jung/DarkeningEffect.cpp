#include "DarkeningEffect.h"

DarkeningEffect::DarkeningEffect()
{
}
	
DarkeningEffect::~DarkeningEffect()
{
}

void DarkeningEffect::init(float _timeInMillisec)
{
	ShaderEffect::init( _timeInMillisec );

	createFragmentShaderFromFile( "./data/shader/Darkening.frag" );

	weightControl = new osg::Uniform("weightControl", 0.0f);
}

void DarkeningEffect::whenOn(float t)
{
	weightControl->set( t );
}	
	
void DarkeningEffect::whenOff(float t)
{
	weightControl->set( (float) (1.0 - t) );
}

void DarkeningEffect::linkTo( osg::Node *nd)
{
	ShaderEffect::linkTo( nd );
	nd->getOrCreateStateSet()->addUniform( weightControl.get() );
}