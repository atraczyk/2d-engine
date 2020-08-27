//VERTICAL WAVE FRAGMENT SHADER
uniform sampler2D s_texture;
uniform float offset;
varying vec2 v_texCoord;

uniform float curvature;

void main(void) 
{
	vec2 center = vec2(0.5,0.5);
	float radius = 0.25; 
	vec2 textureCoordinateToUse = v_texCoord;
    float dist = distance(center, v_texCoord);
    textureCoordinateToUse -= center;
	float percent = 1.0 + ((0.5 - dist) / 0.5) * curvature;
    textureCoordinateToUse = textureCoordinateToUse / percent;
    textureCoordinateToUse += center;
	vec2 texcoord = textureCoordinateToUse;
	
	vec2 clamped = clamp(texcoord, vec2(0, 0), vec2(1, 1));

	if(!all(equal(clamped, texcoord))) 
	{
		gl_FragColor = vec4(0, 0, 0, 1);
	}
	else
	{
        gl_FragColor = texture2D(s_texture, texcoord);
	}
}


