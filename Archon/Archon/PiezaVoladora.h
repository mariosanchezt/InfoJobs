#pragma once
#include "Pieza.h"

class PiezaVoladora : public Pieza {
public:
    bool modoBolaDeFuego; //para la habilidad especial

    PiezaVoladora(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t);// Definicion del constructor
    void atacar(Pieza* enemigo) override;// Definicion del metodo atacar
    void activarBolaDeFuego();
    void desactivarBolaDeFuego();
    void dibujar() override;
};