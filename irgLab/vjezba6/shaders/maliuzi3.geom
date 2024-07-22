#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


void main() {    

    float x1 = gl_in[0].gl_Position.x;
    float y1 = gl_in[0].gl_Position.y;
    float x2 = gl_in[1].gl_Position.x;
    float y2 = gl_in[1].gl_Position.y;
    float x3 = gl_in[2].gl_Position.x;
    float y3 = gl_in[2].gl_Position.y;

    if (((y2 - y1) * (x3 - x2) - (y3 - y2) * (x2 - x1)) < 0.0f)
    {
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
    }

    EndPrimitive();



    

}