#include "common.glsl"

in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{
	//Lights[0].position = normalize(-vec3(0.0, 0.0, 5.0));
	//Lights[0].ambient = vec4(1.0, 244.0 / 255.0, 214.0 / 255.0, 1.0);
	//Lights[0].diffuse = vec4(1.0, 244.0 / 255.0, 214.0 / 255.0, 1.0);
	//Lights[0].specular = vec4(1.0, 1.0, 1.0, 1.0);
	
	Material.ambient = vec4(0.3, 0.3, 0.3, 1.0);
	Material.diffuse = vec4(0.7, 0.7, 0.7, 1.0);
	Material.specular = vec4(0.5, 0.5, 0.5, 1.0);
	Material.shininess = 60.0;

	vec3 L = normalize(LightDir);
	vec3 N = normalize(texture(NormalMap, TexCoord.st).xyz * 2.0 - 1.0);
	vec3 V = normalize(ViewDir);
	vec3 R = normalize(-reflect(L, N));

	float nDotL = max(0.0, dot(N, L));
	float rDotV = max(0.0, dot(R, V));
	
	vec4 ambient = Lights[0].ambient * Material.ambient;
	vec4 diffuse = Lights[0].diffuse * Material.diffuse * nDotL;
	vec4 specular = Lights[0].specular * Material.specular * pow(rDotV, Material.shininess);
	vec4 texel = texture(DiffuseMap, TexCoord);
	float gloss = texture(SpecularMap, TexCoord).r;
		
    FragColor = (ambient + diffuse + (gloss * specular)) * texel;
}
