 
attribute vec3 motionVec;
varying int removeThis;

varying float intensity;
varying vec4 vertColor;
varying vec3 offsetValue;
const float pi_h = 3.141592/2.0;

void main(void)
{
	
	vec3 v = gl_NormalMatrix * normalize(motionVec);
	//vec3 v = (gl_ModelViewMatrix * vec4(normalize(motionVec),0)).xyz;
	v = normalize(v);
	float leng = length(motionVec);
	
	vec3 n = gl_NormalMatrix * normalize(gl_Normal);
	n = normalize(n);

	float test = dot(v, n);
	float test2 = dot( normalize(motionVec) , normalize(gl_Normal)); 
	offsetValue = vec3(0.0,0.0,0.0);


	// for shading
	vec3 lightDir = normalize(vec3(gl_LightSource[0].position));		
	intensity = dot(lightDir,gl_Normal);
	vertColor = gl_Color;	

	removeThis = 1;
	
	//vec3 viewVec = vec3( glModelViewMatrix[8], glModelViewMatrix[9], glModelViewMatrix[10]);

	float angle = acos(test2);
	if ( test > 0 )
	{

		offsetValue = test* v * leng;
		removeThis = 0;
	}
	
	vec4 modifiedPos = gl_Vertex;
	modifiedPos.xyz = modifiedPos.xyz - offsetValue;
	gl_Position = gl_ModelViewProjectionMatrix * modifiedPos;

}