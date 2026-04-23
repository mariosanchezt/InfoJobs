#include "PiezaDistancia.h"
#include <iostream>

//deifnicion constructor
PiezaDistancia::PiezaDistancia(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(
        (b == LUZ ? "Lanzaguisantes" : "Soldado"), // Usamos el nombre según bando
        b,                                         // Bando (LUZ u OSCURIDAD)
        100.0f,                                    // Vida
        (b == LUZ ? 25.0f : 22.0f),                // Fuerza (Lanzaguisantes pega +)
        0.8f, 0.8f, 4,                             // Stats fijas (velocidad, recarga, radio mov)
        fIni, cIni,                                // Posición inicial
        t                                   //tipo movimiento
    )
{
    alcance = (b == LUZ ? 5.0f : 7.0f);

}

//definicion del ataque
void PiezaDistancia::atacar(Pieza* enemigo) {
	if (enemigo != nullptr) {
		std::cout << nombre << "dispara un proyectil (Alcance:" << alcance << ") contra" << enemigo->getNombre() << "!" << std::endl;

		//Resta la vida real
		enemigo->vida -= this->fuerza;
		if (enemigo->vida < 0)enemigo->vida = 0;//Evita que la vida sea negativa

		std::cout << "Vida restante de " << enemigo->getNombre() << ": " << enemigo->vida << std::endl;
	}
}
void PiezaDistancia::dibujar() {
	std::cout << "Dibujando pieza a distancia: " << nombre << std::endl;
}