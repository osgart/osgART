
uniform sampler2D tex;
uniform int numToRepeat;


void main()
{
	int k=0;
	float nn = float(numToRepeat);
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	
	
	for ( k=0; k < 8; k++) // not sure why but variable in for-loop doesnt work so i hard coded the number 
	{
		color = color + texture2D( tex, gl_TexCoord[k].st);
	}

	color = color / nn;
	
	gl_FragColor = color;
}
