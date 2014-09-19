#version 330 core

in vec3 position;
in vec2 texcoord;

uniform mat4 mvp;

out vec2 uv;

void main()
{
	vec4 pos = vec4(position.x, position.y, 0.0, 1.0);
	gl_Position = mvp * pos;
	uv = texcoord;
}
