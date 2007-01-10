uniform sampler2DShadow shadowTexture;

void main(void) {

	vec4 shadowColor = shadow2DProj( shadowTexture, gl_TexCoord[0]);

	gl_FragColor = float4(0.0,0.0,0.0,1.0);
	
	if (shadowColor.r > 0.0) {
		//gl_FragColor = lerp(color, shadowColor, 0.9);		
		gl_FragColor = float4(1.0,1.0,1.0,1.0);
		//gl_FragColor.r = 1.0f;
	} 
}



	

