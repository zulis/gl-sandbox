#include "common.glsl"

in float i;
in vec2 TexCoord;

layout( location = 0 ) out vec4 FragColor;

void main() {
	/*if(OpacityMapIsUsed && texture(OpacityMap, TexCoord).r == 0.0)
		discard;
		
	vec4 finalColor = texture(DiffuseMap, TexCoord);*/
	
	//Material.ambient = vec4(0.3, 0.3, 0.3, 1.0);
	//Material.diffuse = vec4(0.7, 0.7, 0.7, 1.0);
	//Material.specular = vec4(0.5, 0.5, 0.5, 1.0);
	//Material.shininess = 60.0;
	
    FragColor = vec4( i, i, i, 1.0);
}