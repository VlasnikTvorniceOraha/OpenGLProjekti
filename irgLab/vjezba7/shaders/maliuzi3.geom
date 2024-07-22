#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


void main() {    

    vec3 v1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normal = normalize(cross(v1, v2));
    vec3 n = cross(normal, v1); // define a vector perpendicular to the plane of the triangle
    float area = dot(normal, n) / 2.0;

    if (area > 0) {
        gl_Position = gl_in[0].gl_Position;
        EmitVertex();
        gl_Position = gl_in[1].gl_Position;
        EmitVertex();
        gl_Position = gl_in[2].gl_Position;
        EmitVertex();
    
        EndPrimitive();
    }



    

}