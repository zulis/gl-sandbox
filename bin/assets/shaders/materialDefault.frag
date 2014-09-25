#version 430

in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;

//layout(binding=0) uniform sampler2D TexDiffuse;
uniform sampler2D TexDiffuse;
uniform sampler2D TexSpecular;
uniform sampler2D TexNormal;

/*uniform*/ struct DirectionalLight
{
	vec3 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
} DirectionalLight1;

/*uniform*/ struct Material
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
} Material1;

out vec4 FragColor;

void main()
{
	DirectionalLight1.direction = vec3(0.0f, 0.0f, -1.0f);
	DirectionalLight1.ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectionalLight1.diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectionalLight1.specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	Material1.ambient = vec4(0.3f, 0.3f, 0.3f, 1.0f);
	Material1.diffuse = vec4(0.7f, 0.7f, 0.7f, 1.0f);
	Material1.specular = vec4(0.5f, 0.5f, 0.5f, 1.0f);
	Material1.shininess = 60.0f;

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
