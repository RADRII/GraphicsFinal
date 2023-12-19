#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 texcoord;

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //TexCoords = mat2(0.0, -1.0, 1.0, 0.0) * texcoord;
    TexCoords = texcoord;
    Normal = norm;
    gl_Position = projection * view * model * vec4(position, 1.0);
    FragPos = vec3(model * vec4(position, 1.0));

}