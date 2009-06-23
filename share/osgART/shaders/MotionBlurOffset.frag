
varying float intensity;
varying vec4 vertColor;
varying int removeThis;
varying vec3 offsetValue;

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

	//color.xyz = offsetValue.xyz;
	//gl_FragColor.xyz = offsetValue.xyz;

	gl_FragColor = color;
	
	gl_FragColor.w = 1.0;

}