#version 400

in vec2 texCoord;

uniform sampler2D gSampler;
uniform vec4 vertexColor;

out vec4 outputColor;

void main()
{
	vec4 texColor = texture2D(gSampler, texCoord);
	outputColor = (texColor.r, texColor.r, texColor.r, texColor.r) * vertexColor;
}