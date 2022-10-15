#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct SpotLight {
    vec3 position;
	vec3 direction;
    float cutOff;
    float fadeOut;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_SPOT_LIGHTS 2

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform Material material;

// function prototypes
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{    
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0,0.0,0.0);

    // spot lights
    for(int i = 0; i < NR_SPOT_LIGHTS; i++){
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
    }
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float cos_theta = dot(lightDir, normalize(-light.direction));

    float diff = 0.0f;
    float spec = 0.0f;
    float intens = 1.0f;

    if(cos_theta > light.cutOff){
        // diffuse shading
        diff = max(dot(normal, lightDir), 0.0);
        // specular shading
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    if(cos_theta < light.cutOff * light.fadeOut){
        intens =  (cos_theta - light.cutOff) / ((light.cutOff * light.fadeOut) - light.cutOff);
    }

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * intens * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * intens * vec3(texture(material.specular, TexCoords));

    return (ambient + diffuse + specular);
}

