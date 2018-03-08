#version 400

in vec2 UV;
out vec3 color;

uniform sampler2D texDiffuse1;
uniform sampler2D texDiffuse2;

void main()
{
	vec4 sample1 = texture(texDiffuse1, UV);
	vec4 sample2 = texture(texDiffuse2, UV);
	color = mix(sample1, sample2, UV.s);
}