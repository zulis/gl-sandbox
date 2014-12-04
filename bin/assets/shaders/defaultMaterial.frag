#include "common.glsl"

in vec2 TexCoord;
in vec3 ViewPosition;
in vec3 ViewNormal;
in mat3 TBN;

struct Results
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

out vec4 FragColor;

void point(vec3 normal, vec3 viewPosition, MaterialInfo surface, LightSource light, inout Results results)
{
	// Get light position in view space.
	vec3 lightPosition = vec3(ViewMatrix * light.position);
	
	// Get direction to light.
	vec3 lightDirection = TBN * (lightPosition - viewPosition);

	// Compute attenuation factor.
	float light_distance = length(lightDirection);
	float attenuation = smoothstep(light.attenuation.y, light.attenuation.x, light_distance);
	
	lightDirection = normalize(lightDirection);
	
	// Accumulate ambient.
	results.ambient += surface.ambient * light.ambient * attenuation;
	
	// Accumulate diffuse.
	float nDotL = max(0.0, dot(normal, lightDirection));
	results.diffuse += (surface.diffuse * light.diffuse * attenuation) * nDotL;
	
	// If fragment is illuminated accumulate specular.
	if (nDotL > 0.0)
	{
		vec3 viewDirection = normalize(TBN * viewPosition);
		vec3 reflection = reflect(lightDirection, normal);
		float specular = max(0.0, dot(reflection, viewDirection));
		
		results.specular += surface.specular * light.specular * pow(specular, surface.shininess) * attenuation;
	}
}

void directional(vec3 normal, vec3 viewPosition, MaterialInfo surface, LightSource light, inout Results results)
{
	// Get light position in view space.
	vec3 lightPosition = vec3(ViewMatrix * light.position);
	vec3 lightLookAtPosition = vec3(ViewMatrix * light.lookAt);
	
	// Get direction to light.
	vec3 lightDirection = normalize(TBN * (lightPosition - lightLookAtPosition));

	// Accumulate ambient.
	results.ambient += surface.ambient * light.ambient;
	
	// Accumulate diffuse.
	float nDotL = max(0.0, dot(normal, lightDirection));
	results.diffuse += (surface.diffuse * light.diffuse) * nDotL;
	
	// If fragment is illuminated accumulate specular.
	if (nDotL > 0.0)
	{
		vec3 viewDirection = normalize(TBN * viewPosition);
		vec3 reflection = reflect(lightDirection, normal);
		float specular = max(0.0, dot(reflection, viewDirection));
		
		results.specular += surface.specular * light.specular * pow(specular, surface.shininess);
	}
}

void spot(vec3 normal, vec3 viewPosition, MaterialInfo surface, LightSource light, inout Results results)
{
	// Get light position in view space.
	vec3 lightPosition = vec3(ViewMatrix * light.position);
	vec3 lightLookAtPosition = vec3(ViewMatrix * light.lookAt);
	
	// Get direction to light.
	vec3 lightDirection = TBN * (lightPosition - viewPosition);
	vec3 spotViewDirection = TBN * (lightLookAtPosition - viewPosition);
	
	float sDotL = dot(normalize(spotViewDirection), normalize(-lightDirection));
	
	// Compute attenuation factor.
	float light_distance = length(lightDirection);
	float attenuation = smoothstep(light.attenuation.y, light.attenuation.x, light_distance);
	
	lightDirection = normalize(lightDirection);
	
	// Accumulate ambient.
	results.ambient += surface.ambient * light.ambient * attenuation;

	if (sDotL > 0.0) //light.cutoff)
	{
		// Incorporate spot direction into attenuation factor.
		attenuation *= pow(sDotL, light.exponent);
		
		// Accumulate diffuse.
		float nDotL = max(0.0, dot(normal, lightDirection));
		results.diffuse += (surface.diffuse * light.diffuse * attenuation) * nDotL;
		
		// If fragment is illuminated accumulate specular.
		if (nDotL > 0.0)
		{
			vec3 viewDirection = normalize(TBN * viewPosition);
			vec3 reflection = reflect(lightDirection, normal);
			float specular = max(0.0, dot(reflection, viewDirection));
		
			results.specular += surface.specular * light.specular * pow(specular, surface.shininess) * attenuation;
		}
	}
	
/*
	// Get direction to light.
	vec3 lightDirection = light.view_position.xyz - ViewPosition;
	float spot_dot_l = dot(normalize(light.spot_viewDirection), normalize(-lightDirection));
	
	// Compute attenuation factor.
	float light_distance = length(lightDirection);
	float attenuation = smoothstep(light.attenuation.y, light.attenuation.x, light_distance);
	
	lightDirection = normalize(lightDirection);
	
	// Accumulate ambient.
	results.ambient += surface.ambient * light.ambient * attenuation;
	
	if (spot_dot_l > light.spot_cutoff)
	{
		// Incorporate spot direction into attenuation factor.
		attenuation *= pow(spot_dot_l, light.spot_exponent);
	
		// Accumulate diffuse.
		float nDotL = max(0.0, dot(ViewNormal, lightDirection));
		results.diffuse += (surface.diffuse * light.diffuse * attenuation) * nDotL;
		
		// If fragment is illuminated accumulate specular.
		if (nDotL > 0.0)
		{ 
			vec3 viewDirection = normalize(ViewPosition);
			vec3 reflection = reflect(lightDirection, ViewNormal);		
			float specular = max(0.0, dot(reflection, viewDirection));
			results.specular += surface.specular * light.specular * pow(specular, surface.shininess) * attenuation;
		}
	}
*/	
}

