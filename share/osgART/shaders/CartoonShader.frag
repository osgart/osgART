	
varying float intensity;
varying vec4 vertColor;

void main()
{
	vec4 color = vertColor;
		
	if (intensity > 0.95)
		color = color * vec4(0.5,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = color *  vec4(0.3,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = color *  vec4(0.2,0.2,0.2,1.0);
	else
		color = color *  vec4(0.1,0.1,0.1,1.0);

	gl_FragColor = color;
}