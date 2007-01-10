uniform mat4 mvpt;

void main(void)
{	
	vec4 worldPos = gl_ModelViewMatrix * gl_Vertex;
	gl_TexCoord[0] = mvpt * worldPos;	
	gl_Position = ftransform();
} 

