//VERTICAL WAVE FRAGMENT SHADER
uniform sampler2D s_texture;
uniform float offset;
varying vec2 v_texCoord;
 
void main(void) 
{
	vec2 texcoord = v_texCoord;
	texcoord.x += sin((texcoord.y) * 4*2*3.14159 + offset) / 100;
	gl_FragColor = texture2D(s_texture, texcoord);
}