#version 330
 
uniform sampler2D texture_1;
uniform sampler2D texture_2;
uniform float elapsed_time;
uniform vec4 color;
uniform float deformation;
uniform int parameters;
 
in vec2 texcoord;

layout(location = 0) out vec4 out_color;

void main()
{
	vec2 new_texcoord = texcoord;

	vec4 recolor;

	if (parameters == 2)
	{
		recolor = vec4(0.5 + 0.5 * sin(elapsed_time), 0.5 + 0.5 * cos(elapsed_time), 0.5 + 0.5 * tan(elapsed_time), 1);
	}
	else
		recolor = color;
	
	vec4 texture_color1 = texture2D(texture_1, new_texcoord);
	vec4 texture_color2 = texture2D(texture_2, new_texcoord);
	if (parameters == 1)
		out_color = mix(texture_color1, texture_color2, deformation);
	else
	{
		vec4 texture_color;
		texture_color = mix(texture_color1, texture_color2, 0.5f);
	
		if (recolor.a < 0.5 || texture_color.a < 0.5)
		{
			out_color = texture_color;
		}
		else
			out_color = mix(texture_color, recolor, 0.1 + deformation * 0.5);
	}

	if (out_color.a < 0.5) {
		discard;
	}
}