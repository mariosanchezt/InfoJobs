#include <SFML/Graphics.hpp>
#include "Juego.h"
#include "Renderer.h"
#include <iostream>

int main() {
    sf::RenderWindow ventana(sf::VideoMode({ 800, 800 }), "Archon PvZ");
    ventana.setFramerateLimit(60);

    Juego juego;
    juego.inicializarPartida();

    Renderer renderer(ventana);

    if (!renderer.cargarFuente("C:/Windows/Fonts/arial.ttf")) {
        std::cout << "Fuente no encontrada, el juego corre sin texto" << std::endl;
    }

    while (ventana.isOpen()) {
        while (auto event = ventana.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                ventana.close();
            }

            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    int fila, col;
                    if (renderer.pixelACasilla(click->position.x, click->position.y, fila, col)) {
                        int filaOrigen = renderer.getFilaSeleccionada();
                        int colOrigen = renderer.getColSeleccionada();

                        if (filaOrigen == -1) {
                            // Nada seleccionado: seleccionar solo si la pieza es del turno actual
                            Pieza* p = juego.getTablero()->getPieza(fila, col);
                            if (p != nullptr && p->getBando() == juego.getTurnoActual()) {
                                renderer.seleccionarCasilla(fila, col, juego.getTablero());
                            }
                        }
                        else if (fila == filaOrigen && col == colOrigen) {
                            // Clic en la misma casilla: deseleccionar
                            renderer.deseleccionar();
                        }
                        else {
                            // Segunda casilla: intentar mover (el turno cambia dentro de moverPieza solo si es valido)
                            juego.moverPieza(filaOrigen, colOrigen, fila, col);
                            renderer.deseleccionar();
                        }
                    }
                }

                // Click derecho: cancelar seleccion
                if (click->button == sf::Mouse::Button::Right) {
                    renderer.deseleccionar();
                }
            }
        }

        ventana.clear(sf::Color(20, 20, 20));
        renderer.dibujarEstadoTablero(juego.getTablero(), juego.getTurnoActual());
        ventana.display();
    }

    return 0;
}