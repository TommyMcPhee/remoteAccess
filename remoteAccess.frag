#version 150

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;
uniform float[256] series0;
uniform float[256] xData;
uniform float[256] yData;
uniform float[256] aData;
uniform float[256] bData;

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    vec2 position = vec2(0.0);
    float positionFloat = 0.0;
    vec3 color = vec3(0.0);
    float floatIncrement = 1.0;
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    float white = 0.0;
    float luminence = 0.0;
    for(int increment = 0; increment < 256; increment++){
        floatIncrement++;
        position.x += pow((1.0 - abs(normalized.x - xData[increment])), 16) * (2.0 - (abs(normalized.x - 0.5) * 2.0)) / 256.0;
        position.y += pow((1.0 - abs(normalized.y - yData[increment])), 16) * (2.0 - (abs(normalized.y - 0.5) * 2.0)) / 256.0;
        positionFloat = position.x * position.y;
        red = positionFloat * mod(floatIncrement, 4.0);
        green = positionFloat * mod(floatIncrement, 16.0);
        blue = positionFloat * mod(floatIncrement, 128.0);
        white = red * green * blue;
        color.r += red - (white * bData[increment]);
        color.g += green - (white * bData[increment]);
        color.b += blue - (white * bData[increment]);
        /*
        floatIncrement++;
        if(normalized.x > (floatIncrement - 1.0) / 256.0 && normalized.x < floatIncrement / 256.0){
            position.x *= series0[increment];
        }
        if(normalized.x > (floatIncrement - 1.0) / 256.0 && normalized.x < floatIncrement / 256.0){
            position.y *= series0[increment];
        }
        */
    }
    outputColor = vec4(color.r, color.g, color.b, 1.0);
}