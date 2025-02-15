#version 150

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;
uniform vec2 mouse;
uniform vec2 mouseLeft;
uniform vec2 mouseCenter;
uniform vec2 mouseRight;
uniform float progress;
uniform float droneAmplitude;
uniform float keytotal;
float maxkeytotal = 0.0;

float spatialize(float mousePosition, float coordinates){
    return ((1.0 - mousePosition) * (1.0 - coordinates) + (mousePosition * coordinates)) / 2.0;
}

float oscillate(float frequency){
    return sin(TWO_PI * frequency) * 0.5 + 0.5;
}

float modmix(float quantity, float modulo){
    return (mod(quantity, modulo) / modulo) + (0.5 / modulo);
}

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    if(keytotal > maxkeytotal){
        maxkeytotal = keytotal;
    }
    vec2 oscillation = vec2(pow(keytotal, modmix(keytotal, maxkeytotal)));
    vec2 spatializeLeft = vec2(spatialize(mouseLeft.x, normalized.x), spatialize(mouseLeft.y, normalized.y));
    vec2 spatializeCenter = vec2(spatialize(mouseCenter.x, normalized.x), spatialize(mouseCenter.y, normalized.y));
    vec2 spatializeRight = vec2(spatialize(mouseRight.x, normalized.x), spatialize(mouseRight.y, normalized.y));
    float red = mix(oscillate(spatializeLeft.x * oscillation.x) * spatializeLeft.x, oscillate(spatializeLeft.y * oscillation.y) * spatializeLeft.y, modmix(keytotal, 3.0));
    float green = mix(oscillate(spatializeCenter.x * oscillation.x) * spatializeCenter.x, oscillate(spatializeCenter.y * oscillation.y) * spatializeCenter.y, modmix(keytotal, 5.0));
    float blue = mix(oscillate(spatializeRight.x * oscillation.x) * spatializeRight.x, oscillate(spatializeRight.y * oscillation.y) * spatializeRight.y, modmix(keytotal, 7.0));
    vec3 lowPass = texture2DRect(tex0, texCoordVarying * mouse + ((1.0 - mouse) * texCoordVarying * 0.5)).rgb;
    vec3 highPass = vec3(1.0 - lowPass);
    vec3 filterColor = vec3(mix(lowPass, highPass, mod(keytotal, 2.0)));
    float filterMix = pow(progress, abs(sin(TWO_PI * modmix(maxkeytotal, keytotal) * normalized.x * normalized.y + (mouse.x * mouse.y))));
    vec3 filteredColor = mix(vec3(red, green, blue), filterColor, filterMix);
    float white = filteredColor.r * filteredColor.g * filteredColor.b * (1.0 - droneAmplitude);
    outputColor = vec4(filteredColor - white, 1.0);
}