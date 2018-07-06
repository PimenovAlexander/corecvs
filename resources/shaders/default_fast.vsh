attribute lowp vec4 posAttr;
attribute lowp vec4 colAttr;
attribute lowp vec4 faceColAttr;
attribute lowp vec4 normalAttr;
attribute lowp vec2 texAttr;
attribute lowp int texIdAttr;
varying lowp vec4 col;
varying lowp vec4 normal;
varying lowp vec2  vTexCoord;
varying lowp float vTexIdAttr;
uniform lowp mat4 modelview;
uniform lowp mat4 projection;


void main() {
   vTexCoord.x = texAttr.x;
   vTexCoord.y = 1.0 - texAttr.y;
   vTexIdAttr = texIdAttr;
   gl_Position = projection * modelview * posAttr;
}
