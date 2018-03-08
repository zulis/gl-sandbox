#include "common.glsl"

in vec4 vVertex;
in vec3 vNormal;
in vec2 vTexCoord;
in mat3 mTBN;
in mat4 mViewMatrix;

struct Results
{
	vec3 ambient, diffuse, specular;
};

vec3 normal;
//vec3 ViewPosition;

out vec4 FragColor;

/*
void point(in SURFACE_ATTRIBUTES surface, in LIGHT_SOURCE_ATTRIBUTES light,
	inout Results results) {
//	get direction to light:
	vec3 lightDir = light.view_position.xyz - ViewPosition;

//	compute attenuation factor:
	float light_distance = length(lightDir);
	float attenuation = smoothstep(light.attenuation.y, 
            light.attenuation.x, light_distance);
	
	lightDir = normalize(lightDir);
	
//	accumulate ambient:
	results.ambient += surface.ambient * light.ambient * attenuation;
	
//	accumulate diffuse:
	float nDotL = max(0.0, dot(vNormal, lightDir));
	results.diffuse += (surface.diffuse * light.diffuse * attenuation)
            * nDotL;
	
	if (nDotL > 0.0) { // if fragment is illuminated
	//	accumulate specular:
		vec3 viewDir = normalize(ViewPosition);
		vec3 reflection = reflect(lightDir, vNormal);
		float specular = max(0.0, dot(reflection, viewDir));
		
		results.specular += surface.specular * light.specular *
                    pow(specular, surface.shininess) * attenuation;
	}
}
*/

void point(vec3 normal, vec3 viewPosition, MaterialInfo surface, LightSource light, inout Results results)
{
	// Get direction to light.
	vec3 lightDir;
	
	vec3 lightPosition = vec3(ViewMatrix * light.position);
	
	if(NormalMapIsUsed)
		lightDir = normalize(mTBN * (lightPosition - viewPosition));
	else
		lightDir = lightPosition - viewPosition;

	// Compute attenuation factor.
	float light_distance = length(lightDir);
	float attenuation = smoothstep(light.attenuation.y, light.attenuation.x, light_distance);
	
	lightDir = normalize(lightDir);
	
	// Accumulate ambient.
	results.ambient += surface.ambient.xyz * light.ambient.xyz * attenuation;
	
	// Accumulate diffuse.
	float nDotL = max(0.0, dot(normal, lightDir));
	results.diffuse += (surface.diffuse.xyz * light.diffuse.xyz * attenuation) * nDotL;
	
	// If fragment is illuminated accumulate specular.
	if (nDotL > 0.0)
	{
		vec3 viewDir;
		if(NormalMapIsUsed)
			viewDir = normalize(mTBN * viewPosition);
		else
			viewDir = normalize(viewPosition);
		vec3 reflection = reflect(lightDir, normal);
		float specular = max(0.0, dot(reflection, viewDir));
		
		results.specular += surface.specular.xyz * light.specular.xyz * pow(specular, surface.shininess) * attenuation;
	}
}

void directional(vec3 normal, vec3 viewPosition, MaterialInfo surface, LightSource light, inout Results results) {
/*	
	// Get direction to light.
	vec3 lightDir = normalize(light.view_position.xyz);

	// Accumulate ambient.
	results.ambient += surface.ambient * light.ambient;
	
	// Accumulate diffuse.
	float nDotL = max(0.0, dot(vNormal, lightDir));
	results.diffuse += (surface.diffuse * light.diffuse) * nDotL;
	
	// If fragment is illuminated accumulate specular.
	if (nDotL > 0.0)
	{
		vec3 viewDir = normalize(ViewPosition);
		vec3 reflection = reflect(lightDir, vNormal);
		float specular = max(0.0, dot(reflection, viewDir));
		results.specular += surface.specular * light.specular * pow(specular, surface.shininess);
	}
*/	
}

void spot(vec3 normal, vec3 viewPosition, MaterialInfo surface, LightSource light, inout Results results)
{
/*
	// Get direction to light.
	vec3 lightDir = light.view_position.xyz - ViewPosition;
	float spot_dot_l = dot(normalize(light.spot_viewDir), normalize(-lightDir));
	
	// Compute attenuation factor.
	float light_distance = length(lightDir);
	float attenuation = smoothstep(light.attenuation.y, light.attenuation.x, light_distance);
	
	lightDir = normalize(lightDir);
	
	// Accumulate ambient.
	results.ambient += surface.ambient * light.ambient * attenuation;
	
	if (spot_dot_l > light.spot_cutoff)
	{
		// Incorporate spot direction into attenuation factor.
		attenuation *= pow(spot_dot_l, light.spot_exponent);
	
		// Accumulate diffuse.
		float nDotL = max(0.0, dot(vNormal, lightDir));
		results.diffuse += (surface.diffuse * light.diffuse * attenuation) * nDotL;
		
		// If fragment is illuminated accumulate specular.
		if (nDotL > 0.0)
		{ 
			vec3 viewDir = normalize(ViewPosition);
			vec3 reflection = reflect(lightDir, vNormal);		
			float specular = max(0.0, dot(reflection, viewDir));
			results.specular += surface.specular * light.specular * pow(specular, surface.shininess) * attenuation;
		}
	}
*/	
}

