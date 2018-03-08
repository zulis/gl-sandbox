#version 330 core

precision highp float;

in vec3 VertexPosition;
in vec2 VertexTexCoord;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

uniform vec3 Position;
uniform vec3 CamRight;
uniform vec3 CamUp;

out vec2 TexCoord;

void main()
{

	vec3 center = Position
			+ CamRight * VertexPosition.x * 1.0
			+ CamUp * VertexPosition.y * 1.0;
		
	gl_Position = ProjectionMatrix * ViewMatrix * vec4(center, 1.0);
	
	TexCoord = VertexTexCoord;
}
