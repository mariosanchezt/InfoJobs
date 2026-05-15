#include <SFML/Graphics.hpp>
#include "Juego.h"
#include "Renderer.h"
#include "Arena.h"
#include "Menu.h"
#include <iostream>
#include <cmath>

void mostrarPantallaCarga(sf::RenderWindow& ventana, sf::Font& fuente, float segundos) {
    sf::Texture texCarga;
    bool cargada = texCarga.loadFromFile("assets/loading_bg.png");
    texCarga.setSmooth(true);

    sf::Clock temporizador;

    while (temporizador.getElapsedTime().asSeconds() < segundos && ventana.isOpen()) {
        while (auto event = ventana.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                ventana.close();
            }
        }

        ventana.clear(sf::Color(10, 10, 20));

        if (cargada) {
            sf::Sprite sprite(texCarga);

            sf::Vector2u texSize = texCarga.getSize();

            float escalaX = 1000.f / texSize.x;
            float escalaY = 900.f / texSize.y;

            sprite.setScale(sf::Vector2f(escalaX, escalaY));

            ventana.draw(sprite);

            sf::RectangleShape overlay(sf::Vector2f(1000.f, 900.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 60));
            ventana.draw(overlay);
        }

        sf::Text texto(fuente, "Cargando...", 28);
        texto.setFillColor(sf::Color::White);
        texto.setStyle(sf::Text::Bold);

        sf::FloatRect b = texto.getLocalBounds();
        texto.setPosition(sf::Vector2f(500.f - b.size.x / 2.f, 830.f));

        ventana.draw(texto);

        ventana.display();
    }
}

