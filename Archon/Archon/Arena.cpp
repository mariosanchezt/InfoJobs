#include "Arena.h"
#include <iostream>

Arena::Arena() {
    pieza1 = nullptr;
    pieza2 = nullptr;
}

Pieza* Arena::iniciarCombate(Pieza* p1, Pieza* p2) {
    //guardamos las piezas q van a pelear
    pieza1 = p1;
    pieza2 = p2;

    std::cout << "=== COMIENZA EL COMBATE ===" << std::endl;
    std::cout << pieza1->getNombre() << " vs " << pieza2->getNombre() << std::endl;

    //bucle de combate, se van pegando hasta q alguno muera
    while (pieza1->vida > 0 && pieza2->vida > 0) {

        //pieza1 ataca
        pieza1->atacar(pieza2);

        //si pieza2 sigue viva contraataca
        if (pieza2->vida > 0) {
            pieza2->atacar(pieza1);
        }
    }

    std::cout << "=== FIN DEL COMBATE ===" << std::endl;

    //miramos quien ha ganado
    if (pieza1->vida <= 0 && pieza2->vida <= 0) {
        std::cout << "Han muerto las dos piezas" << std::endl;
        //limpiamos la arena
        pieza1 = nullptr;
        pieza2 = nullptr;
        return nullptr;
    }
    else if (pieza1->vida > 0) {
        std::cout << "Gana " << pieza1->getNombre() << std::endl;
        Pieza* ganador = pieza1;
        //limpiamos la arena
        pieza1 = nullptr;
        pieza2 = nullptr;
        return ganador;
    }
    else {
        std::cout << "Gana " << pieza2->getNombre() << std::endl;
        Pieza* ganador = pieza2;
        //limpiamos la arena
        pieza1 = nullptr;
        pieza2 = nullptr;
        return ganador;
    }
}

void Arena::dibujar() {
    std::cout << "Dibujando arena de combate..." << std::endl;
}
