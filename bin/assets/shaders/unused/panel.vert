#version 400

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 mvp;

out vec2 texCoord;

void main()
{
	gl_Position = /*mvp **/ vec4(position, 0.0, 1.0);
	texCoord = texcoord;
}