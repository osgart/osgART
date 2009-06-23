uniform sampler2D tex01;
uniform sampler2D tex02;

//varying vec2 shift;

void main(void) 
{
   //vec4 foregroundImageDist = texture2D( tex01, gl_TexCoord[0].st);	
   vec4 foregroundImage = texture2D( tex01, gl_TexCoord[0].st);	
   //vec4 offsetImage = texture2D( tex02, gl_TexCoord[0].st);	
   
   //gl_FragColor.x = shift.x;
   //gl_FragColor.y = shift.y;
   //gl_FragColor.z = shift.z;
   
   //gl_FragColor.w = texture2D( tex01, gl_TexCoord[0].st).w;
   //gl_FragColor = (foregroundImageDist + foregroundImage) /2.0;
   //offsetImage.z = 0.0f;
   gl_FragColor = foregroundImage ;

   //gl_FragColor.w = foregroundImageDist.w;
}


