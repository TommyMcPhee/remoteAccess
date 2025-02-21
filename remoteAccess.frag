#version 150

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;
uniform float[256] series0;
uniform float[256] phaseIncrements;
uniform float[256] panValue;
uniform float[256] amplitudes;
uniform float[256] aData;
uniform float[256] bData;
uniform float amplitude;

float beam(float coordinates, float location, float power){
    float scale = 0.5 + abs(0.5 - location);
    return pow((scale - abs(coordinates - location)) * (1.0 / scale), power);
}

float modQuotient(float inA, float modValue){
    return mod(inA, modValue) / modValue;
}

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    vec2 position = vec2(0.0);
    float amplitudeScale = 1.0 / (1.0 + amplitude);
    float positionFloat = 0.0;
    vec3 color = vec3(0.0);
    float layerAmplitude = 0.0;
    float floatIncrement = 1.0;
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    float white = 0.0;
    float luminence = 0.0;
    for(int increment = 0; increment < 256; increment++){
        floatIncrement++;
        //layerAmplitude = amplitudes[increment] / 256.0;
        layerAmplitude = mod(panValue[increment], 1.0 / 256.0);
        position.x = beam(normalized.x, panValue[increment], amplitude);
        position.y = beam(normalized.y, phaseIncrements[increment], amplitude);
        //position.x = beam(normalized.x, series0[increment], amplitude, xData[increment] / 256.0);
        //position.y = beam(normalized.y, series0[increment], amplitude, yData[increment] / 256.0);
        positionFloat = position.x * position.y * layerAmplitude;
        red = positionFloat * modQuotient(floatIncrement, 4.0);
        green = positionFloat * modQuotient(floatIncrement, 16.0);
        blue = positionFloat * modQuotient(floatIncrement, 128.0);
        white = red * green * blue;
        color.r += red - (white * bData[increment]);
        color.g += green - (white * bData[increment]);
        color.b += blue - (white * bData[increment]);
    }
    outputColor = vec4(color.r, color.g, color.b, 1.0);
}