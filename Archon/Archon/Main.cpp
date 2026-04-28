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

            
            /*
            HUGO: aqui va la logica de clicks del raton
             Tienes disponibles estos metodos del Renderer:
             renderer.pixelACasilla(x, y, fila, col) -> bool
             renderer.seleccionarCasilla(fila, col)
             renderer.deseleccionar()
             renderer.getFilaSeleccionada() / getColSeleccionada()
             renderer.setEstado(ARENA) para cambiar a pantalla de combate
             Y del Juego:
             juego.moverPieza(fOrigen, cOrigen, fDestino, cDestino)
             */ 
            
        }

        ventana.clear(sf::Color(20, 20, 20));
        renderer.dibujarEstadoTablero(juego.getTablero());
        ventana.display();
    }

    return 0;
}