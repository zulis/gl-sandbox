#version 330 core

precision highp float;

in vec3 position;
in vec2 texcoord;

uniform mat4 mvp;

out vec2 UV;

void main()
{
	gl_Position = mvp * vec4(position, 1);
	UV = texcoord;
}