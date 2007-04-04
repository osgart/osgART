#include "ColorTemperatureEffect.h"

ColorTemperatureEffect::ColorTemperatureEffect()
{
	stTemp = 6500;
	edTemp = 4100;
}
	
ColorTemperatureEffect::~ColorTemperatureEffect()
{
}

void ColorTemperatureEffect::init(float _timeInMillisec)
{
	ShaderEffect::init( _timeInMillisec );

	createFragmentShaderFromFile( "./data/shader/WhiteBalancing.frag" );

	colorTemp = new osg::Uniform("colorTemp", stTemp);
	
}
	
void ColorTemperatureEffect::whenOn(float t)
{	
	float currentTemp = stTemp * ( 1.0 - t ) + edTemp * t;
	colorTemp->set( currentTemp );
	
}
	
void ColorTemperatureEffect::whenOff(float t)
{
	float currentTemp = edTemp * ( 1.0 - t ) + stTemp * t;
	colorTemp->set( currentTemp );
}

void ColorTemperatureEffect::linkTo( osg::Node *nd)
{
	ShaderEffect::linkTo( nd );
	nd->getOrCreateStateSet()->addUniform( colorTemp.get() );
}