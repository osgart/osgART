varying vec3 normal, lightDir, halfVector;

uniform sampler2D baseTexture;
uniform sampler2DShadow shadowTexture;

void main(void) {

	vec4 diffuseLight, specularLight;	
	float shine = 64.0;
	vec3 n = normalize(normal);
	float lightIntensity = max(dot(n, normalize(lightDir)), 0.0);
	diffuseLight = lightIntensity * gl_LightSource[0].diffuse;
	specularLight = pow(max(dot(n, normalize(halfVector)), 0.0), shine) * gl_LightSource[0].specular;

	vec4 color = gl_FrontLightModelProduct.sceneColor+
	diffuseLight*gl_FrontMaterial.diffuse
	 + specularLight*gl_FrontMaterial.specular +
	  gl_LightSource[0].ambient*gl_FrontMaterial.ambient;
	
	vec4 baseColor = texture2D( baseTexture, gl_TexCoord[0].xy );
	
	if (baseColor.a == 0) baseColor.rgb = vec3(0,0,0);
	color = color * baseColor;
	//color = color * gl_Color;

	vec4 shadowColor = shadow2DProj( shadowTexture, gl_TexCoord[1]);

	if (shadowColor.r == 0) {
		gl_FragColor = lerp(color, shadowColor, 0.9);		
	} else {
		gl_FragColor = color;
	}
}



	

