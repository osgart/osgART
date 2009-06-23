uniform sampler2D tex;

const vec3 lumconst = {0.3, 0.59, 0.11};


vec4 make_tones_n(vec3 Pos)
{
	float n = 0.5*(1.0+noise1(Pos.x * Pos.y));

	float s = Pos.z;
	float n2 = (n<s) ? 0.0 : 1.0;
	return vec4(n2,n2,n2,1.0);
}

vec4 vectone_n_PS()
{
	vec4  scnC = texture2D( tex, gl_TexCoord[0].xy);
	float  lum   = dot(lumconst, scnC.xyz);
	vec2 lx0   = vec2(gl_TexCoord[0].xy);
	vec3 lx    = vec3(lx0.x, lx0.y, lum);
	vec4 dotC  = make_tones_n(lx);

	return vec4(dotC.xyz,1.0);
}


void main(void)
{
    gl_FragColor = vectone_n_PS();
}
