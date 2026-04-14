#pragma once
#include "Pieza.h"

class PiezaTanque : public Pieza {
public:
    PiezaTanque(Bando b, int fIni, int cIni);// Definicion del constructor
    void atacar(Pieza* enemigo) override;// Definicion del metodo atacar
};