#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 Normala;

out vec3 normala;

out vec3 pozicijaFragmenta;

uniform mat4 perspektiva;
uniform mat4 pogled;
uniform mat4 model;

void main()
{

    pozicijaFragmenta = vec3(model * vec4(aPos, 1.0));

    normala = Normala;


    gl_Position = perspektiva * pogled * vec4(pozicijaFragmenta, 1.0);
}