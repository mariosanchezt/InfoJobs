#pragma once
#include "Pieza.h"

class PiezaTanque : public Pieza {
public:
    PiezaTanque(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t);// Definicion del constructor
    void atacar(Pieza* enemigo) override;// Definicion del metodo atacar
    void dibujar() override;// Definicion del metodo atacar
};