void main()
{
	if(OpacityMapIsUsed && texture(OpacityMap, TexCoord).r == 0.0)
		discard;
	
	vec3 normal;
	
	if(NormalMapIsUsed && TotalLights > 0)
		normal = normalize(texture(NormalMap, TexCoord).rgb * 2.0 - 1.0);
	else	
		normal = ViewNormal;
	
//	init results accumulator:
	Results results;
	results.ambient = vec4(0.0);
	results.diffuse = vec4(0.0);
	results.specular = vec4(0.0);

//	accumulate results:
	for (int i = 0; i < TotalLights; ++i)
	{
		if (Lights[i].position.w != 0.0) { // w = 1; local
			if (Lights[i].exponent != 0.0) { // spot light
				spot(normal, ViewPosition, Material, Lights[i], results);
			} else { // point light
				point(normal, ViewPosition, Material, Lights[i], results);
			}
		} else { // w = 0; directional
			directional(normal, ViewPosition, Material, Lights[i], results);
		}				
	}
	/*for (int i = 0; i < TotalLights; ++i)
	{
		LIGHT_SOURCE_ATTRIBUTES LIGHT_SOURCE;
		LIGHT_SOURCE.ambient = Lights[i].ambient.xyz;
		LIGHT_SOURCE.diffuse = Lights[i].diffuse.xyz;
		LIGHT_SOURCE.specular = Lights[i].specular.xyz;
		LIGHT_SOURCE.view_position = ViewMatrix * Lights[i].position;
		LIGHT_SOURCE.attenuation = Lights[i].attenuation;
		//////////////////////////////////////////////////////////
		LIGHT_SOURCE.spot_viewDirection = -Lights[i].position.xyz; // ???
		LIGHT_SOURCE.spot_cutoff = cos(Lights[i].cutoff);
		LIGHT_SOURCE.spot_exponent = Lights[i].exponent;
		
		if (LIGHT_SOURCE.view_position.w != 0.0) { // w = 1; local
			if (LIGHT_SOURCE.spot_exponent != 0.0) { // spot light
				spot(surface, LIGHT_SOURCE, results);
			} else { // point light
				point(surface, LIGHT_SOURCE, results);
			}
		} else { // w = 0; directional
			directional(surface, LIGHT_SOURCE, results);
		}
	}*/
	
	//results.specular = clamp(results.specular, 0.0, 1.0);
	
	float alpha = 1.0;
	vec4 ambient = Material.ambient;
	vec4 diffuse = Material.diffuse;
	vec4 specular = vec4(0.0);
	vec4 emissive = vec4(0.0);
	
	if(TotalLights > 0)
	{
		ambient = results.ambient;
		diffuse = results.diffuse;
		specular = results.specular;
	}
	
	if(DiffuseMapIsUsed)
	{
		ambient *= texture(DiffuseMap, TexCoord);
		diffuse *= texture(DiffuseMap, TexCoord);
		alpha = texture(DiffuseMap, TexCoord).a;
	}
	
	if(EmissiveMapIsUsed)
	{
		emissive = texture(EmissiveMap, TexCoord);
	}
	
	if(SpecularMapIsUsed)
	{
		specular *= texture(SpecularMap, TexCoord);
	}
	
	// final color
	FragColor = vec4(vec3(emissive + ambient + diffuse + specular), alpha);
}
