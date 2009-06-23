//uniform vec3 lightDir;
	
varying float intensity;
varying vec4 vertColor;

void main()
{

    vec3 lightDir = normalize(vec3(gl_LightSource[0].position));		
	intensity = dot(lightDir,gl_Normal);
	vertColor = gl_Color;	

	gl_Position = ftransform();
} 