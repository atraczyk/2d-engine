uniform float time;
uniform vec2 resolution;

void main()
{
	vec2 position = gl_FragCoord.xy / resolution.xy;
	float height = min(1.0, max(position.y, 0.0));
	float sintime = sin(time*0.125+2.5)*0.5+0.5;
	
	vec4 c0_b =  vec4(244.0/255.0, 215.0/255.0, 8.0/255.0, 1.0);
	vec4 c0_t = vec4(0.567843, 0.280392, 0.9086275, 1.0);

	vec4 c1_b =  vec4(0.67843, 0.80392, 0.86275, 1.0);
	vec4 c1_t = vec4(0.07451, 0.28627, 0.62353, 1.0);

	vec4 c2_b =  vec4(247.0/255.0, 152.0/255.0, 88.0/255.0, 1.0);
	vec4 c2_t = vec4(0.05, 0.0, 0.1, 1.0);
	
	vec4 c0 = mix(c0_b, c0_t, min(1.0, height+0.2));
	vec4 c1 = mix(c1_b, c1_t, min(1.0, height+0.2));
		
	vec4 c2 = mix(c2_b, c2_t, min(1.0, height+0.2));
						
	if(sintime<0.25)
		gl_FragColor = mix(c2_t, c0, sintime*4.0);
	else if(sintime<0.5)
		gl_FragColor = mix(c0, c1, (sintime-0.25)*4.0);
	else if(sintime<0.75)
		gl_FragColor = mix(c1, c2, (sintime-0.5)*4.0);
	else  if(sintime<1.0) 
		gl_FragColor = mix(c2, c2_t, (sintime-0.75)*4.0);
}