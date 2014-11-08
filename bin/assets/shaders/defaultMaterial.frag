#include "common.glsl"

in vec2 TexCoord;

smooth in vec4 VIEW_POSITION;
noperspective in vec3 VIEW_NORMAL;

struct LIGHT_SOURCE_ATTRIBUTES {
	vec3 ambient, diffuse, specular; 	
	vec4 view_position;	// in view space
	vec2 attenuation; // x = start, y = end
	
	vec3 spot_view_direction; // in view space
	float spot_cutoff; // cosine of the cutoff angle
	float spot_exponent;
};

struct SURFACE_ATTRIBUTES {
//	supplied by the application:
	vec3 ambient, diffuse, specular;
	float shininess;
	
//	supplied by the vertex shader:
	vec4 view_position;
	vec3 view_normal;
};

struct LIGHTING_RESULTS {
	vec3 ambient, diffuse, specular;
};

out vec4 FRAG_COLOR;

void point(in SURFACE_ATTRIBUTES surface, in LIGHT_SOURCE_ATTRIBUTES light,
	inout LIGHTING_RESULTS results) {
//	get direction to light:
	vec3 light_direction = light.view_position.xyz - surface.view_position.xyz;

//	compute attenuation factor:
	float light_distance = length(light_direction);
	float attenuation = smoothstep(light.attenuation.y, light.attenuation.x, light_distance);
	
	light_direction = normalize(light_direction);
	
//	accumulate ambient:
	results.ambient += surface.ambient * light.ambient * attenuation;
	
//	accumulate diffuse:
	float n_dot_l = max(0.0, dot(surface.view_normal, light_direction));
	results.diffuse += (surface.diffuse * light.diffuse * attenuation)
            * n_dot_l;
	
	if (n_dot_l > 0.0) { // if fragment is illuminated
	//	accumulate specular:
		vec3 view_direction = normalize(surface.view_position.xyz);
		vec3 reflection = reflect(light_direction, surface.view_normal);
		float specular = max(0.0, dot(reflection, view_direction));
		results.specular += surface.specular * light.specular *
                    pow(specular, surface.shininess) * attenuation;
	}
}

void spot(in SURFACE_ATTRIBUTES surface, in LIGHT_SOURCE_ATTRIBUTES light,
	inout LIGHTING_RESULTS results) {
//	get direction to light:
	vec3 light_direction = light.view_position.xyz -
            surface.view_position.xyz;
	float spot_dot_l = dot(normalize(light.spot_view_direction), 
		normalize(-light_direction));
	
//	compute attenuation factor:
	float light_distance = length(light_direction);
	float attenuation = smoothstep(light.attenuation.y, 
            light.attenuation.x, light_distance);
	
	light_direction = normalize(light_direction);
	
//	accumulate ambient:
	results.ambient += surface.ambient * light.ambient * attenuation;
	
	if (spot_dot_l > light.spot_cutoff) {
	//	incorporate spot direction into attenuation factor:
		attenuation *= pow(spot_dot_l, light.spot_exponent);
	
	//	accumulate diffuse:
		float n_dot_l = max(0.0, dot(surface.view_normal,
                    light_direction));
		results.diffuse += (surface.diffuse * light.diffuse *
                    attenuation) * n_dot_l;
			
		if (n_dot_l > 0.0) { // if fragment is illuminated
		//	accumulate specular:
			vec3 view_direction = normalize(surface.view_position.xyz);
			vec3 reflection = reflect(light_direction,
                            surface.view_normal);		
			float specular = max(0.0, dot(reflection, view_direction));
			results.specular += surface.specular * light.specular *
                            pow(specular, surface.shininess) * attenuation;
		}
	}
}

