//HORIZONTAL GBLUR FRAGMENT SHADER
uniform sampler2D s_texture; 
varying vec2 v_texCoord;
uniform float blurSize;
 
void main()
{
	vec4 Color = vec4(0.0); 
	Color += texture2D(s_texture, vec2(v_texCoord.x - 4.0*blurSize, v_texCoord.y)) * 0.05;
	Color += texture2D(s_texture, vec2(v_texCoord.x - 3.0*blurSize, v_texCoord.y)) * 0.09;
	Color += texture2D(s_texture, vec2(v_texCoord.x - 2.0*blurSize, v_texCoord.y)) * 0.12;
	Color += texture2D(s_texture, vec2(v_texCoord.x - blurSize, v_texCoord.y)) * 0.15;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y)) * 0.18;
	Color += texture2D(s_texture, vec2(v_texCoord.x + blurSize, v_texCoord.y)) * 0.15;
	Color += texture2D(s_texture, vec2(v_texCoord.x + 2.0*blurSize, v_texCoord.y)) * 0.12;
	Color += texture2D(s_texture, vec2(v_texCoord.x + 3.0*blurSize, v_texCoord.y)) * 0.09;
	Color += texture2D(s_texture, vec2(v_texCoord.x + 4.0*blurSize, v_texCoord.y)) * 0.05;

	gl_FragColor = Color;
}