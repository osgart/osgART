/* -*-glsl-*- */

vec3 cubeMap( in vec3 normal, in vec3 eye )
{
    vec3 u = normalize( eye );
    
    return reflect( u, normal );
}

void main()
{
    gl_Position = gl_ModelViewMatrix * gl_Vertex;
    
    // first transform the normal into eye space and normalize the result
    vec3 normal = normalize( gl_NormalMatrix * gl_Normal );
    vec3 eye = ( vec3(gl_Position) ) / gl_Position.w;
    
    gl_TexCoord[0] = vec4(cubeMap( normal, eye ), 0.0) *
        gl_ModelViewMatrix;
    
    gl_Position = ftransform();
} 
