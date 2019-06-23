uniform sampler2D textureSampler;
uniform sampler2D bumpSampler;
varying lowp vec4 col;
varying lowp vec2 vTexCoord;
varying lowp vec4 normal;

void main() {
   gl_FragColor.r = 0.1;
   gl_FragColor.g = 1.0;
   gl_FragColor.b = 0.1;

   gl_FragColor.a = 1.0;
   gl_FragDepth = gl_FragCoord.z;
};
