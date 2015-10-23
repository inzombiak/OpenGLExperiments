#version 330 core
 
// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

//in float distance;

// Ouput data
out vec4 color;
 
// Values that stay constant for the whole mesh.
uniform sampler2D DiffuseTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;


void main()
{
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;

	// Distance to the light
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	float cosTheta = clamp( dot( n,l ), 0,1 );

	vec3 MaterialDiffuseColor = texture( DiffuseTextureSampler, UV ).rgb;

	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);
	
// Eye vector (towards the camera)
vec3 E = normalize(EyeDirection_cameraspace);
// Direction in which the triangle reflects the light
vec3 R = reflect(-l,n);
// Cosine of the angle between the Eye vector and the Reflect vector,
// clamped to 0
//  - Looking into the reflection -> 1
//  - Looking elsewhere -> < 1
float cosAlpha = clamp( dot( E,R ), 0,1 );
vec3 colorVec =
	// Ambient : simulates indirect lighting
    MaterialAmbientColor +
    // Diffuse : "color" of the object
    MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) ;
    // Specular : reflective highlight, like a mirror
    MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);
	color = vec4(MaterialDiffuseColor, 0.3);
   
   color.a = 1.0;
}