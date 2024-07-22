#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform vec3 pozicijaSvjetla;
uniform vec3 svjetlost;

uniform vec3 ambijentalna;
uniform vec3 difuzna;
uniform vec3 spekularna;
uniform float kolicinaSjaja;

uniform vec3 pozicijaKamere;


void main() {    

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    
    EndPrimitive();

}