

uniform sampler2D modelTexture;
uniform sampler2D shadowTexture;

void main(void) 
{
	float4 model = texture2D( modelTexture,  gl_TexCoord[0].st);
	float4 shadow = texture2D( shadowTexture,  gl_TexCoord[0].st);
	
	//gl_FragColor = shadow;
	gl_FragColor = model * shadow;

	if ( shadow.r == 0 && model.w == 0 )
	{
		gl_FragColor.w = 0.2;
	}
	
}



	