void main()
{
	if(OpacityMapIsUsed && texture(OpacityMap, vTexCoord).r == 0.0)
		discard;
		
	// fetch the normal from the normal map
	vec3 vMappedNormal = texture(NormalMap, vTexCoord.st).rgb * 2.0 - 1.0;
	
	// modify it using the normal & tangents from the 3D mesh (normal mapping)
	vec3 vSurfaceNormal = NormalMapIsUsed ? normalize(mTBN * vMappedNormal) : vNormal;
	
	// apply each of our light sources
	vec4 vDiffuseColor	= EmissiveMapIsUsed ? texture(EmissiveMap, vTexCoord.st) : vec4(0, 0, 0, 1);
	vec4 vSpecularColor	= vec4(0, 0, 0, 1);

	vec3 vToCamera = normalize(-vVertex.xyz);
	
	float alpha = 1.0;
	
	for (int i = 0; i < TotalLights; ++i)
	{
		vec3 lightPosition = vec3(mViewMatrix * Lights[i].position);
		
		// calculate view space light vectors
		vec3 vToLight = normalize(lightPosition /*Lights[i].position.xyz*/ - vVertex.xyz); 
		vec3 vReflect = normalize(-reflect(vToLight, vSurfaceNormal));
		
		// calculate diffuse term
		float fDiffuse = max(dot(vSurfaceNormal, vToLight), 0.0);
		fDiffuse = clamp(fDiffuse, 0.1, 1.0);
		
		// calculate specular term
		float fSpecularPower = 100.0;
		float fSpecular = pow(max(dot(vReflect, vToCamera), 0.0), fSpecularPower);
		fSpecular = clamp(fSpecular, 0.0, 1.0);
		
		// calculate final colors
		if(DiffuseMapIsUsed)
		{
			vDiffuseColor += texture(DiffuseMap, vTexCoord.st) * Lights[i].diffuse * fDiffuse;
			alpha = texture(DiffuseMap, vTexCoord.st).a;
		}
		else
			vDiffuseColor += Lights[i].diffuse * fDiffuse;
		
		if(SpecularMapIsUsed)
			vSpecularColor += texture(SpecularMap, vTexCoord.st) * Lights[i].specular * fSpecular;
		else
			vSpecularColor += Lights[i].specular * fSpecular; 		
	}
	
	FragColor.rgb = (vDiffuseColor + vSpecularColor).rgb;
	FragColor.a = alpha;
	

/*
	if(OpacityMapIsUsed && texture(OpacityMap, vTexCoord).r == 0.0)
		discard;
		
	if(NormalMapIsUsed && TotalLights > 0)
	{
		normal = normalize(texture(NormalMap, vTexCoord).rgb * 2.0 - 1.0);
		//normal = texture(NormalMap, vTexCoord).rgb * 2.0 - 1.0;
		//normal = normalize(mTBN * normal);
		
	}
	else	
		normal = vNormal;
	
//	init results accumulator:
	Results results;
	results.ambient = vec3(0.0);
	results.diffuse = vec3(0.0);
	results.specular = vec3(0.0);

//	accumulate results:
	for (int i = 0; i < TotalLights; ++i)
	{
		if (Lights[i].position.w != 0.0) { // w = 1; local
			if (Lights[i].exponent != 0.0) { // spot light
				spot(normal, vVertex.xyz, Material, Lights[i], results);
			} else { // point light
				point(normal, vVertex.xyz, Material, Lights[i], results);
			}
		} else { // w = 0; directional
			directional(normal, vVertex.xyz, Material, Lights[i], results);
		}				
	}
	
	results.specular = clamp(results.specular, 0.0, 1.0);
	
	float alpha = 1.0;
	vec3 ambient = Material.ambient.xyz;
	vec3 diffuse = Material.diffuse.xyz;
	vec3 specular = vec3(0.0);
	vec3 emissive = vec3(0.0);
	
	if(TotalLights > 0)
	{
		ambient = results.ambient;
		diffuse = results.diffuse;
		specular = results.specular;
	}
	
	if(DiffuseMapIsUsed)
	{
		ambient *= texture(DiffuseMap, vTexCoord).rgb;
	}
	
	if(DiffuseMapIsUsed)
	{
		diffuse *= texture(DiffuseMap, vTexCoord).rgb;
		alpha = texture(DiffuseMap, vTexCoord).a;
	}
	
	if(EmissiveMapIsUsed)
	{
		emissive = texture(EmissiveMap, vTexCoord).rgb;
	}
	
	if(SpecularMapIsUsed)
	{
		specular *= texture(SpecularMap, vTexCoord).rgb;
	}
	
	// final color
	FragColor = vec4(emissive + ambient + diffuse + specular, alpha);
	*/
}
