//BILINEAR
varying vec2 v_texCoord;
uniform sampler2D s_texture;
uniform float xSize;
uniform float ySize;

float height = 2.0 / ySize;

void main()                                            
{
	//BILINEAR BLEND
	vec4 Color = vec4(0.0);
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y)) * 0.25;
	Color += texture2D(s_texture, vec2(v_texCoord.x, v_texCoord.y + ySize)) * 0.25;
	Color += texture2D(s_texture, vec2(v_texCoord.x + xSize, v_texCoord.y + ySize)) * 0.25;
	Color += texture2D(s_texture, vec2(v_texCoord.x + xSize, v_texCoord.y )) * 0.25;
	Color.rgb = Color.rgb + (0.025*asin(sin(0.785*v_texCoord.y*height)));
	Color.a = 1.0;
	gl_FragColor = Color;
}