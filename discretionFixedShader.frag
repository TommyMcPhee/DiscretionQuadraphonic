#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
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

vec3 normalizedAdd(vec3 inA, vec3 inB){
    return (inA + inB) / (1.0 + inB);
}

float averageThree(float inA, float inB, float inC){
    return (inA + inB + inC) / 3.0;
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
    vec4 feedbackColorR = texture2DRect(tex0, (texCoordVarying + (vec2(xTranslate.r, yTranslate.r) * window)) * (zTranslate.r + 1.0) * centered);
    vec4 feedbackColorG = texture2DRect(tex0, (texCoordVarying + (vec2(xTranslate.g, yTranslate.g) * window)) * (zTranslate.g + 1.0) * centered);
    vec4 feedbackColorB = texture2DRect(tex0, (texCoordVarying + (vec2(xTranslate.b, yTranslate.b) * window)) * (zTranslate.b + 1.0) * centered);
    vec3 feedbackColor = vec3(feedbackColorR.r, feedbackColorG.g, feedbackColorB.b);
    vec3 highPass = 1.0 - feedbackColor;
    vec3 filterColor = feedbackColor * highPass * 2.0;
    vec3 processedFilterColor = vec3(pow(filterColor.r, 0.2), pow(filterColor.g, 0.2), pow(filterColor.b, 0.2));
    vec3 filtration = mix(processedFilterColor, 1.0 - processedFilterColor, pow(1.0 - feedback, 0.5) * pow(1.0 - feedback, 4.0));
    vec3 processedFeedback = vec3(pow(filtration.r, 0.04), pow(filtration.g, 0.04), pow(filtration.b, 0.04));
    vec3 newColor = vec3(red.x * red.y, green.x * green.y, blue.x * blue.y);
    vec3 synthesized = mix(newColor, processedFeedback, pow((0.5 - abs(0.5 - feedback)) * 2.0, 8.0));
    vec4 shrinkColor = texture2DRect(tex0, texCoordVarying * (1.0 - feedback) * centered);
    vec4 staticColor = texture2DRect(tex0, texCoordVarying);
    vec3 derived = shrinkColor.rgb * staticColor.rgb;
    vec3 unprocessed = mix(synthesized, derived, pow(feedback, 0.0625) * pow(feedback, 4.0));
    float yellow = unprocessed.r * unprocessed.g;
    float cyan = unprocessed.g * unprocessed.b;
    float magenta = unprocessed.r * unprocessed.b;
    float white = unprocessed.r * unprocessed.g * unprocessed.b;
    float nonwhite = 1.0 - white;
    float grey = white * nonwhite * 2.0;
    float nongrey = 1.0 - grey;
    float secondary = averageThree(yellow, cyan, magenta);
    float nonsecondary = 1.0 - secondary;
    float primary = nonwhite * nonsecondary;
    float primaryFilter = pow(averageThree(pow(unprocessed.r * primary, unprocessed.r / primary), pow(unprocessed.g * primary, unprocessed.g / primary), pow(unprocessed.b * primary, unprocessed.b / primary)), 0.5);
    float secondaryFilter = pow(averageThree(pow(yellow, yellow / secondary), pow(cyan, cyan / secondary), pow(magenta, magenta / secondary)), 0.25);
    vec3 processed = vec3(pow(unprocessed.r, 0.33), pow(unprocessed.g, 0.33), pow(unprocessed.b, 0.33)) * nonwhite * primaryFilter * secondaryFilter * nongrey;
    outputColor = vec4(processed, 1.0);
}