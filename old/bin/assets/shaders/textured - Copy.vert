#version 330 core

precision highp float;

in vec3 VertexPosition;
in vec2 VertexTexCoord;

uniform mat4 MVP;

out vec2 TexCoord;

void main()
{
	gl_Position = MVP * vec4(VertexPosition, 1.0);
	TexCoord = VertexTexCoord;
}