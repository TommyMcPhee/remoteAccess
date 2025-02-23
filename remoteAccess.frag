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
    vec4 position = vec4(0.0);
    float colorPosition = 0.0;
    float whitePosition = 0.0;
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
        position.z = beam(normalized.x, data[increment].z, index);
        position.w = beam(normalized.y, data[increment].w, index);
        colorPosition = position.x * position.y * data[increment].y;
        whitePosition = position.z * position.w * data[increment].w;
        red = colorPosition * modQuotient(floatIncrement, 4.0) / amplitude;
        green = colorPosition * modQuotient(floatIncrement, 16.0) / amplitude;
        blue = colorPosition * modQuotient(floatIncrement, 32.0) / amplitude;
        white = red * green * blue * whitePosition;
        newColor.r += red - white;
        newColor.g += green - white;
        newColor.b += blue - white;
    }
    outputColor = vec4(newColor, 1.0);
}