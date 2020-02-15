uniform sampler2D textureSampler;
uniform sampler2D bumpSampler;
varying lowp vec4 col;
varying lowp vec2 vTexCoord;
varying lowp vec4 normal;

uniform int hasTexture;
uniform lowp vec4 ambientUnif;
uniform lowp vec4 diffuseUnif;
uniform lowp vec4 specularUnif;

void main() {
    if (hasTexture == 1) {
        gl_FragColor.rgb  = texture2D(textureSampler, vTexCoord).rgb;
    } else {
        gl_FragColor.rgb  = ambientUnif.rgb;
    }
   gl_FragColor.a = 1.0;
   gl_FragDepth = gl_FragCoord.z;
};
