#pragma once
#include "Pieza.h"

class PiezaDistancia : public Pieza {
public:
	float alcance;
	//creamos constructor q recibe datos Pieza y añadimos cualidad: ALCANCE
	PiezaDistancia(float v, float f, float va, float intA, int rm, int fIni, int cIni, TipoMovimiento t, float alc);
	
	void atacar() override; 

};