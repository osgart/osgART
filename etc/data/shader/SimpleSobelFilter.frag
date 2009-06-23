//uniform sampler2DRect tex;
uniform sampler2D tex;
uniform vec2 imageDimension;
uniform float threshold;


void main(void) 
{
	const float unitX = 1.0 / imageDimension.x;
	const float unitY = 1.0 / imageDimension.y;
	const vec3 rgb2lum = vec3(0.30, 0.59, 0.11);
	
	vec2 c[9];
	float lum[9];
	c[0].x = -unitX; c[0].y = unitY; 
	c[1].x =  0.0  ; c[0].y = unitY; 
	c[2].x =  unitX; c[0].y = unitY; 
	c[3].x = -unitX; c[0].y = 0.0; 
	c[4].x =  0.0  ; c[0].y = 0.0; 
	c[5].x =  unitX; c[0].y = 0.0; 
	c[6].x = -unitX; c[0].y = -unitY; 
	c[7].x =  0.0  ; c[0].y = -unitY; 
	c[8].x =  unitX; c[0].y = -unitY; 

   int i=0;
   vec2 tempCoord;
   vec4 tempColor;
   
   for (i=0; i < 9; i++) 
   {	   		
	   //tempColor = textureRect( tex,  gl_TexCoord[0].st + c[i]);	   	  
	   tempColor = texture2D( tex,  gl_TexCoord[0].st + c[i]);	   	  
	   lum[i] = dot( tempColor.xyz, rgb2lum);  
   } 
      
   float x = lum[2]+  lum[8]+2*lum[5]-lum[0]-2*lum[3]-lum[6];
   float y = lum[6]+2*lum[7]+  lum[8]-lum[0]-2*lum[1]-lum[2];

   float edge = sqrt(pow(x,2) + pow(y,2));
    
   if ( edge > threshold )
	   edge = edge * 5000;
   else 
	   edge = 0.0;

   
   vec4 cc =  texture2D( tex,  gl_TexCoord[0].st );
   gl_FragColor.x = cc.x - edge;
   gl_FragColor.y = cc.y - edge;
   gl_FragColor.z = cc.z - edge;

   //edge = 1.0 - edge;
   //gl_FragColor.x = edge;
   //gl_FragColor.y = edge;
   //gl_FragColor.z = edge;

   gl_FragColor.w = 1.0;
}


