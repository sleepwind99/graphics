#version 330 core
in vec2 TexCoords;

out vec4 color;
uniform sampler2D texture_;

void main( )
{
    color = vec4( texture( texture_, TexCoords ));
}