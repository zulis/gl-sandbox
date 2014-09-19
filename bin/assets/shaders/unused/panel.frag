#version 400

in vec2 texCoord;

uniform sampler2D texDiffuse;

out vec4 outputColor;

void main()
{
	outputColor = texture2D(texDiffuse, texCoord);
}