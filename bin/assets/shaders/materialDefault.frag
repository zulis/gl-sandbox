#include "common.glsl"

in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{
	Light.direction = vec3(0.0, 0.0, -1.0);
	Light.ambient = vec4(1.0, 244.0 / 255.0, 214.0 / 255.0, 1.0);
	Light.diffuse = vec4(1.0, 244.0 / 255.0, 214.0 / 255.0, 1.0);
	Light.specular = vec4(1.0, 1.0, 1.0, 1.0);
	
	Material.ambient = vec4(0.3, 0.3, 0.3, 1.0);
	Material.diffuse = vec4(0.7, 0.7, 0.7, 1.0);
	Material.specular = vec4(0.5, 0.5, 0.5, 1.0);
	Material.shininess = 60.0;

	vec3 L = normalize(LightDir);
	vec3 N = normalize(texture(TexNormal, TexCoord.st).xyz * 2.0 - 1.0);
	vec3 V = normalize(ViewDir);
	vec3 R = normalize(-reflect(L, N));

	float nDotL = max(0.0, dot(N, L));
	float rDotV = max(0.0, dot(R, V));
	
	vec4 ambient = Light.ambient * Material.ambient;
	vec4 diffuse = Light.diffuse * Material.diffuse * nDotL;
	vec4 specular = Light.specular * Material.specular * pow(rDotV, Material.shininess);
	vec4 texel = texture(TexDiffuse, TexCoord);
	float gloss = texture(TexSpecular, TexCoord).r;
		
    FragColor = (ambient + diffuse + (gloss * specular)) * texel;
}