int main() {
    sf::RenderWindow ventana(sf::VideoMode({ 1000, 900 }), "Archon PvZ");
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

    int filaAtacante = -1;
    int colAtacante = -1;

    int filaDefensor = -1;
    int colDefensor = -1;

    // Variables de animacion y arrastre
    bool arrastrando = false;
    bool animando = false;
    bool moverConTeclado = false;

    sf::Vector2f posPixelMuneco;
    sf::Vector2f posPixelDestino;

    int targetFila = -1;
    int targetCol = -1;

    float velAnimacion = 750.f;

    int hechizoSeleccionado = -1;

    float tiempoEsperaIA = 0.f;

    sf::Clock reloj;

    while (ventana.isOpen()) {
        float dt = reloj.restart().asSeconds();

        if (dt > 0.05f) {
            dt = 0.05f;
        }

        // PANTALLA DE VICTORIA
        if (estadoJuego == EstadoJuego::VICTORIA) {
            while (auto event = ventana.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    ventana.close();
                }

                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::Enter ||
                        key->code == sf::Keyboard::Key::Escape) {
                        estadoJuego = EstadoJuego::MENU;
                    }
                }

                if (event->is<sf::Event::MouseButtonPressed>()) {
                    estadoJuego = EstadoJuego::MENU;
                }
            }

            ventana.clear(sf::Color(10, 10, 20));

            renderer->dibujarPantallaVictoria(juego->getBandoGanador());

            ventana.display();

            continue;
        }

        // MODO MENU
        if (estadoJuego == EstadoJuego::MENU) {
            while (auto event = ventana.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    ventana.close();
                }

                EstadoJuego resultado = menu.procesarEvento(*event);
                if (resultado == EstadoJuego::SELECCION_DIFICULTAD) {
                    estadoJuego = EstadoJuego::SELECCION_DIFICULTAD;
                }
                else if (resultado != EstadoJuego::MENU) {
                    mostrarPantallaCarga(ventana, fuente, 2.0f);

                    estadoJuego = EstadoJuego::JUGANDO_LOCAL;

                    delete juego;
                    delete renderer;
                    delete arena;

                    juego = new Juego();
                    renderer = new Renderer(ventana);
                    arena = new Arena();

                    arrastrando = false;
                    animando = false;
                    moverConTeclado = false;
                    enCombate = false;

                    hechizoSeleccionado = -1;
                    tiempoEsperaIA = 0.f;

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

        // SELECCION DE DIFICULTAD
        if (estadoJuego == EstadoJuego::SELECCION_DIFICULTAD) {
            while (auto event = ventana.pollEvent()) {
                if (event->is<sf::Event::Closed>()) ventana.close();

                EstadoJuego resultado = menu.procesarEventoDificultad(*event);
                if (resultado == EstadoJuego::JUGANDO_IA) {
                    mostrarPantallaCarga(ventana, fuente, 2.0f);

                    estadoJuego = EstadoJuego::JUGANDO_LOCAL;

                    delete juego;
                    delete renderer;
                    delete arena;

                    juego = new Juego();
                    juego->activarIA(menu.dificultadSeleccionada);
                    renderer = new Renderer(ventana);
                    arena = new Arena();

                    arrastrando = false;
                    animando = false;
                    moverConTeclado = false;
                    enCombate = false;

                    renderer->cargarFuente("assets/SamdanEvil.ttf");
                    renderer->cargarSprites("assets");
                    juego->inicializarPartida();
                }
                else if (resultado == EstadoJuego::MENU) {
                    estadoJuego = EstadoJuego::MENU;
                }
            }

            ventana.clear(sf::Color(15, 15, 25));
            menu.dibujarDificultad();
            ventana.display();
            continue;
        }

        auto ejecutarMovimiento = [&](int fOri, int cOri, int fDest, int cDest) {
            Pieza* p = juego->getTablero()->getPieza(fOri, cOri);
            Pieza* ocupante = juego->getTablero()->getPieza(fDest, cDest);

            if (ocupante != nullptr && p != nullptr && ocupante->getBando() != p->getBando()) {
                if (juego->esModoIA()) {
                    // Combate automatico en modo IA
                    juego->iniciarCombate(p, ocupante, fOri, cOri, fDest, cDest);
                    juego->cambiarTurno();

                    if (juego->verificarVictoria()) {
                        estadoJuego = EstadoJuego::VICTORIA;
                    }
                }
                else {
                    filaAtacante = fOri;
                    colAtacante = cOri;

                    filaDefensor = fDest;
                    colDefensor = cDest;

                    arena->iniciarCombate(p, ocupante);

                    if (juego->getPiezaCongelada() == p) {
                        arena->setMultiplicadorVelocidad(p, 0.f);
                    }
                    else if (juego->getPiezaCongelada() == ocupante) {
                        arena->setMultiplicadorVelocidad(ocupante, 0.f);
                    }

                    enCombate = true;

                    renderer->setEstado(ARENA);
                }
            }
            else {
                juego->moverPieza(fOri, cOri, fDest, cDest);

                if (juego->verificarVictoria()) {
                    estadoJuego = EstadoJuego::VICTORIA;
                }
            }

            renderer->deseleccionar();
            };

        // MODO ARENA
        if (enCombate) {
            while (auto event = ventana.pollEvent()) {
                if (event->is<sf::Event::Closed>()) {
                    ventana.close();
                }
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
                    juego->registrarMuerte(atacante);
                    juego->registrarMuerte(defensor);

                    delete atacante;
                    delete defensor;
                }
                else if (ganador == atacante) {
                    juego->getTablero()->colocarPieza(filaDefensor, colDefensor, atacante);

                    atacante->filaInicial = filaDefensor;
                    atacante->colInicial = colDefensor;

                    juego->registrarMuerte(defensor);

                    delete defensor;
                }
                else {
                    juego->getTablero()->colocarPieza(filaDefensor, colDefensor, defensor);

                    juego->registrarMuerte(atacante);

                    delete atacante;
                }

                juego->cambiarTurno();

                tiempoEsperaIA = 0.f;

                if (juego->verificarVictoria()) {
                    estadoJuego = EstadoJuego::VICTORIA;
                }

                ventana.clear(sf::Color(20, 20, 20));

                renderer->dibujarEstadoTablero(
                    juego->getTablero(),
                    juego->getTurnoActual(),
                    juego->getHechizosUsadosLuz(),
                    juego->getHechizosUsadosOscuridad(),
                    hechizoSeleccionado,
                    juego->getCementerioLuz(),
                    juego->getCementerioOscuridad()
                );

                ventana.display();
            }
            else {
                renderer->dibujarEstadoArena(*arena);
            }

            ventana.display();

            continue;
        }

        // LOGICA DE ANIMACION
        if (animando) {
            sf::Vector2f dir = posPixelDestino - posPixelMuneco;

            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist <= velAnimacion * dt) {
                animando = false;

                ejecutarMovimiento(
                    renderer->getFilaSeleccionada(),
                    renderer->getColSeleccionada(),
                    targetFila,
                    targetCol
                );
            }
            else {
                dir /= dist;
                posPixelMuneco += dir * velAnimacion * dt;
            }
        }

        // TURNO DE LA IA
        if (juego->esModoIA() &&
            juego->getTurnoActual() == OSCURIDAD &&
            !enCombate &&
            !animando) {

            tiempoEsperaIA += dt;

            if (tiempoEsperaIA > 0.8f) {
                tiempoEsperaIA = 0.f;

                MovimientoIA mov = juego->obtenerMovimientoIA();

                if (mov.fOrigen != -1) {
                    animando = true;

                    targetFila = mov.fDestino;
                    targetCol = mov.cDestino;

                    renderer->seleccionarCasilla(
                        mov.fOrigen,
                        mov.cOrigen,
                        juego->getTablero()
                    );

                    posPixelMuneco = renderer->getCentroCasilla(
                        mov.fOrigen,
                        mov.cOrigen
                    );

                    posPixelDestino = renderer->getCentroCasilla(
                        mov.fDestino,
                        mov.cDestino
                    );
                }
            }
        }

        // MODO TABLERO - EVENTOS
        while (auto event = ventana.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                ventana.close();
            }

            if (animando) {
                continue;
            }

            // DRAG AND DROP / CLICK RATON
            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    int fila;
                    int col;

                    if (renderer->pixelACasilla(click->position.x, click->position.y, fila, col)) {
                        int fSel = renderer->getFilaSeleccionada();
                        int cSel = renderer->getColSeleccionada();

                        if (hechizoSeleccionado != -1) {
                            Pieza* objetivo = juego->getTablero()->getPieza(fila, col);

                            juego->lanzarHechizo(
                                hechizoSeleccionado + 1,
                                objetivo,
                                fila,
                                col
                            );

                            hechizoSeleccionado = -1;

                            renderer->deseleccionar();

                            continue;
                        }

                        if (fSel == -1) {
                            Pieza* p = juego->getTablero()->getPieza(fila, col);

                            if (p && p->getBando() == juego->getTurnoActual()) {
                                renderer->seleccionarCasilla(fila, col, juego->getTablero());

                                arrastrando = true;
                                moverConTeclado = false;

                                posPixelMuneco = sf::Vector2f(
                                    (float)click->position.x,
                                    (float)click->position.y
                                );
                            }
                        }
                        else if (fila == fSel && col == cSel) {
                            arrastrando = true;
                            moverConTeclado = false;

                            posPixelMuneco = sf::Vector2f(
                                (float)click->position.x,
                                (float)click->position.y
                            );
                        }
                        else {
                            if (juego->getTablero()->esMovimientoValido(fSel, cSel, fila, col)) {
                                moverConTeclado = false;
                                animando = true;

                                targetFila = fila;
                                targetCol = col;

                                posPixelMuneco = renderer->getCentroCasilla(fSel, cSel);
                                posPixelDestino = renderer->getCentroCasilla(fila, col);
                            }
                            else {
                                renderer->deseleccionar();
                                moverConTeclado = false;
                            }
                        }
                    }
                }

                if (click->button == sf::Mouse::Button::Right) {
                    renderer->deseleccionar();
                    moverConTeclado = false;
                }
            }
            else if (const auto* move = event->getIf<sf::Event::MouseMoved>()) {
                if (arrastrando) {
                    posPixelMuneco = sf::Vector2f(
                        (float)move->position.x,
                        (float)move->position.y
                    );
                }
            }
            else if (const auto* release = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (release->button == sf::Mouse::Button::Left && arrastrando) {
                    arrastrando = false;

                    int fila;
                    int col;

                    if (renderer->pixelACasilla(release->position.x, release->position.y, fila, col)) {
                        int fSel = renderer->getFilaSeleccionada();
                        int cSel = renderer->getColSeleccionada();

                        if (fila != fSel || col != cSel) {
                            if (juego->getTablero()->esMovimientoValido(fSel, cSel, fila, col)) {
                                ejecutarMovimiento(fSel, cSel, fila, col);
                            }
                        }
                    }
                }
            }

            // CONTROLES POR TECLADO
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    animando = false;
                    arrastrando = false;
                    moverConTeclado = false;
                    enCombate = false;

                    estadoJuego = EstadoJuego::MENU;

                    continue;
                }

                int teclaHechizo = -1;

                if (key->code == sf::Keyboard::Key::Num1) teclaHechizo = 0;
                else if (key->code == sf::Keyboard::Key::Num2) teclaHechizo = 1;
                else if (key->code == sf::Keyboard::Key::Num3) teclaHechizo = 2;
                else if (key->code == sf::Keyboard::Key::Num4) teclaHechizo = 3;
                else if (key->code == sf::Keyboard::Key::Num5) teclaHechizo = 4;
                else if (key->code == sf::Keyboard::Key::Num6) teclaHechizo = 5;
                else if (key->code == sf::Keyboard::Key::Num7) teclaHechizo = 6;

                if (teclaHechizo != -1) {
                    hechizoSeleccionado =
                        (hechizoSeleccionado == teclaHechizo)
                        ? -1
                        : teclaHechizo;

                    renderer->deseleccionar();

                    moverConTeclado = false;

                    continue;
                }

                Bando turno = juego->getTurnoActual();

                int dFila = 0;
                int dCol = 0;

                bool accion = false;

                if (turno == LUZ) {
                    if (key->code == sf::Keyboard::Key::W) dFila = -1;
                    if (key->code == sf::Keyboard::Key::S) dFila = 1;
                    if (key->code == sf::Keyboard::Key::A) dCol = -1;
                    if (key->code == sf::Keyboard::Key::D) dCol = 1;
                    if (key->code == sf::Keyboard::Key::Space) accion = true;
                }
                else {
                    if (key->code == sf::Keyboard::Key::Up) dFila = -1;
                    if (key->code == sf::Keyboard::Key::Down) dFila = 1;
                    if (key->code == sf::Keyboard::Key::Left) dCol = -1;
                    if (key->code == sf::Keyboard::Key::Right) dCol = 1;
                    if (key->code == sf::Keyboard::Key::Enter) accion = true;
                }

                if (dFila != 0 || dCol != 0) {
                    int fSel = renderer->getFilaSeleccionada();

                    renderer->moverCursor(dFila, dCol);

                    if (fSel != -1) {
                        moverConTeclado = true;

                        posPixelMuneco = renderer->getCentroCasilla(
                            renderer->getCursorFila(),
                            renderer->getCursorCol()
                        );
                    }
                }

                if (accion) {
                    int fila = renderer->getCursorFila();
                    int col = renderer->getCursorCol();

                    int fSel = renderer->getFilaSeleccionada();
                    int cSel = renderer->getColSeleccionada();

                    if (hechizoSeleccionado != -1) {
                        Pieza* objetivo = juego->getTablero()->getPieza(fila, col);

                        juego->lanzarHechizo(
                            hechizoSeleccionado + 1,
                            objetivo,
                            fila,
                            col
                        );

                        hechizoSeleccionado = -1;

                        renderer->deseleccionar();

                        moverConTeclado = false;

                        continue;
                    }

                    if (fSel == -1) {
                        Pieza* p = juego->getTablero()->getPieza(fila, col);

                        if (p && p->getBando() == turno) {
                            renderer->seleccionarCasilla(
                                fila,
                                col,
                                juego->getTablero()
                            );

                            moverConTeclado = false;

                            posPixelMuneco = renderer->getCentroCasilla(fila, col);
                        }
                    }
                    else if (fila == fSel && col == cSel) {
                        renderer->deseleccionar();

                        moverConTeclado = false;
                    }
                    else {
                        if (juego->getTablero()->esMovimientoValido(fSel, cSel, fila, col)) {
                            moverConTeclado = false;
                            animando = true;

                            targetFila = fila;
                            targetCol = col;

                            posPixelMuneco = renderer->getCentroCasilla(fSel, cSel);
                            posPixelDestino = renderer->getCentroCasilla(fila, col);
                        }
                        else {
                            renderer->deseleccionar();
                            moverConTeclado = false;
                        }
                    }
                }
            }
        }

        // DIBUJADO FINAL
        ventana.clear(sf::Color(20, 20, 20));

        renderer->ocultarMunecoCursor =
            (arrastrando || animando || moverConTeclado);

        renderer->dibujarEstadoTablero(
            juego->getTablero(),
            juego->getTurnoActual(),
            juego->getHechizosUsadosLuz(),
            juego->getHechizosUsadosOscuridad(),
            hechizoSeleccionado,
            juego->getCementerioLuz(),
            juego->getCementerioOscuridad()
        );

        if ((arrastrando || animando || moverConTeclado) &&
            renderer->getFilaSeleccionada() != -1) {

            Pieza* p = juego->getTablero()->getPieza(
                renderer->getFilaSeleccionada(),
                renderer->getColSeleccionada()
            );

            if (p) {
                renderer->dibujarPiezaPixel(
                    p,
                    posPixelMuneco.x,
                    posPixelMuneco.y
                );
            }
        }

        ventana.display();
    }

    delete juego;
    delete renderer;
    delete arena;

    return 0;
}