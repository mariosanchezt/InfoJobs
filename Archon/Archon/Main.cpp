#include <SFML/Graphics.hpp>
#include "Juego.h"
#include "Renderer.h"
#include "Arena.h"
#include "Menu.h"
#include <iostream>

void mostrarPantallaCarga(sf::RenderWindow& ventana, sf::Font& fuente, float segundos) {
    sf::Texture texCarga;
    bool cargada = texCarga.loadFromFile("assets/loading_bg.png");
    texCarga.setSmooth(true);

    sf::Clock temporizador;
    while (temporizador.getElapsedTime().asSeconds() < segundos && ventana.isOpen()) {
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

            sf::RectangleShape overlay(sf::Vector2f(800.f, 800.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 60));
            ventana.draw(overlay);
        }

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
    if (!fuente.openFromFile("assets/SamdanEvil.ttf")) {
        std::cout << "Fuente no encontrada" << std::endl;
    }

    EstadoJuego estadoJuego = EstadoJuego::MENU;
    Menu menu(ventana, fuente);

    Juego* juego = nullptr;
    Renderer* renderer = nullptr;
    Arena* arena = nullptr;

    bool enCombate = false;
    int filaAtacante = -1, colAtacante = -1;
    int filaDefensor = -1, colDefensor = -1;

    sf::Clock reloj;

    while (ventana.isOpen()) {
        float dt = reloj.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        // PANTALLA DE VICTORIA
        if (estadoJuego == EstadoJuego::VICTORIA) {
            while (auto event = ventana.pollEvent()) {
                if (event->is<sf::Event::Closed>()) ventana.close();

                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::Enter ||
                        key->code == sf::Keyboard::Key::Escape)
                        estadoJuego = EstadoJuego::MENU;
                }
                if (event->is<sf::Event::MouseButtonPressed>())
                    estadoJuego = EstadoJuego::MENU;
            }

            ventana.clear(sf::Color(10, 10, 20));
            renderer->dibujarPantallaVictoria(juego->getBandoGanador());
            ventana.display();
            continue;
        }

        // MODO MENU
        if (estadoJuego == EstadoJuego::MENU) {
            while (auto event = ventana.pollEvent()) {
                if (event->is<sf::Event::Closed>()) ventana.close();

                EstadoJuego resultado = menu.procesarEvento(*event);
                if (resultado != EstadoJuego::MENU) {
                    mostrarPantallaCarga(ventana, fuente, 2.0f);

                    estadoJuego = EstadoJuego::JUGANDO_LOCAL;

                    delete juego;
                    delete renderer;
                    delete arena;

                    juego = new Juego();
                    renderer = new Renderer(ventana);
                    arena = new Arena();

                    renderer->cargarFuente("assets/SamdanEvil.ttf");
                    renderer->cargarSprites("assets");
                    juego->inicializarPartida();
                }
            }

            ventana.clear(sf::Color(15, 15, 25));
            menu.dibujar();
            ventana.display();
            continue;
        }

        // MODO ARENA
        if (enCombate) {
            while (auto event = ventana.pollEvent()) {
                if (event->is<sf::Event::Closed>()) ventana.close();
            }

            arena->update(dt);
            ventana.clear(sf::Color(20, 20, 20));

            if (arena->haTerminado()) {
                enCombate = false;
                renderer->setEstado(TABLERO);

                Pieza* ganador = arena->getGanador();
                Pieza* atacante = juego->getTablero()->getPieza(filaAtacante, colAtacante);
                Pieza* defensor = juego->getTablero()->getPieza(filaDefensor, colDefensor);

                juego->getTablero()->colocarPieza(filaAtacante, colAtacante, nullptr);
                juego->getTablero()->colocarPieza(filaDefensor, colDefensor, nullptr);

                arena->limpiar();

                if (ganador == nullptr) {
                    delete atacante; delete defensor;
                }
                else if (ganador == atacante) {
                    juego->getTablero()->colocarPieza(filaDefensor, colDefensor, atacante);
                    atacante->filaInicial = filaDefensor;
                    atacante->colInicial = colDefensor;
                    delete defensor;
                }
                else {
                    juego->getTablero()->colocarPieza(filaDefensor, colDefensor, defensor);
                    delete atacante;
                }

                juego->cambiarTurno();
                if (juego->verificarVictoria())
                    estadoJuego = EstadoJuego::VICTORIA;
            }
            else {
                renderer->dibujarEstadoArena(*arena);
            }

            ventana.display();
            continue;
        }

        // MODO TABLERO
        while (auto event = ventana.pollEvent()) {
            if (event->is<sf::Event::Closed>()) ventana.close();

            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    int fila, col;
                    if (renderer->pixelACasilla(click->position.x, click->position.y, fila, col)) {
                        int fSel = renderer->getFilaSeleccionada();
                        int cSel = renderer->getColSeleccionada();

                        if (fSel == -1) {
                            Pieza* p = juego->getTablero()->getPieza(fila, col);
                            if (p && p->getBando() == juego->getTurnoActual())
                                renderer->seleccionarCasilla(fila, col, juego->getTablero());
                        }
                        else if (fila == fSel && col == cSel) {
                            renderer->deseleccionar();
                        }
                        else {
                            Pieza* p = juego->getTablero()->getPieza(fSel, cSel);
                            Pieza* ocupante = juego->getTablero()->getPieza(fila, col);

                            if (ocupante != nullptr && p != nullptr && ocupante->getBando() != p->getBando()) {
                                filaAtacante = fSel; colAtacante = cSel;
                                filaDefensor = fila; colDefensor = col;
                                arena->iniciarCombate(p, ocupante);
                                enCombate = true;
                                renderer->setEstado(ARENA);
                            }
                            else {
                                juego->moverPieza(fSel, cSel, fila, col);
                                if (juego->verificarVictoria())
                                    estadoJuego = EstadoJuego::VICTORIA;
                            }
                            renderer->deseleccionar();
                        }
                    }
                }

                if (click->button == sf::Mouse::Button::Right)
                    renderer->deseleccionar();
            }

            // Escape vuelve al menu
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    enCombate = false;
                    estadoJuego = EstadoJuego::MENU;
                }
            }
        }

        // DIBUJADO
        ventana.clear(sf::Color(20, 20, 20));
        renderer->dibujarEstadoTablero(juego->getTablero(), juego->getTurnoActual());
        ventana.display();
    }

    delete juego;
    delete renderer;
    delete arena;
    return 0;
}