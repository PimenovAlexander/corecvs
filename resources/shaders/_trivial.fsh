uniform sampler2D textureSampler;
uniform sampler2D bumpSampler;
varying lowp vec4 col;
varying lowp vec2 vTexCoord;
varying lowp vec4 normal;

void main() {
   //gl_FragColor = col;
   //gl_FragColor.rg = vTexCoord;
   //gl_FragColor.rgb = normal.xyz;
   
   gl_FragColor.rgb  = texture2D(textureSampler, vTexCoord).rgb;
   float diff = dot(normal.xyz, vec3(0.5,0.5,0.5));
   if (diff < 0) 
        diff = 0;
        
   gl_FragColor.r += diff;
   
   gl_FragColor.a = 1.0;
   gl_FragDepth = gl_FragCoord.z;
};