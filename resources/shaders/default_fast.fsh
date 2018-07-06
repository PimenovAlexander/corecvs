uniform sampler2D      textureSampler;
uniform sampler2DArray multiTextureSampler;

uniform sampler2D bumpSampler;
varying lowp vec4 col;
varying lowp float vTexIdAttr;
varying lowp vec2  vTexCoord;
varying lowp vec4  normal;

void main() {
   //gl_FragColor = col;
   //gl_FragColor.rg = vTexCoord;
   //gl_FragColor.rgb = normal.xyz;
   
   gl_FragColor.bgr  = texture2DArray(multiTextureSampler, vec3(vTexCoord, vTexIdAttr)).rgb;

   gl_FragDepth = gl_FragCoord.z;
};
