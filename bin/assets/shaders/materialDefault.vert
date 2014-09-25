#version 430

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;
in vec4 VertexTangent;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

/*uniform*/ struct DirectionalLight
{
	vec3 direction;
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
} DirectionalLight1;

out vec3 LightDir;
out vec3 ViewDir;
out vec2 TexCoord;

void main()
{
	DirectionalLight1.direction = vec3(-0.5f, -1.0f, -1.0f);
	DirectionalLight1.ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectionalLight1.diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	DirectionalLight1.specular = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	// Create a matrix to transform vectors from eye space to tangent space.

	vec3 n = normalize(NormalMatrix * VertexNormal);
    vec3 t = normalize(NormalMatrix * VertexTangent.xyz);
    vec3 b = cross(n, t) * VertexTangent.w;
    
    mat3 tbnMatrix = mat3(t.x, b.x, n.x,
                          t.y, b.y, n.y,
                          t.z, b.z, n.z);

	// All vertex shaders should write the transformed homogeneous clip space
	// vertex position into the gl_Position variables.

	mat4 modelViewMatrix = ViewMatrix * ModelMatrix;
    mat4 mvpMatrix = ProjectionMatrix * modelViewMatrix;
    vec4 pos = vec4(VertexPosition.x, VertexPosition.y, VertexPosition.z, 1.0);
    
    gl_Position = mvpMatrix * pos;
    
	// Calculate the view direction in eye space. In eye space space the camera
	// position is the origin. The eye space view direction is simply the
	// inverse of the vector to any vertex that has been transformed into eye
	// space. Once we have the eye space view direction vector we transform it
	// to tangent space.
    
    pos = modelViewMatrix * pos;
	ViewDir = -(pos.xyz / pos.w);
	ViewDir = tbnMatrix * ViewDir;
        
    // Transform the light direction into tangent space space. Directional
    // lights are specified in world space. For example, a directional light
    // aimed along the world negative z axis has the vector (0, 0, -1).
    // Since the tbnMatrix will only transform eye space vectors to tangent
    // space we first transform the light direction vector to eye space and
    // then transform it to tangent space.

	LightDir = vec3(ViewMatrix * vec4(-DirectionalLight1.direction, 0.0f));
	LightDir = tbnMatrix * LightDir;
    
    TexCoord = VertexTexCoord;
}
