//VERTICAL WAVE VERTEX SHADER
attribute vec2 v_coord;
uniform sampler2D s_texture;
varying vec2 v_texCoord;
 
void main(void) 
{
	gl_Position = vec4(v_coord, 0.0, 1.0);
	v_texCoord = (v_coord + 1.0) / 2.0;
}