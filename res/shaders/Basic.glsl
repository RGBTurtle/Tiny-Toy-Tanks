#shader vertex
#version 330 core

layout (location = 0) in vec3 aposition;   
layout (location = 1) in vec3 anormal; 
layout (location = 2) in vec2 atexCoord;
  
out vec2 texCoord;

uniform mat4 u_MVP;


void main()
{
    gl_Position = u_MVP * vec4(aposition, 1.0);
    texCoord = atexCoord;
}

#shader fragment
#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D ourTexture;
  
void main()
{
    FragColor = texture(ourTexture, texCoord);
}