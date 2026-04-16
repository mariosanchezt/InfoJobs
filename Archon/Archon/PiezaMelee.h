#pragma once
#include "Pieza.h"

class PiezaMelee : public Pieza {
public:
    PiezaMelee(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t); //Definicion del constructor
    void atacar(Pieza* p) override; //Definicion del atributo atacar
    void dibujar() override;// Definicion del metodo atacar
};
