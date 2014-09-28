#version 440

#include "common.glsl"

in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;

out vec4 FragColor;

void main()
{
	DirectionalLight1.direction = vec3(0.0, 0.0, -1.0);
	DirectionalLight1.ambient = vec4(1.0, 244.0 / 255.0, 214.0 / 255.0, 1.0);
	DirectionalLight1.diffuse = vec4(1.0, 244.0 / 255.0, 214.0 / 255.0, 1.0);
	DirectionalLight1.specular = vec4(1.0, 1.0, 1.0, 1.0);
	
	Material1.ambient = vec4(0.3, 0.3, 0.3, 1.0);
	Material1.diffuse = vec4(0.7, 0.7, 0.7, 1.0);
	Material1.specular = vec4(0.5, 0.5, 0.5, 1.0);
	Material1.shininess = 60.0;

	vec3 L = normalize(LightDir);
	vec3 N = normalize(texture(TexNormal, TexCoord.st).xyz * 2.0 - 1.0);
	vec3 V = normalize(ViewDir);
	vec3 R = normalize(-reflect(L, N));

	float nDotL = max(0.0, dot(N, L));
	float rDotV = max(0.0, dot(R, V));
	
	vec4 ambient = DirectionalLight1.ambient * Material1.ambient;
	vec4 diffuse = DirectionalLight1.diffuse * Material1.diffuse * nDotL;
	vec4 specular = DirectionalLight1.specular * Material1.specular * pow(rDotV, Material1.shininess);
	vec4 texel = texture(TexDiffuse, TexCoord);
	float gloss = texture(TexSpecular, TexCoord).r;
		
    FragColor = (ambient + diffuse + (gloss * specular)) * texel;
}
