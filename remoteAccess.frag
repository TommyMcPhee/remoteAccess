#version 150

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;
uniform vec4[256] data;

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
    float positionFloat = 0.0;
    vec3 newColor = vec3(0.0);
    float floatIncrement = 0.0;
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    float white = 0.0;
    float amplitude = 0.0;
    float index = 0.0;
    for(int increment = 0; increment < 256; increment++){
            amplitude += data[increment].y;
            index += data[increment].w;
    }
    for(int increment = 0; increment < 256; increment++){
        floatIncrement++;
        position.x = beam(normalized.x, data[increment].x, amplitude);
        position.y = beam(normalized.y, data[increment].y, amplitude);
        positionFloat = position.x * position.y * data[increment].y;
        red = positionFloat * modQuotient(floatIncrement, amplitude / 3.0) / amplitude;
        green = positionFloat * modQuotient(floatIncrement, amplitude / 2.0) / amplitude;
        blue = positionFloat * modQuotient(floatIncrement, amplitude) / amplitude;
        white = red * green * blue * data[increment].w / index;
        newColor.r += red - white;
        newColor.g += green - white;
        newColor.b += blue - white;
    }
    outputColor = vec4(newColor, 1.0);
}