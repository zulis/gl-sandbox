#version 330 core

precision highp float;

in vec2 UV;
out vec4 color;

uniform sampler2D texDiffuse;

void main()
{
	color = texture2D(texDiffuse, UV);
}