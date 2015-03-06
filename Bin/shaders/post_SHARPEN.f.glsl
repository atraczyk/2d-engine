//SHARPEN
varying vec2 v_texCoord;
uniform sampler2D s_texture;
uniform float Sharpenfactor = 30.0;
uniform float xSize;
uniform float ySize;

void main()                                            
{
	//sharpen
	vec4 Color = texture2D( s_texture, v_texCoord.xy ); 
	Color -= texture2D( s_texture, v_texCoord.xy+xSize)*Sharpenfactor;
	Color += texture2D( s_texture, v_texCoord.xy-ySize)*Sharpenfactor;
	Color.a = 1.0;
	gl_FragColor = Color;
}