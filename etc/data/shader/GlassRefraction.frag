
uniform sampler2D tex;

varying vec3  normal;
varying vec3  viewRayVec;
varying vec4  position;
varying float lightIntensity;

void main (void)
{
 
	// calc refraction
	vec3 refractionDir = normalize(viewRayVec) - normalize(normal); // very simple one. 
																	// do more complated refraction later..if you want i mean.
	refractionDir = normalize(refractionDir);

	float depth = ((position.z / position.w) + 1.0f) / 2.0f ;
	vec2 shift = normalize(refractionDir.xy) * depth;
	// felt like multiplying depth value.. which didn't change much

	vec2 textureCoord = vec2(0.0,0.0);
	

	// find screen coordinate.
	vec3 md = position.xyz + refractionDir*150*depth;
	textureCoord.st = (md.xy * 0.5 + position.w*0.5) / position.w;
		
    vec3 refractionColor = vec3 (texture2D(tex, textureCoord));
  	
	// add tiny bit of diffuse color :)
    gl_FragColor = vec4 (refractionColor.xyz, 1.0) * (0.9 + 0.1*lightIntensity);	
}