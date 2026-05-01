#include "Tablero.h"
#include <iostream>
#include <cstdlib>

Tablero::Tablero() {
    ciclo = 1;             // Inicializamos el ciclo actual
    cicloOscilacion = 0;   // Contador de pasos para el ciclo

    // Inicializamos todo el tablero a nullptr (vacío)
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            casillas[i][j] = nullptr;
        }
    }
}

Pieza* Tablero::getPieza(int fila, int col) {
    if (fila >= 0 && fila < 9 && col >= 0 && col < 9) {
        return casillas[fila][col];
    }
    return nullptr;
}

void Tablero::colocarPieza(int fila, int col, Pieza* p) {
    if (fila >= 0 && fila < 9 && col >= 0 && col < 9) {
        casillas[fila][col] = p;
    }
}

bool Tablero::esMovimientoValido(int fOrigen, int cOrigen, int fDestino, int cDestino) {
    // 1. Comprobar que el origen esté dentro del tablero
    if (fOrigen < 0 || fOrigen >= 9 || cOrigen < 0 || cOrigen >= 9) {
        std::cout << "Movimiento invalido: origen fuera del tablero." << std::endl;
        return false;
    }

    // 2. Comprobar que el destino esté dentro del tablero
    if (fDestino < 0 || fDestino >= 9 || cDestino < 0 || cDestino >= 9) {
        std::cout << "Movimiento invalido: destino fuera del tablero." << std::endl;
        return false;
    }

    // 3. Comprobar que hay una pieza en el origen
    Pieza* p = getPieza(fOrigen, cOrigen);

    if (p == nullptr) {
        std::cout << "Movimiento invalido: no hay pieza en la casilla de origen." << std::endl;
        return false;
    }

    // 4. Calcular distancia entre origen y destino
    int distanciaFilas = abs(fDestino - fOrigen);
    int distanciaColumnas = abs(cDestino - cOrigen);

    int distanciaTotal = distanciaFilas + distanciaColumnas;

    // 5. Comprobar que no supera el radio de movimiento de la pieza
    if (distanciaTotal > p->radioMovimiento) {
        std::cout << "Movimiento invalido: supera el radio de movimiento de "
            << p->getNombre() << "." << std::endl;
        return false;
    }

    // 6. Teleport: puede moverse a cualquier casilla
    if (p->mov == TELEPORT) {
        return true;
    }

    // 7. FLYING: puede saltar piezas (solo comprueba el radio, que ya se comprueba en 6)
    if (p->mov == FLYING) {
        return true;
    }

    // 8. GROUND: no puede saltarse piezas ni moverse en diagonal
    if (distanciaFilas > 0 && distanciaColumnas > 0) {
        std::cout << "Movimiento invalido: " << p->getNombre() << " no puede moverse en diagonal." << std::endl;
        return false;
    }

    int stepFila = 0;
    int stepCol = 0;
    if (distanciaFilas > 0) stepFila = (fDestino > fOrigen) ? 1 : -1;
    if (distanciaColumnas > 0)stepCol = (cDestino > cOrigen) ? 1 : -1;
    int f = fOrigen + stepFila;
    int c = cOrigen + stepCol;
    while (f != fDestino || c != cDestino) {
        if (casillas[f][c] != nullptr) {
            std::cout << "Movimiento invalido: " << p->getNombre() << " tiene el camino bloqueado." << std::endl;
            return false;
        }
        f += stepFila;
        c += stepCol;
    }
    return true;
}

void Tablero::moverPieza(int fOrigen, int cOrigen, int fDestino, int cDestino) {
    if (!esMovimientoValido(fOrigen, cOrigen, fDestino, cDestino)) {
        return;
    }

    Pieza* p = getPieza(fOrigen, cOrigen);

    colocarPieza(fDestino, cDestino, p);
    casillas[fOrigen][cOrigen] = nullptr;

    // Actualizamos la posicion interna de la pieza
    p->filaInicial = fDestino;
    p->colInicial = cDestino;

    std::cout << p->getNombre() << " se ha movido a la casilla ("
        << fDestino << ", " << cDestino << ")." << std::endl;
}

bool Tablero::esPuntoDePoder(int f, int c) {
    // El tablero es de 9x9 y tiene 5 puntos de poder [cite: 136]
    // 1. Centro del tablero (4,4) [cite: 136]
    if (f == 4 && c == 4) return true;

    // 2. Centros de cada borde: (0,4), (8,4), (4,0), (4,8) [cite: 136]
    if ((f == 0 && c == 4) || (f == 8 && c == 4) ||
        (f == 4 && c == 0) || (f == 4 && c == 8)) {
        return true;
    }
    return false;
}

// Esta función la llamará el 'Juego' cada vez que termine un turno completo
void Tablero::avanzarCiclo() {
    // El ciclo va del 0 al 6 (representando: Blanco -> Gris -> Negro -> Gris -> Blanco...)
    ciclo++;
    if (ciclo > 6) ciclo = 0;
}

// Devuelve: 0 para Blanco (Luz), 1 para Negro (Oscuridad), 2 para Neutral (Gris)
int Tablero::getColorActual(int f, int c) {
    // 1. Identificamos si esta casilla es una de las que "oscila" (1/3 del tablero) 
    // Usamos una fórmula matemática para elegir 1 de cada 3 casillas de forma repartida
    if ((f * 9 + c) % 3 == 0) {
        // Estas casillas cambian según la variable 'ciclo'
        if (ciclo == 0 || ciclo == 6) return 0; // Blanco puro
        if (ciclo == 1 || ciclo == 5) return 2; // Gris (tirando a blanco)
        if (ciclo == 2 || ciclo == 4) return 2; // Gris (tirando a negro)
        if (ciclo == 3) return 1;               // Negro puro
    }

    // 2. Si no es oscilante, es una casilla fija tipo ajedrez [cite: 109, 136]
    return (f + c) % 2 == 0 ? 0 : 1;
}

void Tablero::dibujar() {
    // Implementación básica para ver algo por consola
    std::cout << "  0 1 2 3 4 5 6 7 8" << std::endl;
    for (int i = 0; i < 9; i++) {
        std::cout << i << " ";
        for (int j = 0; j < 9; j++) {
            if (casillas[i][j] == nullptr) {
                // Dibujamos un punto si está vacío
                std::cout << ". ";
            }
            else {
                // Llamamos al dibujar de la pieza (polimorfismo)
                casillas[i][j]->dibujar();
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}