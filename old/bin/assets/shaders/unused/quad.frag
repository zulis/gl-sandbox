#version 330 core

in vec2 uv;
out vec4 color;

uniform sampler2D texDiffuse;

void main()
{
	color = texture2D(texDiffuse, uv);
}
