#pragma once
#include "Pieza.h"

class PiezaVoladora : public Pieza {
public:
    bool modoBolaDeFuego; //para la habilidad especial

    PiezaVoladora(int fIni, int cIni); //constructor, siempre vuela asi q no hace falta pasar tipo
    void atacar() override;
    void activarBolaDeFuego();
    void desactivarBolaDeFuego();
};
