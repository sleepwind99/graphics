#version 330 core
layout (location = 0) in vec2 aPos;
uniform vec2 posset;

void main()
{
   gl_Position = vec4(aPos.x + posset.x, aPos.y + posset.y, 0.0, 1.0);
}