#version 330 core
out vec4 FragColor;

uniform vec3 bojaKrivulje;


void main()
{
    FragColor = vec4(bojaKrivulje, 0.0);  

} 