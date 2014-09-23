#version 430

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

layout(binding=0) uniform sampler2D TexDiffuse;

struct LightInfo {
  vec4 Position;  // Light position in eye coords.
  vec3 Intensity; // A,D,S intensity
};
//uniform LightInfo Light;
LightInfo Light;

struct MaterialInfo {
  vec3 ambient;            // Ambient reflectivity
  vec3 diffuse;            // Diffuse reflectivity
  vec3 specular;            // Specular reflectivity
  float shininess;    // Specular shininess factor
};
//uniform MaterialInfo Material;
MaterialInfo Material;

layout( location = 0 ) out vec4 FragColor;

void phongModel( vec3 pos, vec3 norm, out vec3 ambAndDiff, out vec3 spec ) {
    vec3 s = normalize(vec3(Light.Position) - pos);
    vec3 v = normalize(-pos.xyz);
    vec3 r = reflect( -s, norm );
    vec3 ambient = Light.Intensity * Material.ambient;
    float sDotN = max( dot(s,norm), 0.0 );
    vec3 diffuse = Light.Intensity * Material.diffuse * sDotN;
    spec = vec3(0.0);
    if( sDotN > 0.0 )
        spec = Light.Intensity * Material.specular *
               pow( max( dot(r,v), 0.0 ), Material.shininess );
    ambAndDiff = ambient + diffuse;
}

void main()
{
	Light.Position = vec4(0.0f,0.0f,0.0f,1.0f);
	Light.Intensity = vec3(1.0f,1.0f,1.0f);
    Material.diffuse = vec3(0.9f, 0.9f, 0.9f);
    Material.specular = vec3(0.95f, 0.95f, 0.95f);
    Material.ambient = vec3(0.1f, 0.1f, 0.1f);
    Material.shininess = 100.0f;

    vec3 ambAndDiff, spec;
    vec4 texColor = texture( TexDiffuse, TexCoord );
    phongModel( Position, Normal, ambAndDiff, spec );
    FragColor = (vec4( ambAndDiff, 1.0 ) * texColor) + vec4(spec,1.0);
}
