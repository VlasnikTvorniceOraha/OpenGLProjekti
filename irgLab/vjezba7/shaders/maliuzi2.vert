#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aCol;

out vec3 color;

uniform mat4 pogled;

uniform mat4 perspektiva;

uniform mat4 model;

void main()
{
	color = vec3(0, 0, 0);
    gl_Position = perspektiva * pogled * model * vec4(aPos, 1.0);
}