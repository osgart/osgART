varying vec3 normal, lightDir, halfVector;
uniform mat4 mvpt;

void main(void)
{	
	vec4 worldPos;

	normal = normalize(gl_NormalMatrix * gl_Normal);
		
	worldPos = gl_ModelViewMatrix * gl_Vertex;
	lightDir = normalize(vec3(gl_LightSource[0].position - worldPos));
	
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	
	gl_TexCoord[0].xy = vec2(gl_MultiTexCoord0);
	gl_TexCoord[1] = mvpt * worldPos;
	
	gl_Position = ftransform();
} 

