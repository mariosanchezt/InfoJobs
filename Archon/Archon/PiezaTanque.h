#pragma once
#include "Pieza.h"

class PiezaTanque : public Pieza {
public:
    PiezaTanque(int fIni, int cIni, TipoMovimiento t); // Definicion del constructor
    void atacar() override; // Definicion del metodo atacar
};