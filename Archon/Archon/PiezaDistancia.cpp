#include "PiezaDistancia.h"
#include <iostream>

//deifnicion constructor
PiezaDistancia::PiezaDistancia(Bando b, int fIni, int cIni) 
    : Pieza(
        (b==LUZ ? "Lanzaguisantes" : "Soldado Z"), //nombre
        b,                                          //bando
        (b==LUZ ? 100.0f : 100.0f),                 //vida
        (b==LUZ ? 25.0f, 22.0f),                    //fuerza
        0.8f, 0.8f, 4,                              //stats fijas
        fIni,cIni,                                  //pos ini
        GROUND                                      //tipo movimiento
    )
{
    alcance = (b == LUZ ? 5.0f : 7.0f);

}

//definicion del ataque
void PiezaDistancia::atacar(Pieza* enemigo) {
	std::cout << "Disparando proyectil a una distancia de: " << alcance 
		<< "contra: "<< enemigo->getNombre() << std::endl;
}
void PiezaDistancia::dibujar() {
	std::cout << "Dibujando pieza a distancia: " << nombre << std::endl;
}