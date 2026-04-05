#pragma once
#include "Pieza.h"
#include <iostream>

//El guerero hereda de la clase pieza
class PiezaMelee : public Pieza {
public:
	//Constructor con los valores tipicos de piezas a melee
	PiezaMelee(int fIni, int cIni, TipoMovimiento t)
		: Pieza(100.0f, 15.0f, 1.0f, 0.5f, 1, fIni, cIni, t) {
		//Valores de ejemplo
	}

	//Implementacion ataque
	void atacar() override {
		std::cout << "Ataque cuerpo a cuerpo realizado" << std::endl;
	}
};