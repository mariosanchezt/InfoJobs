#pragma once
#include "Pieza.h"

class Arena {
private:
    Pieza* pieza1; //la pieza q ataca
    Pieza* pieza2; //la pieza q defiende

public:
    //constructor: la arena se crea vacia, sin piezas
    Arena();

    //lanza el combate recibiendo las dos piezas q van a pelear
    //devuelve quien gana (nullptr si mueren las dos)
    Pieza* iniciarCombate(Pieza* p1, Pieza* p2);

    //Funcion para dibujar la arena 
    void dibujar();
};
