#include "Arena.h"
#include <iostream>

Arena::Arena(Pieza* p1, Pieza* p2) {
    pieza1 = p1;
    pieza2 = p2;
}

Pieza* Arena::iniciarCombate() {
    std::cout << "=== COMIENZA EL COMBATE ===" << std::endl;

    //bucle de combate, se van pegando hasta q alguno muera
    while (pieza1->vida > 0 && pieza2->vida > 0) {

        //pieza1 ataca
        float dano1 = pieza1->fuerza * pieza1->velAtaque;
        pieza2->vida -= dano1;
        pieza1->atacar();

        //si pieza2 sigue viva contraataca
        if (pieza2->vida > 0) {
            float dano2 = pieza2->fuerza * pieza2->velAtaque;
            pieza1->vida -= dano2;
            pieza2->atacar();
        }
    }

    std::cout << "=== FIN DEL COMBATE ===" << std::endl;

    //miramos quien ha ganado
    if (pieza1->vida <= 0 && pieza2->vida <= 0) {
        std::cout << "Han muerto las dos piezas" << std::endl;
        return nullptr;
    }
    else if (pieza1->vida > 0) {
        std::cout << "Gana pieza 1" << std::endl;
        return pieza1;
    }
    else {
        std::cout << "Gana pieza 2" << std::endl;
        return pieza2;
    }
}
