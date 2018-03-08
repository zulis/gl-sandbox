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

// Post Complement method
// http://developer.download.nvidia.com/shaderlibrary/webpages/shader_library.html#post_complements
 
// RGB to HSV and HSV to RGB methods
// from http://www.chilliant.com/rgb2hsv.html
float RGBCVtoHUE(in vec3 RGB, in float C, in float V)
{
    vec3 Delta = (V - RGB) / C;
    Delta.rgb -= Delta.brg;
    Delta.rgb += vec3(2,4,6);
    // NOTE 1
    Delta.brg = step(V, RGB) * Delta.brg;
    float H;
    H = max(Delta.r, max(Delta.g, Delta.b));
    return fract(H / 6);
}
 
vec3 RGBtoHSV(in vec3 RGB)
{
    vec3 HSV = vec3(0, 0, 0);
    HSV.z = max(RGB.r, max(RGB.g, RGB.b));
    float M = min(RGB.r, min(RGB.g, RGB.b));
    float C = HSV.z - M;
    if (C != 0)
    {
        HSV.x = RGBCVtoHUE(RGB, C, HSV.z);
        HSV.y = C / HSV.z;
    }
    return HSV;
}
 
vec3 HUEtoRGB(in float H)
{
    float R = abs(H * 6 - 3) - 1;
    float G = 2 - abs(H * 6 - 2);
    float B = 2 - abs(H * 6 - 4);
    return clamp(vec3(R,G,B), 0.0, 1.0);
}
 
vec3 HSVtoRGB(vec3 HSV)
{
    vec3 RGB = HUEtoRGB(HSV.x);
    return ((RGB - 1) * HSV.y + 1) * HSV.z;
}
 
vec3 HSVComplement(vec3 HSV)
{
    // X = Hue, so rotate it for the complement
    vec3 complement = HSV;
    complement.x -= 0.5;
    if (complement.x < 0.0) { complement.x += 1.0; }
    return(complement);
}
 
// Mixes 2 hue values, since they are on a circle
// in HSV we need some weird code for that
float HueMix(float h1, float h2, float v)
{
    float d = abs(h1 - h2);
    if(d <= 0.5)
    {
        return mix(h1, h2, v);
    }
    else if(h1 < h2)
    {
        return fract(mix((h1 + 1.0), h2, v));
    }
    else
    {
        return fract(mix(h1, (h2 + 1.0), v));
    }
}
 
vec3 PostComplement(vec3 input)
{
    // Tweakable values
    vec3 guide = vec3(1.0f, 0.5f, 0.0f); // the RGB colour that you want to 'bring out'
 
    float amount = 0.5f; // influence how much a colour gets mixed toward the guide or complement
 
    // Correlation and Concentration together define a curve along which the colour grading is done
    // tweak these values to see the effects, I think correlation should be < 0.5f 
	// and concentration should be > 1.0f, but I havent double checked that math
    float correlation = 0.5f;
    float concentration = 2.0f;
 
    // Convert everything to HSV
    vec3 input_hsv = RGBtoHSV(input);
    vec3 hue_pole1 = RGBtoHSV(guide);
    vec3 hue_pole2 = HSVComplement(hue_pole1);
 
    // Find the difference in hue, again hue is circular so keep it in a circle
    float dist1 = abs(input_hsv.x - hue_pole1.x); if (dist1 > 0.5) dist1 = 1.0 - dist1;
    float dist2 = abs(input_hsv.x - hue_pole2.x); if (dist2 > 0.5) dist2 = 1.0 - dist2;
 
    float descent = smoothstep(0, correlation, input_hsv.y);
 
    // *there was a version here that forced it 100% but I skipped implementing that*
 
    vec3 output_hsv = input_hsv;
    // Check if we are closer to the guide or to the complement and color grade according
    if(dist1 < dist2)
    {
        // Bring the colour closer to the guide
        float c = descent * amount * (1.0 - pow((dist1 * 2.0), 1.0 / concentration));
        output_hsv.x = HueMix(input_hsv.x, hue_pole1.x, c);
        output_hsv.y = mix(input_hsv.y, hue_pole1.y, c);
    }
    else
    {
        // Bring the colour closer to the complement
        float c = descent * amount * (1.0 - pow((dist2 * 2.0), 1.0 / concentration));
        output_hsv.x = HueMix(input_hsv.x, hue_pole2.x, c);
        output_hsv.y = mix(input_hsv.y, hue_pole2.y, c);
    }
 
    vec3 output_rgb = HSVtoRGB(output_hsv);
    return output_rgb;
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
	
	if(ColorMapIsUsed)
	{
		ambient *= texture(ColorMap, TexCoord);
		diffuse *= texture(ColorMap, TexCoord);
		alpha = texture(ColorMap, TexCoord).a;
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
	vec3 finalColor = vec3(emissive + ambient + diffuse + specular); 
	finalColor = PostComplement(finalColor);
	FragColor = vec4(finalColor, alpha);
}
