#include "PiezaDistancia.h"
#include <iostream>

//deifnicion constructor
PiezaDistancia::PiezaDistancia(std::string n, Bando b, float v, float f, float va, float intA, int rm, int fIni, int cIni, TipoMovimiento t, float alc)
	: Pieza(n, b, v, f, va, intA, rm, fIni, cIni, t)//llamamos a constructor del padre
{
	alcance = alc;

}

//definicion del ataque
void PiezaDistancia::atacar(Pieza* enemigo) {
	std::cout << "Disparando proyectil a una distancia de: " << alcance
		<< "contra: " << enemigo->getNombre() << std::endl;
}
void PiezaDistancia::dibujar() {
	std::cout << "Dibujando pieza a distancia: " << nombre << std::endl;
}