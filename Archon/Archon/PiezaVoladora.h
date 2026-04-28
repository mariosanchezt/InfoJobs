#pragma once
#include "Pieza.h"

class PiezaVoladora : public Pieza {
public:
    bool modoBolaDeFuego;

    PiezaVoladora(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t);

    void atacar(Pieza* enemigo) override;
    void dibujar() override;

    void activarBolaDeFuego();
    void desactivarBolaDeFuego();
};