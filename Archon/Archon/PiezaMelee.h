#pragma once
#include "Pieza.h"

class PiezaMelee : public Pieza {
public:
    PiezaMelee(int fIni, int cIni, TipoMovimiento t); //Definicion del constructor
    void atacar() override; //Definicion del atributo atacar
};