#version 150

#define TWO_PI 6.283185307179586476925286766559

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    outputColor = vec4(0.0, normalized.x, 1.0, 1.0);
}