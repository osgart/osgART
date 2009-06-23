uniform sampler2D tex;

uniform vec2 imageDimension;
uniform vec2 tileSize;

const float edgeWidth = 0.001;
const vec3 edgeColor = vec3(0.3, 0.3, 0.3);
void main(void)
{
	vec2 unit;
	unit = 1.0 / tileSize;
	
	vec2 moddedtexCoord = mod( gl_TexCoord[0] , unit );

	vec2 texCoord = gl_TexCoord[0].st - moddedtexCoord + unit * 0.5;	
	
	gl_FragColor = texture2D(tex, texCoord);
	if ( moddedtexCoord.x < edgeWidth ||  moddedtexCoord.y < edgeWidth )
	{
		gl_FragColor.xyz = gl_FragColor.xyz * edgeColor;
	}

		
	
	
}

