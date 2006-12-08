uniform sampler2D tex;

uniform vec2 imageDimension;

void main(void) 
{
	const float unitX = 1.0 / imageDimension.x;
	const float unitY = 1.0 / imageDimension.y;

    const int RAD_HALF = 3;
	const float RAD_HALF_F = 3.0;
    const int RAD = 7;


   int i=0, k=0;
   vec2 coord = gl_TexCoord[0].st;
   float xCoord = coord.x - RAD_HALF_F * unitX;
   coord.y = coord.y - RAD_HALF_F * unitY;

   vec4 avg = vec4(0.0,0.0,0.0,0.0);
   vec4 tempVal;
   int counter =0;	

   for (i=0; i < RAD; i++) 
   {
	  coord.x = xCoord;
	  for (k=0; k < RAD; k++) 
	  {
		    tempVal = texture2D( tex, coord);
			if ( tempVal.w != 0.0)
			{
				avg = avg +  tempVal ;
				coord.x = coord.x + unitX;
				counter++;
			}
	  }
	  coord.y = coord.y + unitY;
   }   
   avg = avg / counter;
	
   gl_FragColor = avg; 
 
}