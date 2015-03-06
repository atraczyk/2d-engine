//VERTICAL GBLUR FRAGMENT SHADER
uniform sampler2D s_texture;
varying vec2 v_texCoord;
uniform float blurSize;

void main()                                            
{
	vec4 Color = vec4(0.0);
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y - 4.0*blurSize)) * 0.05;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y - 3.0*blurSize)) * 0.09;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y - 2.0*blurSize)) * 0.12;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y - blurSize)) * 0.15;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y)) * 0.18;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y + blurSize)) * 0.15;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y + 2.0*blurSize)) * 0.12;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y + 3.0*blurSize)) * 0.09;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y + 4.0*blurSize)) * 0.05;

	float max = 0.1;
	if(Color.r > max) Color.r = max;
	if(Color.g > max) Color.g = max;
	if(Color.b > max) Color.b = max;
	
	gl_FragColor = Color;
}