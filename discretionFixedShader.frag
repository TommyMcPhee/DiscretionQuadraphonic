#version 150

uniform sampler2DRect tex0;
in vec2 texCoordVarying;
out vec4 outputColor;
uniform float feedback;
uniform vec2 window;
uniform vec3 xTranslate;
uniform vec3 yTranslate;
uniform vec3 zTranslate;
uniform vec3 xDuty;
uniform vec3 yDuty;
uniform vec3 xFrequency;
uniform vec3 yFrequency;
uniform vec3 xAmplitude;
uniform vec3 yAmplitude;
uniform vec3 xPan;
uniform vec3 yPan;

float oscillate(float duty, float phase){
    return step(duty, mod(phase, 1.0));
}

void main()
{
    vec2 normalized = gl_FragCoord.xy / window;
    float pixel = sqrt(window.x * window.y);
    vec2 centered = 1.0 - (abs(0.5 - normalized) * 2.0);
    vec2 normalizedR = (centered * vec2(xPan.r, yPan.r)) * 0.5 + 0.5;
    vec2 normalizedG = (centered * vec2(xPan.g, yPan.g)) * 0.5 + 0.5;
    vec2 normalizedB = (centered * vec2(xPan.b, yPan.b)) * 0.5 + 0.5;
    vec2 phaseR = pixel * centered * vec2(pow(xFrequency.g, 8.0), pow(yFrequency.g, 8.0));
    vec2 phaseG = pixel * centered * vec2(pow(xFrequency.g, 8.0), pow(yFrequency.g, 8.0));
    vec2 phaseB = pixel * centered * vec2(pow(xFrequency.b, 8.0), pow(yFrequency.b, 8.0));
    float rX = float(oscillate(xDuty.r, phaseR.x));
    float rY = float(oscillate(yDuty.r, phaseR.y));
    float gX = float(oscillate(xDuty.g, phaseG.x));
    float gY = float(oscillate(yDuty.g, phaseG.y));
    float bX = float(oscillate(xDuty.b, phaseB.x));
    float bY = float(oscillate(yDuty.b, phaseB.y));
    vec2 stepR = vec2(rX, rY);
    vec2 stepG = vec2(gX, gY);
    vec2 stepB = vec2(bX, bY);
    vec2 red = stepR * vec2(xAmplitude.r, yAmplitude.r) * normalizedR;
    vec2 green = stepG * vec2(xAmplitude.g, yAmplitude.g) * normalizedG;
    vec2 blue = stepB * vec2(xAmplitude.b, yAmplitude.b) * normalizedB;
    vec4 feedbackColorR = texture2DRect(tex0, (texCoordVarying + vec2(xTranslate.r, yTranslate.r)) * zTranslate.r);
    vec4 feedbackColorG = texture2DRect(tex0, (texCoordVarying + vec2(xTranslate.g, yTranslate.g)) * zTranslate.g);
    vec4 feedbackColorB = texture2DRect(tex0, (texCoordVarying + vec2(xTranslate.b, yTranslate.b)) * zTranslate.b);
    vec4 feedbackColor = vec4(feedbackColorR.r, feedbackColorG.g, feedbackColorB.b, 1.0) * feedback;
    vec4 highPass = vec4(1.0 - feedbackColor) * (1.0 - feedback);
    vec4 oldColor = feedbackColor * highPass;
    
    vec4 newColor = vec4(red.x * red.y, green.x * green.y, blue.x * blue.y, 1.0);

    outputColor = mix(newColor, oldColor, pow(abs(0.5 - feedback) * 1.75, 8.0));
}