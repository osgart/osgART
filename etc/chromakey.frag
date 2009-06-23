uniform samplerRect tex;

void main (void)
{
//  CHROMA KEY SHADER
    vec4 lowColor=textureRect( tex, gl_TexCoord[0].st);
    
    float threshold_ru=0.38;
    float threshold_rd=0.1;
    float threshold_gu=0.6;
    float threshold_gd=0.32;
    float threshold_bu=0.32;
    float threshold_bd=0.09;      
           
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