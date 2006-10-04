uniform samplerRect texture;
uniform samplerRect texture2;
	
void main (void)
{
//MATTE SHADER
	vec4 video1 = textureRect( texture, gl_TexCoord[0].st);
	vec4 video2 = textureRect( texture2, gl_TexCoord[0].st);
	
	if (video2.r<0.1)
	{
		gl_FragColor=video1;
		gl_FragColor.a=0.0;
	}
	else
	{
		gl_FragColor=video1;
		gl_FragColor.a=1.0;
	}
}