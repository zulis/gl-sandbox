// normal_mapping_dir.vert
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
// These are setup in the host application.

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

// GLSL 1.30 (OpenGL 3.0) deprecates most of the built-in uniform state
// variables. In GLSL 1.40 (OpenGL 3.1) they have been removed. Typically we
// need to supply projection, view, and model matrices in order to transform
// vertices from model space to clip space.

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;

// GLSL 1.30 (OpenGL 3.0) deprecates vertex shader attribute variables. They
// have been replaced with user-defined generic shader input variables. The
// application code uses the new glVertexAttribPointer() function to map vertex
// data stored in a vertex buffer object to these generic vertex shader input
// variables. This replaces the existing method of specifying vertex array data
// by calling glVertexPointer(), glNormalPointer(), etc.

in vec3 VertexPosition;
in vec3 VertexNormal;
in vec2 VertexTexCoord;
in vec4 VertexTangent;

// GLSL 1.30 (OpenGL 3.0) also deprecates varying shader variables. This has
// been replaced with user-defined generic shader output variables. The output
// of the vertex shader and the input of the fragment shader form an interface.
// For this interface, vertex shader output variables and fragment shader input
// variables of the same name must match in type and qualification (other than
// out matching to in).

out vec3 LightDir;
out vec3 ViewDir;
out vec2 TexCoord;

void main()
{
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

	LightDir = vec3(ViewMatrix * vec4(-Light.direction, 0.0f));
	LightDir = tbnMatrix * LightDir;
    
    TexCoord = VertexTexCoord;
}
