uniform vec3 lightPos;

varying float NdotL;
varying vec3  reflectVec;
varying vec3  viewVec;

void main(void)
{
    vec3 pos        = vec3 (gl_ModelViewMatrix * gl_Vertex);
    vec3 normal     = normalize(gl_NormalMatrix * gl_Normal);
	
    vec3 lightVec   = normalize(gl_NormalMatrix * normalize(lightPos - gl_Vertex) );
    
	reflectVec      = normalize(reflect(-lightVec, normal));
    viewVec         = normalize(-pos);
    NdotL           = (dot(lightVec, normal) + 1.0) * 0.5; // NdotL for gooch! use all -1 ~ 1
   
	gl_Position     = ftransform();
}