#version 330 core
layout ( location = 0 ) in vec3 position;
layout ( location = 1 ) in vec3 normal;
layout ( location = 2 ) in vec2 TexCoords;

out VS_OUT{
    vec2 texCoords;
    vec3 FragPos;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main( )
{
    vs_out.FragPos = vec3(model * vec4(position, 1.0));
    vs_out.texCoords = TexCoords;
    gl_Position = projection * view * model * vec4( position, 1.0f );
}
