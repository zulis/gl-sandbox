#include "common.glsl"

out vec2 TexCoord;
//out vec3 Color;

//uniform vec4 LightPosition;
//uniform vec3 LightIntensity;
vec3 LightIntensity = vec3(1.0);

//uniform vec3 Kd;            // Diffuse reflectivity
//uniform vec3 Ka;            // Ambient reflectivity
//uniform vec3 Ks;            // Specular reflectivity
//uniform float Shininess;    // Specular shininess factor

//uniform mat4 ModelViewMatrix;
//uniform mat3 NormalMatrix;
//uniform mat4 ProjectionMatrix;
//uniform mat4 MVP;

/*
vec3 ads( vec4 position, vec3 norm )
{
    vec3 s;
    if( Lights[0].position.w == 0.0 )
      s = normalize(vec3(Lights[0].position.xyz));
    else
      s = normalize( vec3(Lights[0].position.xyz - position) );

    vec3 v = normalize(vec3(-position));
    vec3 r = reflect( -s, norm );

    return
        LightIntensity * ( Material.ambient.xyz +
           Material.diffuse.xyz * max( dot(s, norm), 0.0 ) +
           Material.specular.xyz * pow( max( dot(r,v), 0.0 ), Material.shininess ) );
}
*/

void main()
{
	gl_Position = MVP * vec4(VertexPosition, 1.0);
	TexCoord = VertexTexCoord;
	
	/*
	vec3 eyeNorm = normalize( NormalMatrix * VertexNormal);
    vec4 eyePosition = ModelViewMatrix * vec4(VertexPosition,1.0);;

    // Evaluate the lighting equation
    Color = ads( eyePosition, eyeNorm );

    gl_Position = MVP * vec4(VertexPosition,1.0);*/
}