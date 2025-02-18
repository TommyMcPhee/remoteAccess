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
    vec2 position = normalized;
    float floatIncrement = 1.0;
    for(int increment = 0; increment < 256; increment++){
        floatIncrement++;
        if(normalized.x > (floatIncrement - 1.0) / 256.0 && normalized.x < floatIncrement / 256.0){
            position.x *= series0[increment];
        }
        if(normalized.x > (floatIncrement - 1.0) / 256.0 && normalized.x < floatIncrement / 256.0){
            position.y *= series0[increment];
        }
    }
    outputColor = vec4(0.0, position.x * position.y, 1.0, 1.0);
}