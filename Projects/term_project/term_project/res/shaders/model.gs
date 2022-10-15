#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT{
    vec2 texCoords;
    vec3 FragPos;
}gs_in[];

out vec2 TexCoords; 
out vec3 fragPos;
out vec3 aNormal;

uniform float time;
uniform bool isCompos;

vec4 decomposition(vec4 pos, vec3 norm)
{
    float magnitude = 0.5;
    vec3 dir = norm * ((sin(time) + 1.0f) / 2.0f) * magnitude; 
    return pos + vec4(dir, 0.0);
} 

vec3 getNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

void main() {    
    vec3 norm = getNormal();
    for(int i = 0; i < 3; i++){
        gl_Position = (isCompos) ? decomposition(gl_in[i].gl_Position, norm) : gl_in[i].gl_Position;
        TexCoords = gs_in[i].texCoords;
        fragPos = gs_in[i].FragPos;
        aNormal = norm;
        EmitVertex();
    }
    EndPrimitive();
}  