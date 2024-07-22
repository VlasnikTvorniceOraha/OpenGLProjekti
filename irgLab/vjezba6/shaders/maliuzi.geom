#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

uniform vec3 ociste;

uniform mat4 pogled;

uniform mat4 perspektiva;

uniform mat4 model;

void main() {    

    vec3 centarTrokuta;

    vec4[3] listaTocaka;

    for (int i = 0; i < 3; i++) {

        listaTocaka[i] = model * gl_in[i].gl_Position;
    }

    centarTrokuta = vec3((listaTocaka[0].x + listaTocaka[1].x + listaTocaka[2].x) / 3.0,
                         (listaTocaka[0].y + listaTocaka[1].y + listaTocaka[2].y) / 3.0,
                         (listaTocaka[0].z + listaTocaka[1].z + listaTocaka[2].z) / 3.0);
    
    vec3 eVektor = vec3(ociste.xyz - centarTrokuta.xyz);

    vec3 brid1 = vec3(listaTocaka[1].x - listaTocaka[0].x,
                      listaTocaka[1].y - listaTocaka[0].y,
                      listaTocaka[1].z - listaTocaka[0].z);

    vec3 brid2 = vec3(listaTocaka[2].x - listaTocaka[0].x,
                      listaTocaka[2].y - listaTocaka[0].y,
                      listaTocaka[2].z - listaTocaka[0].z);


    /*centarTrokuta = vec3((gl_in[0].gl_Position.x + gl_in[1].gl_Position.x + gl_in[2].gl_Position.x) / 3.0,
                         (gl_in[0].gl_Position.y + gl_in[1].gl_Position.y + gl_in[2].gl_Position.y) / 3.0,
                         (gl_in[0].gl_Position.z + gl_in[1].gl_Position.z + gl_in[2].gl_Position.z) / 3.0);

    vec3 eVektor = vec3(ociste.xyz - centarTrokuta.xyz);

    vec3 brid1 = vec3(gl_in[1].gl_Position.x - gl_in[0].gl_Position.x,
                      gl_in[1].gl_Position.y - gl_in[0].gl_Position.y,
                      gl_in[1].gl_Position.z - gl_in[0].gl_Position.z);

    vec3 brid2 = vec3(gl_in[2].gl_Position.x - gl_in[0].gl_Position.x,
                      gl_in[2].gl_Position.y - gl_in[0].gl_Position.y,
                      gl_in[2].gl_Position.z - gl_in[0].gl_Position.z);

    */

    vec3 normala = normalize(cross(brid1, brid2));

    if (dot(normala, eVektor) > 0)
    {
        
       for (int i = 0; i < 3; i++) {
            
            gl_Position = perspektiva * pogled * model * vec4(gl_in[i].gl_Position.xyz, 1.0);
            EmitVertex();
       }
    
        EndPrimitive();

    }
    
    
}