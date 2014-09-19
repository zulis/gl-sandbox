#version 430

in vec2 TexCoord;

layout(binding = 0) uniform sampler2D ColorTex;

out vec4 FragColor;

void main()
{
	FragColor = texture2D(ColorTex, TexCoord);
}