#pragma once
#include "Pieza.h"

//Lanzaguisantes (P) o Soldado (Z)
class PiezaDistancia : public Pieza {
public:
	float alcance;
	//creamos constructor q recibe datos Pieza y añadimos cualidad: ALCANCE
	//PiezaDistancia(float v, float f, float va, float intA, int rm, int fIni, int cIni, TipoMovimiento t, float alc);

	PiezaDistancia(std::string n, Bando b, int fIni, int cIni, TipoMovimiento t);

	void atacar(Pieza* enemigo) override;
	void dibujar() override;
};