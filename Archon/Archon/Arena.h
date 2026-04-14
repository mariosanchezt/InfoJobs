#pragma once
#include "Pieza.h"

class Arena {
private:  
    Pieza* pieza1; //la pieza q ataca
    Pieza* pieza2; //la pieza q defiende

public:
    //constructor: recibe las dos piezas q van a pelear
    Arena(Pieza* p1, Pieza* p2);

    //lanza el combate y devuelve quien gana (nullptr si mueren las dos)
    Pieza* iniciarCombate();

    //Funcion para dibujar la arena 
    void dibujar();

};
