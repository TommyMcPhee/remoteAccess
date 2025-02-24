#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform vec2 window;
uniform vec4[256] data;

vec3 adjustColor(vec3 inputV, float adjustment){
    float adjustmentPower = pow(adjustment / 256.0, 0.25);
    float adjustRed = pow(inputV.r, adjustmentPower);
    float adjustGreen = pow(inputV.g, adjustmentPower);
    float adjustBlue = pow(inputV.b, adjustmentPower);
    return vec3(adjustRed, adjustGreen, adjustBlue);
}

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
    float amplitude = 0.0;
    float index = 0.0;
    float floatIncrement = 0.0;
    vec4 position = vec4(0.0);
    float colorPosition = 0.0;
    float whitePosition = 0.0;
    float red = 0.0;
    float green = 0.0;
    float blue = 0.0;
    float white = 0.0;
    float feedbackPosition = 0.0;
    vec3 oldColor = texture2DRect(tex0, texCoordVarying).rgb;
    vec3 newColor = vec3(0.0);
    for(int increment = 0; increment < 256; increment++){
            amplitude += data[increment].y;
            index += data[increment].w;
    }
    oldColor = adjustColor(oldColor, index);
    for(int increment = 0; increment < 256; increment++){
        floatIncrement++;
        position.x = beam(normalized.x, data[increment].x, amplitude);
        position.y = beam(normalized.y, data[increment].y, amplitude);
        position.z = beam(normalized.x, data[increment].z, index);
        position.w = beam(normalized.y, data[increment].w, index);
        colorPosition = position.x * position.y * data[increment].y;
        whitePosition = position.z * position.w * data[increment].w;
        red = colorPosition * modQuotient(floatIncrement, 32.0) / amplitude;
        green = colorPosition * modQuotient(floatIncrement, 128.0) / amplitude;
        blue = colorPosition * modQuotient(floatIncrement, 256.0) / amplitude;
        feedbackPosition += red * green * blue * whitePosition / index;
        newColor += vec3(red, green, blue);
    }
    newColor = adjustColor(newColor, amplitude);
    outputColor = vec4(mix(newColor, oldColor, pow(feedbackPosition, 0.25)), 1.0);
}