uniform samplerRect tex;

void main (void)
{
//  ALPHA SHADER
	gl_FragColor = textureRect( tex, gl_TexCoord[0].st);
//  alpha shader: full visibility
//	gl_FragColor.a=1.0;
//  alpha shader: half transparent
//	gl_FragColor.a=0.5;
//  alpha shader: fully transparent 
//	gl_FragColor.a=0.;

//  GREY IMAGE SHADER
//  vec4 lowColor=textureRect( tex, gl_TexCoord[0].st);
//  float averageColor= (lowColor.r+lowColor.b+lowColor.g)/3.;
//  gl_FragColor.r=averageColor;
//  gl_FragColor.g=averageColor;
//  gl_FragColor.b=averageColor;
//  gl_FragColor.a=1.0;

//  BLACK AND WHITE SHADER
//    vec4 lowColor=textureRect( tex, gl_TexCoord[0].st);
//    float averageColor= (lowColor.r+lowColor.b+lowColor.g)/3.;
//    float threshold=0.5;
//    if (averageColor>threshold)
//    {
//    gl_FragColor.r=0.;
//    gl_FragColor.g=0.;
//    gl_FragColor.b=0.;
//	}
//	else
//	{
//    gl_FragColor.r=1.;
//    gl_FragColor.g=1.;
//    gl_FragColor.b=1.;
//	}
//    gl_FragColor.a=1.0;

	//INVERSE SHADER
//	vec4 lowColor=textureRect( tex, gl_TexCoord[0].st);
//	gl_FragColor = 1 - lowColor;
//	gl_FragColor.a=1.0;

//  CHROMA KEY SHADER
    vec4 lowColor=textureRect( tex, gl_TexCoord[0].st);
    
    float threshold_ru=1.0;
    float threshold_rd=0.5;
    float threshold_gu=1.0;
    float threshold_gd=0.5;
    float threshold_bu=1.0;
    float threshold_bd=0.5;      
           
   if (lowColor.r < threshold_ru && lowColor.r > threshold_rd 
    && lowColor.g < threshold_gu && lowColor.g > threshold_gd
    && lowColor.b < threshold_bu && lowColor.b > threshold_bd)
    {
    gl_FragColor.r=0.;
    gl_FragColor.g=0.;
    gl_FragColor.b=0.;
    gl_FragColor.a=0.0;
	}
	else
	{
    gl_FragColor.r=lowColor.r;
    gl_FragColor.g=lowColor.g;
    gl_FragColor.b=lowColor.b;
    gl_FragColor.a=1.0;
	}
}