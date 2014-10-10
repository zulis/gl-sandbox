#include "common.glsl"

//in vec3 LightDir;
//in vec3 ViewDir;
in vec2 TexCoord;

smooth in vec4 VIEW_POSITION;
noperspective in vec3 VIEW_NORMAL;

out vec4 FragColor;

void main()
{
<<<<<<< HEAD
	FragColor = vec4(1.0);

/*
=======
	if(OpacityMapIsUsed && texture(OpacityMap, TexCoord).r == 0.0)
		discard;
		
>>>>>>> origin/master
	vec3 L = normalize(LightDir);
	vec3 N = normalize(texture(NormalMap, TexCoord.st).xyz * 2.0 - 1.0);
	vec3 V = normalize(ViewDir);
	vec3 R = normalize(-reflect(L, N));

	float nDotL = max(0.0, dot(N, L));
	float rDotV = max(0.0, dot(R, V));
	
	vec4 ambient = Lights[MaxLights].ambient * Material.ambient;
	vec4 diffuse = Lights[MaxLights].diffuse * Material.diffuse * nDotL;
	vec4 specular = Lights[MaxLights].specular * Material.specular * pow(rDotV, Material.shininess);
	vec4 texel = texture(DiffuseMap, TexCoord);
	float gloss = texture(SpecularMap, TexCoord).r;
		
	FragColor = (ambient + diffuse + (gloss * specular)) * texel;
*/
}
