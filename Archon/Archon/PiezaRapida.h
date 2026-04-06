#pragma once
#include "Pieza.h"

class PiezaRapida : public Pieza {
public:
    PiezaRapida(int fIni, int cIni, TipoMovimiento t); // Definicion del constructor
    void atacar() override; // Definicion del metodo atacar
};