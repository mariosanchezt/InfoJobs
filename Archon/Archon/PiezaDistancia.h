#pragma once
#include "Pieza.h"

// Lanzaguisantes (P) o Minero (Z)
class PiezaDistancia : public Pieza {
public:
    float alcance;

    PiezaDistancia(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t);

    void atacar(Pieza* enemigo) override;
    void dibujar() override;
};