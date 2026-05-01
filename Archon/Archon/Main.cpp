#include <SFML/Graphics.hpp>
#include "Juego.h"
#include "Renderer.h"
#include "Menu.h"
#include <iostream>

// Muestra la pantalla de carga durante 'segundos' segundos
void mostrarPantallaCarga(sf::RenderWindow& ventana, sf::Font& fuente, float segundos) {
    sf::Texture texCarga;
    bool cargada = texCarga.loadFromFile("assets/loading_bg.png");
    texCarga.setSmooth(true);

    sf::Clock temporizador;
    while (temporizador.getElapsedTime().asSeconds() < segundos && ventana.isOpen()) {
        // Procesamos eventos pa q la ventana no se congele
        while (auto event = ventana.pollEvent()) {
            if (event->is<sf::Event::Closed>()) ventana.close();
        }

        ventana.clear(sf::Color(10, 10, 20));

        if (cargada) {
            sf::Sprite sprite(texCarga);
            sf::Vector2u texSize = texCarga.getSize();
            float escalaX = 800.f / texSize.x;
            float escalaY = 800.f / texSize.y;
            sprite.setScale(sf::Vector2f(escalaX, escalaY));
            ventana.draw(sprite);

            // Overlay oscuro suave
            sf::RectangleShape overlay(sf::Vector2f(800.f, 800.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 60));
            ventana.draw(overlay);
        }

        // Texto "Cargando..."
        sf::Text texto(fuente, "Cargando...", 28);
        texto.setFillColor(sf::Color::White);
        texto.setStyle(sf::Text::Bold);
        sf::FloatRect b = texto.getLocalBounds();
        texto.setPosition(sf::Vector2f(400.f - b.size.x / 2.f, 750.f));
        ventana.draw(texto);

        ventana.display();
    }
}

int main() {
    sf::RenderWindow ventana(sf::VideoMode({ 800, 800 }), "Archon PvZ");
    ventana.setFramerateLimit(60);

    sf::Font fuente;
    if (!fuente.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cout << "Fuente no encontrada" << std::endl;
    }

    EstadoJuego estadoJuego = EstadoJuego::MENU;
    Menu menu(ventana, fuente);

    Juego* juego = nullptr;
    Renderer* renderer = nullptr;

    while (ventana.isOpen()) {

        // MENU
        if (estadoJuego == EstadoJuego::MENU) {
            while (auto event = ventana.pollEvent()) {
                if (event->is<sf::Event::Closed>()) ventana.close();

                EstadoJuego resultado = menu.procesarEvento(*event);
                if (resultado != EstadoJuego::MENU) {
                    // Pantalla de carga antes de iniciar
                    mostrarPantallaCarga(ventana, fuente, 2.0f);

                    estadoJuego = EstadoJuego::JUGANDO_LOCAL;

                    delete juego;
                    delete renderer;

                    juego = new Juego();
                    renderer = new Renderer(ventana);
                    renderer->cargarFuente("C:/Windows/Fonts/arial.ttf");
                    renderer->cargarSprites("assets");
                    juego->inicializarPartida();
                }
            }
            menu.dibujar();
            ventana.display();
            continue;
        }

        // JUGANDO
        while (auto event = ventana.pollEvent()) {
            if (event->is<sf::Event::Closed>()) ventana.close();

            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    int fila, col;
                    if (renderer->pixelACasilla(click->position.x, click->position.y, fila, col)) {
                        int filaOrigen = renderer->getFilaSeleccionada();
                        int colOrigen = renderer->getColSeleccionada();

                        if (filaOrigen == -1) {
                            Pieza* p = juego->getTablero()->getPieza(fila, col);
                            if (p != nullptr && p->getBando() == juego->getTurnoActual()) {
                                renderer->seleccionarCasilla(fila, col, juego->getTablero());
                            }
                        }
                        else if (fila == filaOrigen && col == colOrigen) {
                            renderer->deseleccionar();
                        }
                        else {
                            juego->moverPieza(filaOrigen, colOrigen, fila, col);
                            renderer->deseleccionar();
                        }
                    }
                }
                if (click->button == sf::Mouse::Button::Right) {
                    renderer->deseleccionar();
                }
            }

            // Escape vuelve al menu
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    estadoJuego = EstadoJuego::MENU;
                }
            }
        }

        ventana.clear(sf::Color(20, 20, 20));
        renderer->dibujarEstadoTablero(juego->getTablero(), juego->getTurnoActual());
        ventana.display();
    }

    delete juego;
    delete renderer;
    return 0;
}