uniform sampler2D tex01;
uniform sampler2D tex02;

//varying vec2 shift;

void main(void) 
{
      
   vec4 warpImage = texture2D( tex01, gl_TexCoord[0].st);	
   vec4 oriImage = texture2D( tex02, gl_TexCoord[0].st);	



   //float alpha = warpImage.w;
   gl_FragColor = oriImage + warpImage ;
   //gl_FragColor = warpImage + oriImage;  
   //gl_FragColor = warpImage;  
}


