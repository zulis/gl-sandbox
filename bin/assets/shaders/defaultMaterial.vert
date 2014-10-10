#include "common.glsl"

//out vec3 LightDir;
//out vec3 ViewDir;
out vec2 TexCoord;

smooth out vec4 VIEW_POSITION;
noperspective out vec3 VIEW_NORMAL;

void main()
{
	VIEW_NORMAL = NormalMatrix * VertexNormal;
	VIEW_POSITION = ModelViewMatrix * vec4(VertexPosition, 1.0);
	TexCoord = VertexTexCoord;
	gl_Position = MVP * vec4(VertexPosition, 1.0);

/*
	// Create a matrix to transform vectors from eye space to tangent space.
	vec3 n = normalize(NormalMatrix * VertexNormal);
    vec3 t = normalize(NormalMatrix * VertexTangent);
	vec3 b = normalize(NormalMatrix * VertexBitangent);
    
    mat3 tbnMatrix = mat3(t.x, b.x, n.x,
                          t.y, b.y, n.y,
                          t.z, b.z, n.z);

	// All vertex shaders should write the transformed homogeneous clip space
	// vertex position into the gl_Position variables.

	//mat4 modelViewMatrix = ViewMatrix * ModelMatrix;
    //mat4 mvpMatrix = ProjectionMatrix * modelViewMatrix;
    //vec4 pos = vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0);
    
    gl_Position = MVP * vec4(VertexPosition, 1.0);
    
	// Calculate the view direction in eye space. In eye space space the camera
	// position is the origin. The eye space view direction is simply the
	// inverse of the vector to any vertex that has been transformed into eye
	// space. Once we have the eye space view direction vector we transform it
	// to tangent space.
    
    vec4 pos = ModelViewMatrix * vec4(VertexPosition, 1.0);
	ViewDir = -(pos.xyz / pos.w);
	ViewDir = tbnMatrix * ViewDir;
        
    // Transform the light direction into tangent space space. Directional
    // lights are specified in world space. For example, a directional light
    // aimed along the world negative z axis has the vector (0, 0, -1).
    // Since the tbnMatrix will only transform eye space vectors to tangent
    // space we first transform the light direction vector to eye space and
    // then transform it to tangent space.

	// original
	//LightDir = vec3(ViewMatrix * vec4(-DirectionalLight1.direction, 0.0f));
	
	//LightDir = vec3(ViewMatrix * vec4(-Lights[0].position.xyz, 0.0f));
	
	if (Lights[MaxLights].position.w == 0.0) // Directional?
		LightDir = normalize(vec3(Lights[MaxLights].position));
    else
	{
		//vec4 eyePosition = ModelViewMatrix * vec4(VertexPosition, 1.0);
		//LightDir = normalize( vec3(Lights[0].position - eyePosition) );
		LightDir = vec3(ViewMatrix * vec4(Lights[MaxLights].position.xyz, 0.0f));
	}
	//LightDir = vec3(ViewMatrix * -normalize(Lights[0].position)); // Spot
	//LightDir = vec3(ViewMatrix * normalize(-Lights[0].position)); // Directional
	
	LightDir = tbnMatrix * LightDir;
    
    TexCoord = VertexTexCoord;
*/	
}
