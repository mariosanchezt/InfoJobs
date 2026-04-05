#include "PiezaMelee.h"
#include <iostream>


PiezaMelee::PiezaMelee(int fIni, int cIni, TipoMovimiento t)
    : Pieza(100.0f, 15.0f, 1.0f, 0.5f, 1, fIni, cIni, t) {
    //Constructor PiezaMelee
}

//Atributo atacar de PiezaMelee
void PiezaMelee::atacar() {
    std::cout << "Ataque cuerpo a cuerpo realizado" << std::endl;
}