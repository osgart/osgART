uniform sampler2D tex;

uniform vec2 imageDimension;

void main(void) 
{
	const float unitX = 1.0 / imageDimension.x;
	const float unitY = 1.0 / imageDimension.y;

    float blurWeight[49] = {
0.000244, 0.001465, 0.003662, 0.004883, 0.003662, 0.001465, 0.000244, 
0.001465, 0.008789, 0.021973, 0.029297, 0.021973, 0.008789, 0.001465, 
0.003662, 0.021973, 0.054932, 0.073242, 0.054932, 0.021973, 0.003662, 
0.004883, 0.029297, 0.073242, 0.097656, 0.073242, 0.029297, 0.004883, 
0.003662, 0.021973, 0.054932, 0.073242, 0.054932, 0.021973, 0.003662, 
0.001465, 0.008789, 0.021973, 0.029297, 0.021973, 0.008789, 0.001465, 
0.000244, 0.001465, 0.003662, 0.004883, 0.003662, 0.001465, 0.000244
  };
    const int RAD_HALF = 3;
	const float RAD_HALF_F = 3.0;
    const int RAD = 7;


   int i=0, k=0;
   vec2 coord = gl_TexCoord[0].st;
   float xCoord = coord.x - RAD_HALF_F * unitX;
   coord.y = coord.y + RAD_HALF_F * unitY;

   vec4 avg;
  
   for (i=0; i < RAD; i++) 
   {
	  coord.x = xCoord;
	  for (k=0; k < RAD; k++) 
	  {
			avg = avg +  texture2D( tex, coord) * blurWeight[i*RAD + k];
			coord.x = coord.x + unitX;
	  }
	  coord.y = coord.y - unitY;
   }   


   gl_FragColor = avg;


   //gl_FragColor.w = 1.0;

 
}


