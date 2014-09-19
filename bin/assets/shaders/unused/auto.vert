#version 400

// Input vertex data, different for all executions of this shader.
in vec3 position;
in vec2 texcoord;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

// Values that stay constant for the whole mesh.
//uniform mat4 MVP;
uniform mat4 camera;
uniform mat4 model;

void main(){

	// Output position of the vertex, in clip space : MVP * position
	//gl_Position =  MVP * vec4(position, 1);
	gl_Position = camera * model * vec4(position, 1);
	
	// UV of the vertex. No special space for this one.
	UV = texcoord;
}