void directional(in SURFACE_ATTRIBUTES surface, in LIGHT_SOURCE_ATTRIBUTES light,
	inout LIGHTING_RESULTS results) {
//	get direction to light:
	vec3 light_direction = normalize(light.view_position.xyz);

//	accumulate ambient:
	results.ambient += surface.ambient * light.ambient;
	
//	accumulate diffuse:
	float n_dot_l = max(0.0, dot(surface.view_normal, light_direction));
	results.diffuse += (surface.diffuse * light.diffuse) * n_dot_l;
	
	if (n_dot_l > 0.0) { // if fragment is illuminated
	//	accumulate specular:
		vec3 view_direction = normalize(surface.view_position.xyz);
		vec3 reflection = reflect(light_direction, surface.view_normal);
		float specular = max(0.0, dot(reflection, view_direction));
		results.specular += surface.specular * light.specular *
                    pow(specular, surface.shininess);
	}	
}

void main()
{
	if(OpacityMapIsUsed && texture(OpacityMap, TexCoord).r == 0.0)
		discard;
	
	vec3 normal = VIEW_NORMAL;
	if(NormalMapIsUsed)
	{
		normal = (normalize(2.0 * texture(NormalMap, TexCoord) - 1.0)).xyz;
		
		
	}

	//	init surface properties:
	SURFACE_ATTRIBUTES surface;
	surface.ambient = Material.ambient.xyz;
	surface.diffuse = Material.diffuse.xyz;
	surface.specular = Material.specular.xyz;
	surface.shininess = Material.shininess;
	surface.view_position = VIEW_POSITION;
	surface.view_normal = normal;

	
//	init results accumulator:
	LIGHTING_RESULTS results;
	results.ambient = vec3(0.0);
	results.diffuse = vec3(0.0);
	results.specular = vec3(0.0);

//	accumulate results:
	for (int i = 0; i < TotalLights; ++i) {

		LIGHT_SOURCE_ATTRIBUTES LIGHT_SOURCE;
		LIGHT_SOURCE.ambient = Lights[i].ambient.xyz;
		LIGHT_SOURCE.diffuse = Lights[i].diffuse.xyz;
		LIGHT_SOURCE.specular = Lights[i].specular.xyz;
		LIGHT_SOURCE.view_position = ViewMatrix * Lights[i].position;
		LIGHT_SOURCE.attenuation = Lights[i].attenuation;
		//////////////////////////////////////////////////////////
		LIGHT_SOURCE.spot_view_direction = -Lights[i].position.xyz; // ???
		LIGHT_SOURCE.spot_cutoff = cos(Lights[i].cutoff);
		LIGHT_SOURCE.spot_exponent = Lights[i].exponent;
		
		point(surface, LIGHT_SOURCE, results);
		
		/*
		if (LIGHT_SOURCE.view_position.w != 0.0) { // w = 1; local
			if (LIGHT_SOURCE.spot_exponent != 0.0) { // spot light
				spot(surface, LIGHT_SOURCE, results);
			} else { // point light
				point(surface, LIGHT_SOURCE, results);
			}
		} else { // w = 0; directional
			directional(surface, LIGHT_SOURCE, results);
		}
		*/
	}
	
	float alpha = 1.0;
	
	vec3 ambient = results.ambient;
	if(DiffuseMapIsUsed)
	{
		ambient *= texture(DiffuseMap, TexCoord).rgb;
		//ambient = clamp(ambient, 0.0, 1.0);
	}
		
	vec3 diffuse = results.diffuse;
	if(DiffuseMapIsUsed)
	{
		diffuse *= texture(DiffuseMap, TexCoord).rgb;
		//diffuse = clamp(diffuse, 0.0, 1.0);
		alpha = texture(DiffuseMap, TexCoord).a;
	}
	
	vec3 specular = results.specular;
	if(SpecularMapIsUsed)
	{
		specular *= texture(SpecularMap, TexCoord).rgb;
		//specular = clamp(specular, 0.0, 1.0);
	}
	
	// final color
	vec4 color = vec4(ambient + diffuse + specular, alpha);
	
	// final color (after gamma correction)
    //vec3 gamma = vec3(1.0/2.2);
    //color = vec4(pow(color.xyz, gamma), alpha);
	
	FRAG_COLOR = color;
}

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
