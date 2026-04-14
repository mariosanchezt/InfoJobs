#pragma once
#include "Pieza.h"

class PiezaRapida : public Pieza {
public:
    PiezaRapida(Bando b, int fIni, int cIni);// Definicion del constructor
    void atacar(Pieza* enemigo) override;// Definicion del metodo atacar
};