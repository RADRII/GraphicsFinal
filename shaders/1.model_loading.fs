#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D texture_normal1;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

//Point light Specification
const int NUM_POINT_LIGHTS = 1;

struct PointLight {
    vec3 position;
    vec3 colour;
    // Attenuation parameters
    float constant;
    float linear;
    float quadratic;
};

uniform PointLight pointLights[NUM_POINT_LIGHTS];

//General Specification
// Imported from Main code
uniform float ambientStrength;
uniform vec3 ambientColour;
uniform float shininess;

// Directional Light Specification
uniform vec3 dlightDirection;
uniform vec3 dlColour;

// Camera Position
uniform vec3 viewPos;

vec3 calculatePL(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
    // Distance between point light and fragment
    float distance = length(light.position - fragPos);
    // Attentuation
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Diffuse Shading
    vec3 lightDirection = normalize(light.position - fragPos);
    float diffuseS = max(dot(normal, lightDirection), 0.0);
    diffuseS *= max(dot(normal, lightDirection), 0.0);

    // Specular shading
    float specularStrength = texture(texture_specular1, TexCoords).r;
    vec3 reflectDir = reflect(-lightDirection, normal);
    float specularS = pow(max(dot(viewDir, reflectDir), 0.0), shininess);

    // calculate the diffuse and specular components
    vec3 diffuse = diffuseS * light.colour * attenuation;
    vec3 specular = specularStrength * specularS * light.colour * attenuation;

    return (diffuse + specular) * texture(texture_diffuse1, TexCoords).rgb;
}


    void main()
{    
    // Calculate Ambiant Light
    vec3 diffuseColour = texture(texture_diffuse1, TexCoords).rgb;
    vec3 ambient = ambientStrength * ambientColour * diffuseColour;

    // Calculate Directional Light
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(-dlightDirection);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 normal = normalize(texture(texture_normal1, TexCoords).rgb * 2.0 - 1.0);
    diff *= max(dot(normal, lightDirection), 0.0);
    float specularStrength = texture(texture_specular1, TexCoords).r;
    vec3 viewDirection = normalize(viewPos - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), shininess);

    // Combine Directional Light with diffuse and spec
    vec3 cAmbient = (ambientStrength * ambientColour) * texture(texture_diffuse1, TexCoords).rgb;
    vec3 cDiffuse = diff * dlColour * texture(texture_diffuse1, TexCoords).rgb;
    vec3 cSpecular = specularStrength * spec * dlColour;
    vec3 result = (cAmbient + cDiffuse + cSpecular);

    // Calculate all point lights
    for(int i = 0; i < NUM_POINT_LIGHTS; i++) {
        result += calculatePL(pointLights[i], norm, FragPos, viewDirection);
    }
    
    FragColor = vec4(result, 1.0);
    
}