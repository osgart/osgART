
varying vec4 position;

void main()
{	
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec4 pos = ftransform();
	gl_Position = pos;
	position = pos;
} 

