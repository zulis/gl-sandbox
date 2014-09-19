// normal_mapping_dir.frag
//
// Tangent space normal mapping shader for a single directional light source.

#version 430

// Precision qualifiers are added for code portability with OpenGL ES, not for
// functionality. According to the GLSL 1.30 and later specs: the same object
// declared in different shaders that are linked together must have the same
// precision qualification. This applies to inputs, outputs, uniforms, and
// globals.

precision highp float;

// Custom user defined structures used to model the directional light source
// and the characteristics of the material that the light will interact with.
// These are setup in the host application. This must match the declaration in
// the vertex shader.

uniform struct LightInfo
{
	vec3 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
} Light;

uniform struct MaterialInfo
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
} Material;

layout(binding = 0) uniform sampler2D ColorMapSampler;
layout(binding = 1) uniform sampler2D NormalMapSampler;
layout(binding = 2) uniform sampler2D GlossMapSampler;

// Fragment shader input variable declarations must exactly match the vertex
// shader's output variable declarations. The output of the vertex shader and
// the input of the fragment shader form an interface. For this interface,
// vertex shader output variables and fragment shader input variables of the
// same name must match in type and qualification (other than out matching to
// in).

in vec3 LightDir;
in vec3 ViewDir;
in vec2 TexCoord;

// GLSL 1.3 deprecates gl_FragColor. Its replacement is user defined output
// variables. Fragment shader output variables can be explicitly bound to a
// render target. When a program is linked any output variables without an
// explicit binding specified through glBindFragDataLocation() will
// automatically be bound to fragment colors by the GL.

out vec4 FragColor;

void main()
{
	vec3 L = normalize(LightDir);
	vec3 N = normalize(texture(NormalMapSampler, TexCoord.st).xyz * 2.0 - 1.0);
	vec3 V = normalize(ViewDir);
	vec3 R = normalize(-reflect(L, N));

	float nDotL = max(0.0, dot(N, L));
	float rDotV = max(0.0, dot(R, V));
	
	vec4 ambient = Light.ambient * Material.ambient;
	vec4 diffuse = Light.diffuse * Material.diffuse * nDotL;
	vec4 specular = Light.specular * Material.specular * pow(rDotV, Material.shininess);
	vec4 texel = texture(ColorMapSampler, TexCoord);
	float gloss = texture(GlossMapSampler, TexCoord).r;
		
    FragColor = (ambient + diffuse + (gloss * specular)) * texel;
}
