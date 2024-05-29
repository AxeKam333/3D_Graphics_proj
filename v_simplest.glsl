#version 330

//Zmienne jednorodne
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec2 lights;

//Atrybuty
in vec4 vertex; //wspolrzedne wierzcholka w przestrzeni modelu
in vec4 color; //kolor związany z wierzchołkiem
in vec4 normal; //wektor normalny w przestrzeni modelu
in vec2 texCoord0;

//Zmienne interpolowane
out vec4 ic;
out vec4 l1;
out vec4 l2;
out vec4 n;
out vec4 v;
out vec2 iTexCoord0; 
out vec2 iTexCoord1;

void main(void) {
    if (lights.x == 1) {
        vec4 lp1 = vec4(5, 1, 6, 0); //pozcyja 1 światła, przestrzeń świata
        l1 = normalize(V * lp1 - V*M*vertex); //wektor do światła w przestrzeni oka
    }
    else
    {
        l1 = vec4(0, 0, 0, 0);
    } 
    if (lights.y == 1) {
        vec4 lp2 = vec4(-6, 2, -6, 0); //pozcyja 2 światła, przestrzeń świata
        l2 = normalize(V * lp2 - V*M*vertex); //wektor do światła w przestrzeni oka
    }
    else
    {
        l2 = vec4(0, 0, 0, 0);
    }


    v = normalize(vec4(0, 0, 0, 1) - V * M * vertex); //wektor do obserwatora w przestrzeni oka
    n = normalize(V * M * normal); //wektor normalny w przestrzeni oka
    
    iTexCoord0 = texCoord0;
    iTexCoord1 = (n.xy + 1) / 2;

    ic = color;
    
    gl_Position=P*V*M*vertex;
}
