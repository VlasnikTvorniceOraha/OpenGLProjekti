#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 Normala;

out vec3 color;

uniform mat4 perspektiva;
uniform mat4 pogled;
uniform mat4 model;

uniform vec3 pozicijaSvjetla;
uniform vec3 svjetlost;

uniform vec3 ambijentalna;
uniform vec3 difuzna;
uniform vec3 spekularna;
uniform float kolicinaSjaja;

uniform vec3 pozicijaKamere;

void main()
{

    vec4 pozicijaTocke = model * vec4(aPos, 1.0);

    vec3 eVektor = vec3(normalize(pozicijaKamere.xyz - pozicijaTocke.xyz));

    vec3 sVektor = vec3(normalize(pozicijaSvjetla.xyz - pozicijaTocke.xyz));
    

    vec3 reflektiraniVektor = reflect(-sVektor, Normala);


    vec3 ambientBoja = ambijentalna * svjetlost;

    vec3 difuznaBoja =  svjetlost * difuzna * max(0.0, dot(Normala, sVektor));

    vec3 spekularnaBoja = svjetlost * spekularna * pow( max(0.0, dot(eVektor, reflektiraniVektor)), kolicinaSjaja);

    
    color = ambientBoja + difuznaBoja + spekularnaBoja;







    gl_Position = perspektiva * pogled * pozicijaTocke;
}