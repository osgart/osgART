
varying vec3  normal;
varying vec3  viewRayVec;
varying vec4  position;
varying float lightIntensity;

uniform vec3  lightPos;

void main(void) 
{
	vec4 pos       = gl_ModelViewProjectionMatrix * gl_Vertex;
	
    gl_Position    = pos;
	viewRayVec     = (gl_ModelViewMatrix * gl_Vertex).xyz ;
    normal         = normalize(gl_NormalMatrix * gl_Normal);
    position       = pos;

	vec3 lightVec = normalize(lightPos - gl_Vertex.xyz);
    lightIntensity = max( dot( lightVec , normalize(gl_Normal) ), 0.0 );
}

