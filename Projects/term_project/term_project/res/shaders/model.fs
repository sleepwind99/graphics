#version 330 core

in vec2 TexCoords;
in vec3 fragPos;  
in vec3 aNormal;  

out vec4 FragColor;

struct Material {
    float shininess;
}; 

uniform bool isAttack;
uniform vec3 viewPos;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular; 
uniform vec3 direction;
uniform Material material;

void main( ){
    vec3 norm = normalize(aNormal);
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 lightDir = normalize(-direction);

    //ambient
    vec3 ambient = vec3(0.4f) * vec3(texture(texture_diffuse, TexCoords));

    //diffuse
    float dotNormLight = dot(norm, lightDir);
    float diff = max(dotNormLight, 0.0);
    vec3 diffuse = vec3(0.7f) * diff * vec3(texture(texture_diffuse, TexCoords));

    //specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = (dotNormLight > 0) ? pow(max(dot(viewDir, reflectDir), 0.0), material.shininess) : 0.0f;
    vec3 specular = vec3(1.0f) * spec * vec3(texture(texture_specular, TexCoords));

    //combine results
    vec3 result =ambient + diffuse + specular;

    // directional lighting
    FragColor = vec4(result, 1.0);
    if(isAttack) FragColor.a = 0.3f;
}