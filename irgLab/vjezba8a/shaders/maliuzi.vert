#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 pogled2;

uniform mat4 perspektiva2;

//bruh zasto ne outputas


void main()
{

    vec4 outvektor = vec4(aPos, 1.0);
    gl_Position = perspektiva2 * pogled2 * outvektor;
}