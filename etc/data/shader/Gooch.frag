varying float NdotL;
varying vec3  reflectVec;
varying vec3  viewVec;

const vec3 warmColor = {0.6, 0.6, 0.0};
const vec3 coolColor = {0.0, 0.0, 0.6};


void main (void)
{
    vec3 gooch   = mix(coolColor, warmColor, NdotL);

    float spec    = max(dot(reflectVec, viewVec), 0.0);
    spec          = pow(spec, 32.0);

    gl_FragColor = vec4 (min(gooch + spec, 1.0), 1.0);
}