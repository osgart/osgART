uniform sampler2D tex;

uniform vec2 imageDimension;
uniform vec2 centerPos;
uniform float radius;

varying vec4 position;

void main()
{
	int k=0;

	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	

	vec2 centerCoord = centerPos / imageDimension;
	vec2 shiftVec = gl_TexCoord[0].xy - centerCoord;
	float dist = length( shiftVec );
	
	//float twist = 3.14 /4.0;
	//float angle = length(gl_TexCoord[k].st) * twist;
	float angle = dist * 6.28 / radius - 3.14;
	//float angle = twist;
	float sinLength = sin(angle);
	float cosLength = cos(angle);

	vec2 modifiedCoord = gl_TexCoord[0].st;
	if (  dist < radius )
	{	
		  shiftVec.x = cosLength * shiftVec.x - sinLength * shiftVec.y;
		  shiftVec.y = sinLength * shiftVec.x +	cosLength * shiftVec.y;
		
		  modifiedCoord = centerCoord + shiftVec;
	}

	gl_FragColor = texture2D( tex, modifiedCoord);
}
