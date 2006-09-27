uniform samplerRect texture;
uniform samplerRect texture2;
	
void main (void)
{
//  MIX VIDEO SHADER
// first video	
	vec4 video1 = textureRect( texture, gl_TexCoord[0].st);
	vec4 video2 = textureRect( texture2, gl_TexCoord[0].st);
	
//	gl_FragColor=video1;
//	gl_FragColor.a=1.0;
	
//	gl_FragColor=video2;
//	gl_FragColor.a=1.0;
	
	gl_FragColor=(video1+video2)/2.;
	gl_FragColor.a=1.0;	

	//MATTE SHADER
//	if (video2.r<0.1)
//	{
//		gl_FragColor=video1;
//		gl_FragColor.a=0.0;
//	}
//	else
//	{
//		gl_FragColor=video1;
//		gl_FragColor.a=1.0;
//	}

}