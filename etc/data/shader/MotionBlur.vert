uniform sampler2D tex01;
uniform sampler2D tex02;

varying vec4 position;
//varying vec2 shift;

void main()
{	
	gl_TexCoord[0] = gl_MultiTexCoord0;
	

	vec2 shift = texture2D( tex02, gl_MultiTexCoord0).xy;
	shift = (shift * 2.0) - 1.0;
	//shift = shift * 0.2;
	//shift = normalize(shift);

		
	//gl_TexCoord[1] = gl_MultiTexCoord0 ;// + shift;
	//gl_TexCoord[1].x = gl_TexCoord[1].x + shift.x/64;
	//gl_TexCoord[1].y = gl_TexCoord[1].y + shift.y/48;
	
	gl_Position = ftransform();
	gl_Position.x = gl_Position.x - shift.x/64.0;
	gl_Position.y = gl_Position.y - shift.y/48.0;
	
} 

