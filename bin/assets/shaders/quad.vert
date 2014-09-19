#version 330 core

precision highp float;

in vec3 VertexPosition;
in vec2 VertexTexCoord;

uniform mat4 MVP;

out vec2 TexCoord;

void main()
{
	vec4 pos = vec4(VertexPosition.x, VertexPosition.y, 0.0, 1.0);
	gl_Position = MVP * pos;
	TexCoord = VertexTexCoord;
}
