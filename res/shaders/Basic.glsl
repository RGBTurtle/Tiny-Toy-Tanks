#shader vertex
#version 330 core

layout (location = 0) in vec3 position;   
layout (location = 1) in vec3 color; 
  
out vec3 ourColor;

uniform mat4 u_MVP;

void main()
{
    gl_Position = u_MVP * vec4(position, 1.0);
    ourColor = color;
}

#shader fragment
#version 330 core

out vec4 FragColor;  
in vec3 ourColor;
  
void main()
{
    FragColor = vec4(ourColor, 1.0);
}