#version 330 core

out vec4 FragColor;

in vec3 normala;

in vec3 pozicijaFragmenta;

in vec2 texKordinate;


uniform vec3 pozicijaSvjetla;
uniform vec3 svjetlost;

uniform vec3 ambijentalna;
uniform vec3 difuzna;
uniform vec3 spekularna;
uniform float kolicinaSjaja;

uniform vec3 pozicijaKamere;

uniform sampler2D Tekstura;


void main()
{

    vec3 eVektor = vec3(normalize(pozicijaKamere.xyz - pozicijaFragmenta.xyz));

    vec3 sVektor = vec3(normalize(pozicijaSvjetla.xyz - pozicijaFragmenta.xyz));
    

    vec3 reflektiraniVektor = reflect(-sVektor, normala);


    vec3 ambientBoja = ambijentalna * svjetlost;

    //sampleanje boje iz teksture umjesto materijala
    vec4 sampleBoja = texture(Tekstura, texKordinate);

    //vec3 difuznaBoja =  svjetlost * difuzna * max(0.0, dot(normala, sVektor));

    vec3 difuznaBoja =  svjetlost * sampleBoja.xyz * max(0.0, dot(normala, sVektor));

    vec3 spekularnaBoja = svjetlost * spekularna * pow( max(0.0, dot(eVektor, reflektiraniVektor)), kolicinaSjaja);

    
    vec3 fColor = ambientBoja + difuznaBoja + spekularnaBoja;

    FragColor = vec4(fColor, 0.0);
} 