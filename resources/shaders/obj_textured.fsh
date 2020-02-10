uniform sampler2D textureSampler;
uniform sampler2D bumpSampler;
varying lowp vec4 col;
varying lowp vec2 vTexCoord;
varying lowp vec4 normal;

void main() {
   gl_FragColor.rgb  = texture2D(textureSampler, vTexCoord).rgb;
   gl_FragColor.a = 1.0;
   gl_FragDepth = gl_FragCoord.z;
};
