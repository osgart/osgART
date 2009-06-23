uniform vec2 imageDimension;

uniform vec3 blurDirectionAndLength;

void main(void) {

	gl_TexCoord[0] = gl_MultiTexCoord0;


	// let's set the texture coordinate

	vec2 unit = 1.0 / imageDimension;

	vec2 shiftDirec = vec2( blurDirectionAndLength.x ,blurDirectionAndLength.y);
	shiftDirec = normalize( shiftDirec ) * blurDirectionAndLength.z * unit;
		
	gl_TexCoord[1].st = gl_TexCoord[0].st + shiftDirec;
	gl_TexCoord[2].st = gl_TexCoord[1].st + shiftDirec;
	gl_TexCoord[3].st = gl_TexCoord[2].st + shiftDirec;
	gl_TexCoord[4].st = gl_TexCoord[3].st + shiftDirec;
	gl_TexCoord[5].st = gl_TexCoord[4].st + shiftDirec;
	gl_TexCoord[6].st = gl_TexCoord[5].st + shiftDirec;
	gl_TexCoord[7].st = gl_TexCoord[6].st + shiftDirec;

	gl_Position = ftransform();
} 

