#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aCol;


uniform mat4 perspektiva;
uniform mat4 pogled;


void main()
{
    vec4 outvektor = vec4(aPos, 1.0);
    gl_Position = perspektiva * pogled * outvektor;
}