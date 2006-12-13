uniform samplerCube tex0;

void main ( void )
{
    gl_FragColor = textureCube( tex0, gl_TexCoord[0].stp );
}
