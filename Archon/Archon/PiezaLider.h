#pragma once
#include "Pieza.h"

class PiezaLider : public Pieza {
public:
    int hechizosDisponibles;

    PiezaLider(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t);

    void atacar(Pieza* enemigo) override;
    void dibujar() override;

    void lanzarHechizo();
};