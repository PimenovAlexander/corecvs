attribute highp vec4 posAttr;
attribute lowp  vec4 colAttr;
attribute lowp  vec4 faceColAttr;
attribute lowp  vec4 normalAttr;
attribute lowp  vec2 texAttr;
varying lowp vec4 col;
varying lowp vec4 normal;
varying lowp vec2 vTexCoord;
uniform highp mat4 modelview;
uniform highp mat4 projection;


void main() {
   col = colAttr / 1.0;
   col.a = 1.0;
   vTexCoord.x = texAttr.x;
   vTexCoord.y = 1.0 - texAttr.y;
   normal = normalAttr;
   gl_Position = projection * modelview * posAttr;
}
