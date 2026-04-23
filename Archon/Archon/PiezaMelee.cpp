#include "PiezaMelee.h"
#include <iostream>


PiezaMelee::PiezaMelee(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t)
    : Pieza(n, b, 100.0f, 15.0f, 1.0f, 0.5f, 1, fIni, cIni, t) {
    //Constructor PiezaMelee
}

//Atributo atacar de PiezaMelee
void PiezaMelee::atacar(Pieza* enemigo) {
    if (enemigo != nullptr) {
        std::cout << nombre << " realiza un ataque cuerpo a cuerpo contra " << enemigo->getNombre() << std::endl;

        // Lógica de daño real: restamos tu fuerza a su vida
        enemigo->vida -= this->fuerza;
        
        if (enemigo->vida < 0) enemigo->vida = 0;//Evita que la vida sea negativa
        std::cout << "Vida restante de " << enemigo->getNombre() << ": " << enemigo->vida << std::endl;
    }
}

void PiezaMelee::dibujar() {
    std::cout << "Dibujando pieza melee: " << nombre << std::endl;
}