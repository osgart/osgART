
uniform sampler2D tex;

const vec3 LightColor = { 1.0, 0.9,  0.5  };
const vec3 DarkColor  = { 0.2, 0.05, 0.0  };
const vec3 toLum = {0.3, 0.59, 0.11};

vec4 sepia(vec3 inColor)
{
	vec3 scnColor = LightColor * inColor;
	float lum    = dot(toLum,   scnColor);
	vec3 lum3 = vec3(lum, lum, lum);
    vec3 muted    = mix(scnColor,  lum3,   0.5);
    vec3 sepia    = mix(DarkColor, LightColor, lum);
    vec3 result   = mix(muted,     sepia,      0.85);
    return vec4(result.xyz, 1.0);
}

void main()
{
	gl_FragColor = sepia( texture2D(tex, gl_TexCoord[0].xy).xyz );
}


