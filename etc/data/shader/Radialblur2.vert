uniform vec2 imageDimension;

uniform int numToRepeat;
uniform vec2 centerPos;
uniform vec2 radAndLength;


void main(void) {

	gl_TexCoord[0] = gl_MultiTexCoord0;

	// let's set the texture coordinate

	vec2 centerCoord = centerPos / imageDimension;

	vec2 shiftDirec = vec2( 0.0, 0.0);
	vec2 shiftVec = gl_TexCoord[0].st - centerCoord;
	float dist = length( shiftVec );
	shiftVec = normalize(shiftVec);

	if (  dist > radAndLength.x )
	{
		float noise = (noise1(gl_TexCoord[0].s * gl_TexCoord[0].t) + 1.0) / 2 * 0.825;
		noise = noise + 0.125;
		shiftDirec = shiftVec * radAndLength.y * noise;
	}
	
	gl_TexCoord[1].st = gl_TexCoord[0].st + shiftDirec;
	gl_TexCoord[2].st = gl_TexCoord[1].st + shiftDirec;
	gl_TexCoord[3].st = gl_TexCoord[2].st + shiftDirec;
	gl_TexCoord[4].st = gl_TexCoord[3].st + shiftDirec;
	gl_TexCoord[5].st = gl_TexCoord[4].st + shiftDirec;
	gl_TexCoord[6].st = gl_TexCoord[5].st + shiftDirec;
	gl_TexCoord[7].st = gl_TexCoord[6].st + shiftDirec;
	


	gl_Position = ftransform();
} 

