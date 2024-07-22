#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out FS {
    vec3 fColor;
} buraz;

uniform mat4 perspektiva;
uniform mat4 pogled;

uniform vec3 pozicijaSvjetla;
uniform vec3 svjetlost;

uniform vec3 ambijentalna;
uniform vec3 difuzna;
uniform vec3 spekularna;
uniform float kolicinaSjaja;

uniform vec3 pozicijaKamere;


void main() {

    

    vec3 centarTrokuta = vec3((gl_in[0].gl_Position.x + gl_in[1].gl_Position.x + gl_in[2].gl_Position.x) / 3.0,
                              (gl_in[0].gl_Position.y + gl_in[1].gl_Position.y + gl_in[2].gl_Position.y) / 3.0,
                              (gl_in[0].gl_Position.z + gl_in[1].gl_Position.z + gl_in[2].gl_Position.z) / 3.0);

    vec3 eVektor = vec3(normalize(pozicijaKamere.xyz - centarTrokuta.xyz));

    vec3 sVektor = vec3(normalize(pozicijaSvjetla.xyz - centarTrokuta.xyz));
    

    vec3 brid1 = vec3(gl_in[1].gl_Position.x - gl_in[0].gl_Position.x,
                      gl_in[1].gl_Position.y - gl_in[0].gl_Position.y,
                      gl_in[1].gl_Position.z - gl_in[0].gl_Position.z);

    vec3 brid2 = vec3(gl_in[2].gl_Position.x - gl_in[0].gl_Position.x,
                      gl_in[2].gl_Position.y - gl_in[0].gl_Position.y,
                      gl_in[2].gl_Position.z - gl_in[0].gl_Position.z);

    vec3 normala = normalize(cross(brid1, brid2));

    vec3 reflektiraniVektor = reflect(-sVektor, normala);


    vec3 ambientBoja = ambijentalna * svjetlost;

    vec3 difuznaBoja =  svjetlost * difuzna * max(0.0, dot(normala, sVektor));

    vec3 spekularnaBoja = svjetlost * spekularna * pow( max(0.0, dot(eVektor, reflektiraniVektor)), kolicinaSjaja);

    
    buraz.fColor = ambientBoja + difuznaBoja + spekularnaBoja;

    gl_Position = perspektiva * pogled * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = perspektiva * pogled * gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = perspektiva * pogled * gl_in[2].gl_Position;
    EmitVertex();
    
    EndPrimitive();


    

    

}