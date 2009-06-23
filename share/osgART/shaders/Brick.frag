
//uniform vec3  BrickColor, MortarColor;
const vec3 BrickColor = { 0.75, 0.25, 0.25 };
const vec3 MortarColor = { 0.6, 0.6, 0.6 };

uniform vec2  colRowNum;
uniform vec2  brickRatio;

varying vec2  MCposition;
varying float lightIntensity;

void main(void)
{
    vec3  color;
    vec2  position, useBrick;
    
    position = MCposition / colRowNum;

    if (fract(position.y * 0.5) > 0.5) // poision is the x y index of bricks
        position.x += 0.5;

    position = fract(position);

    useBrick = step(position, brickRatio);
	//useBrick = position * brickRatio;

    color  = mix(MortarColor, BrickColor, useBrick.x * useBrick.y);
    color *= lightIntensity;
    gl_FragColor = vec4 (color, 1.0);

}