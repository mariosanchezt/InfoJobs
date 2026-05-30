#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Juego.h"
#include "Renderer.h"
#include "Arena.h"
#include "Menu.h"
#include "AudioManager.h"
#include <iostream>
#include <cmath>

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

            sprite.setScale(sf::Vector2f(1200.f / texSize.x, 900.f / texSize.y));
            ventana.draw(sprite);

            sf::RectangleShape overlay(sf::Vector2f(1200.f, 900.f));
            overlay.setFillColor(sf::Color(0, 0, 0, 60));
            ventana.draw(overlay);
        }

        sf::Text texto(fuente, "Cargando...", 28);
        texto.setFillColor(sf::Color::White);
        texto.setStyle(sf::Text::Bold);

        sf::FloatRect b = texto.getLocalBounds();
        texto.setPosition(sf::Vector2f(600.f - b.size.x / 2.f, 830.f));

        ventana.draw(texto);
        ventana.display();
    }
}

ModoHechizo modoParaHechizo(int indice) {
    switch (indice) {
    case 0: return HECHIZO_ALIADO;   // Curacion
    case 1: return HECHIZO_CASILLA;  // Teleport
    case 2: return HECHIZO_ENEMIGO;  // Dano
    case 3: return HECHIZO_ENEMIGO;  // Ralentizar
    case 4: return HECHIZO_ALIADO;   // Fortalecer
    case 5: return HECHIZO_ALIADO;   // Escudo
    case 6: return HECHIZO_ENEMIGO;  // Congelar
    default: return SIN_HECHIZO;
    }
}

TipoAnimHechizo animParaHechizo(int indice) {
    switch (indice) {
    case 0: return ANIM_CURACION;
    case 2: return ANIM_DANO;
    case 3: return ANIM_RALENTIZAR;
    case 4: return ANIM_FORTALECER;
    case 5: return ANIM_ESCUDO;
    case 6: return ANIM_CONGELAR;
    default: return ANIM_NINGUNA;
    }
}

int main() {
    sf::RenderWindow ventana(sf::VideoMode({ 1200, 900 }), "Archon PvZ");
    ventana.setFramerateLimit(60);

    bool pantallaCompleta = false;
    sf::View vistaJuego(sf::FloatRect({ 0.f, 0.f }, { 1200.f, 900.f }));

    auto manejarRedimension = [&](sf::Vector2u tam) {
        float windowRatio = (float)tam.x / (float)tam.y;
        float viewRatio = 1200.f / 900.f;
        float sizeX = 1.f;
        float sizeY = 1.f;
        float posX = 0.f;
        float posY = 0.f;

        if (windowRatio > viewRatio) {
            sizeX = viewRatio / windowRatio;
            posX = (1.f - sizeX) / 2.f;
        }
        else {
            sizeY = windowRatio / viewRatio;
            posY = (1.f - sizeY) / 2.f;
        }

        vistaJuego.setViewport(sf::FloatRect({ posX, posY }, { sizeX, sizeY }));
        ventana.setView(vistaJuego);
        };

    manejarRedimension(ventana.getSize());

    auto alternarPantallaCompleta = [&]() {
        pantallaCompleta = !pantallaCompleta;

        if (pantallaCompleta) {
            ventana.create(sf::VideoMode::getDesktopMode(), "Archon PvZ", sf::State::Fullscreen);
        }
        else {
            ventana.create(sf::VideoMode({ 1200, 900 }), "Archon PvZ", sf::State::Windowed);
        }

        ventana.setFramerateLimit(60);
        manejarRedimension(ventana.getSize());
        };

    auto eventosGlobalesVentana = [&](const sf::Event& event) {
        if (event.is<sf::Event::Closed>()) {
            ventana.close();
        }
        else if (const auto* res = event.getIf<sf::Event::Resized>()) {
            manejarRedimension(res->size);
        }
        else if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
            if (key->code == sf::Keyboard::Key::F11) {
                alternarPantallaCompleta();
            }
        }
        };

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

    Pieza* piezaTeleport = nullptr;

    int teleportDestFila = -1;
    int teleportDestCol = -1;

    MovimientoIA sugerencia = { -1, -1, -1, -1, 0.f };
    bool mostrarSugerencia = false;

    bool teleportPiezaElegida = false;

    sf::Clock reloj;

    AudioManager audio;
    audio.cargar();
    audio.playMenuMusic();

    auto aplicarConfiguracion = [&]() {
        audio.setVolumenMusica(menu.getVolumenMusicaAplicado());
        audio.setVolumenEfectos(menu.getVolumenEfectosAplicado());

        if (menu.consumirSolicitudPantallaCompleta()) {
            if (menu.getPantallaCompletaActivada() != pantallaCompleta) {
                alternarPantallaCompleta();
            }
        }
        };

    auto activarPantallaVictoria = [&]() {
        if (juego == nullptr) {
            return;
        }

        juego->actualizarPuntuaciones();

        audio.stopMusic();

        if (juego->getBandoGanador() == LUZ) {
            audio.playVictoria();
        }
        else {
            audio.playDerrota();
        }

        estadoJuego = EstadoJuego::VICTORIA;
        };

    aplicarConfiguracion();

    while (ventana.isOpen()) {
        float dt = reloj.restart().asSeconds();

        if (dt > 0.05f) {
            dt = 0.05f;
        }

        // PANTALLA DE VICTORIA
        if (estadoJuego == EstadoJuego::VICTORIA) {
            while (auto event = ventana.pollEvent()) {
                eventosGlobalesVentana(*event);

                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::Enter ||
                        key->code == sf::Keyboard::Key::Escape) {
                        audio.playMenuMusic();
                        estadoJuego = EstadoJuego::MENU;
                    }
                }

                if (event->is<sf::Event::MouseButtonPressed>()) {
                    audio.playMenuMusic();
                    estadoJuego = EstadoJuego::MENU;
                }
            }

            ventana.clear(sf::Color(10, 10, 20));

            if (renderer != nullptr && juego != nullptr) {
                renderer->dibujarPantallaVictoria(
                    juego->getBandoGanador(),
                    juego->getTipoVictoria(),
                    juego->getBajasLuz(),
                    juego->getBajasOscuridad(),
                    juego->getBajasTotales(),
                    juego->getTurnosJugados(),
                    juego->getPuntuacionLuz(),
                    juego->getPuntuacionOscuridad(),
                    juego->getTiempoGeneral(),
                    juego->getTiempoLuz(),
                    juego->getTiempoOscuridad()
                );
            }

            ventana.display();
            continue;
        }

        // MENU PAUSA
        if (estadoJuego == EstadoJuego::PAUSA) {
            while (auto event = ventana.pollEvent()) {
                eventosGlobalesVentana(*event);

                if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                    if (key->code == sf::Keyboard::Key::P ||
                        key->code == sf::Keyboard::Key::Escape) {
                        audio.playGameMusic();
                        estadoJuego = EstadoJuego::JUGANDO_LOCAL;
                    }

                    if (key->code == sf::Keyboard::Key::R) {
                        delete juego;
                        juego = new Juego();

                        delete renderer;
                        renderer = new Renderer(ventana);

                        delete arena;
                        arena = new Arena();

                        renderer->cargarFuente("assets/SamdanEvil.ttf");
                        renderer->cargarSprites("assets");

                        juego->inicializarPartida();

                        aplicarConfiguracion();
                        audio.playGameMusic();

                        arrastrando = false;
                        animando = false;
                        moverConTeclado = false;
                        enCombate = false;

                        hechizoSeleccionado = -1;

                        piezaTeleport = nullptr;
                        teleportPiezaElegida = false;

                        mostrarSugerencia = false;
                        sugerencia = { -1, -1, -1, -1, 0.f };

                        estadoJuego = EstadoJuego::JUGANDO_LOCAL;
                    }

                    if (key->code == sf::Keyboard::Key::M) {
                        audio.playMenuMusic();
                        estadoJuego = EstadoJuego::MENU;
                    }
                }
            }

            ventana.clear(sf::Color(20, 20, 20));

            if (renderer != nullptr && juego != nullptr) {
                renderer->dibujarEstadoTablero(
                    juego->getTablero(),
                    juego->getTurnoActual(),
                    juego->getHechizosUsadosLuz(),
                    juego->getHechizosUsadosOscuridad(),
                    hechizoSeleccionado,
                    juego->getCementerioLuz(),
                    juego->getCementerioOscuridad(),
                    juego->getPuntuacionLuz(),
                    juego->getPuntuacionOscuridad(),
                    juego->getTiempoGeneral(),
                    juego->getTiempoLuz(),
                    juego->getTiempoOscuridad()
                );

                renderer->dibujarMenuPausa();
            }

            ventana.display();
            continue;
        }

        // COMO JUGAR
        if (estadoJuego == EstadoJuego::COMO_JUGAR) {
            while (auto event = ventana.pollEvent()) {
                eventosGlobalesVentana(*event);

                EstadoJuego resultado = menu.procesarEventoComoJugar(*event);

                if (resultado == EstadoJuego::MENU) {
                    estadoJuego = EstadoJuego::MENU;
                }
            }

            ventana.clear(sf::Color(15, 15, 25));
            menu.dibujarComoJugar();
            ventana.display();
            continue;
        }

        // CONFIGURACION
        if (estadoJuego == EstadoJuego::CONFIGURACION) {
            while (auto event = ventana.pollEvent()) {
                eventosGlobalesVentana(*event);

                EstadoJuego resultado = menu.procesarEventoConfiguracion(*event);

                aplicarConfiguracion();

                if (resultado == EstadoJuego::MENU) {
                    estadoJuego = EstadoJuego::MENU;
                }
            }

            aplicarConfiguracion();

            ventana.clear(sf::Color(15, 15, 25));
            menu.dibujarConfiguracion();
            ventana.display();
            continue;
        }

        // CONFIRMAR SALIDA
        if (estadoJuego == EstadoJuego::CONFIRMAR_SALIDA) {
            while (auto event = ventana.pollEvent()) {
                eventosGlobalesVentana(*event);

                EstadoJuego resultado = menu.procesarEventoConfirmarSalida(*event);

                if (resultado == EstadoJuego::SALIR) {
                    ventana.close();
                }
                else if (resultado == EstadoJuego::MENU) {
                    estadoJuego = EstadoJuego::MENU;
                }
            }

            ventana.clear(sf::Color(15, 15, 25));
            menu.dibujarConfirmarSalida();
            ventana.display();
            continue;
        }
        // MODO MENU
        if (estadoJuego == EstadoJuego::MENU) {
            while (auto event = ventana.pollEvent()) {
                eventosGlobalesVentana(*event);

                EstadoJuego resultado = menu.procesarEvento(*event);

                if (resultado == EstadoJuego::SELECCION_DIFICULTAD) {
                    estadoJuego = EstadoJuego::SELECCION_DIFICULTAD;
                }
                else if (resultado == EstadoJuego::COMO_JUGAR) {
                    estadoJuego = EstadoJuego::COMO_JUGAR;
                }
                else if (resultado == EstadoJuego::CONFIGURACION) {
                    estadoJuego = EstadoJuego::CONFIGURACION;
                }
                else if (resultado == EstadoJuego::CONFIRMAR_SALIDA) {
                    estadoJuego = EstadoJuego::CONFIRMAR_SALIDA;
                }
                else if (resultado == EstadoJuego::CARGANDO) {
                    mostrarPantallaCarga(ventana, fuente, 2.0f);

                    estadoJuego = EstadoJuego::JUGANDO_LOCAL;

                    delete juego;
                    delete renderer;
                    delete arena;

                    juego = new Juego();
                    renderer = new Renderer(ventana);
                    arena = new Arena();

                    arena->setAudio(&audio);
                    arena->setModoIA(false);

                    arrastrando = false;
                    animando = false;
                    moverConTeclado = false;
                    enCombate = false;

                    hechizoSeleccionado = -1;
                    tiempoEsperaIA = 0.f;

                    piezaTeleport = nullptr;
                    teleportPiezaElegida = false;

                    mostrarSugerencia = false;
                    sugerencia = { -1, -1, -1, -1, 0.f };

                    renderer->cargarFuente("assets/SamdanEvil.ttf");
                    renderer->cargarSprites("assets");

                    juego->inicializarPartida();

                    aplicarConfiguracion();
                    audio.playGameMusic();
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
                eventosGlobalesVentana(*event);

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

                    arena->setAudio(&audio);
                    arena->setModoIA(true);

                    arrastrando = false;
                    animando = false;
                    moverConTeclado = false;
                    enCombate = false;

                    hechizoSeleccionado = -1;
                    tiempoEsperaIA = 0.f;

                    piezaTeleport = nullptr;
                    teleportPiezaElegida = false;

                    mostrarSugerencia = false;
                    sugerencia = { -1, -1, -1, -1, 0.f };

                    renderer->cargarFuente("assets/SamdanEvil.ttf");
                    renderer->cargarSprites("assets");

                    juego->inicializarPartida();

                    aplicarConfiguracion();
                    audio.playGameMusic();
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

        // Cancelar hechizo activo
        auto cancelarHechizo = [&]() {
            hechizoSeleccionado = -1;

            piezaTeleport = nullptr;

            teleportDestFila = -1;
            teleportDestCol = -1;

            teleportPiezaElegida = false;

            renderer->setModoHechizo(SIN_HECHIZO);
            renderer->setPanelHechizosVisible(false);
            renderer->resetNombrePiezaTeleport();
            };

        // Ejecutar movimiento o iniciar combate
        auto ejecutarMovimiento = [&](int fOri, int cOri, int fDest, int cDest) {
            Pieza* p = juego->getTablero()->getPieza(fOri, cOri);
            Pieza* ocupante = juego->getTablero()->getPieza(fDest, cDest);

            if (ocupante != nullptr && p != nullptr && ocupante->getBando() != p->getBando()) {
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

                aplicarConfiguracion();
                audio.playArenaMusic();

                enCombate = true;

                renderer->setEstado(ARENA);
            }
            else {
                juego->moverPieza(fOri, cOri, fDest, cDest);

                juego->actualizarPuntuaciones();

                if (juego->verificarVictoria() || juego->verificarVictoriaPorTiempo()) {
                    activarPantallaVictoria();
                }
            }

            renderer->deseleccionar();

            mostrarSugerencia = false;
            sugerencia = { -1, -1, -1, -1, 0.f };
            };

        // Actualizar animaciones cada frame
        if (renderer != nullptr) {
            renderer->actualizarAnimaciones(dt);
        }

        // Actualizar temporizadores solo en tablero activo
        if (estadoJuego == EstadoJuego::JUGANDO_LOCAL &&
            juego != nullptr &&
            !enCombate &&
            !animando &&
            renderer != nullptr &&
            !renderer->teleportEnCurso()) {

            juego->actualizarTemporizadores(dt);
            juego->actualizarPuntuaciones();

            if (juego->verificarVictoriaPorTiempo()) {
                activarPantallaVictoria();
            }
        }

        // Cuando termina la animacion de teleport, ejecutamos el hechizo
        if (renderer != nullptr && renderer->teleportTerminado()) {
            juego->lanzarHechizo(2, piezaTeleport, teleportDestFila, teleportDestCol);

            renderer->resetAnimTeleport();

            cancelarHechizo();

            juego->actualizarPuntuaciones();

            if (juego->verificarVictoria() || juego->verificarVictoriaPorTiempo()) {
                activarPantallaVictoria();
            }
        }

        // Actualizar bordes de efectos persistentes
        if (renderer != nullptr && juego != nullptr) {
            renderer->setPiezasConEfecto(
                juego->getPiezaCongelada(),
                juego->getPiezaRalentizada(),
                juego->getPiezaFortalecida(),
                juego->getPiezaEscudo()
            );
        }

        // MODO ARENA
        if (enCombate) {
            while (auto event = ventana.pollEvent()) {
                eventosGlobalesVentana(*event);
            }

            arena->update(dt);
            ventana.clear(sf::Color(20, 20, 20));

            if (arena->haTerminado()) {
                sf::sleep(sf::milliseconds(500));

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

                aplicarConfiguracion();
                audio.playGameMusic();

                juego->actualizarPuntuaciones();
                juego->cambiarTurno();

                tiempoEsperaIA = 0.f;

                if (juego->verificarVictoria() || juego->verificarVictoriaPorTiempo()) {
                    activarPantallaVictoria();
                }
            }
            else {
                renderer->dibujarEstadoArena(*arena);
            }

            ventana.display();

            continue;
        }

        // LOGICA DE ANIMACION NORMAL
        if (animando && !renderer->teleportEnCurso()) {
            sf::Vector2f dir = posPixelDestino - posPixelMuneco;

            float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

            if (dist <= velAnimacion * dt) {
                animando = false;
                audio.stopMovimiento();

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
            !animando &&
            renderer != nullptr &&
            !renderer->teleportEnCurso()) {

            tiempoEsperaIA += dt;

            if (tiempoEsperaIA > 0.8f) {
                tiempoEsperaIA = 0.f;

                if (!juego->iaLanzarHechizo()) {
                    MovimientoIA mov = juego->obtenerMovimientoIA();

                    if (mov.fOrigen != -1) {
                        animando = true;
                        audio.playMovimiento();

                        targetFila = mov.fDestino;
                        targetCol = mov.cDestino;

                        renderer->seleccionarCasilla(
                            mov.fOrigen,
                            mov.cOrigen,
                            juego->getTablero()
                        );

                        posPixelMuneco = renderer->getCentroCasilla(mov.fOrigen, mov.cOrigen);
                        posPixelDestino = renderer->getCentroCasilla(mov.fDestino, mov.cDestino);
                    }
                }
                else {
                    juego->actualizarPuntuaciones();

                    if (juego->verificarVictoria() || juego->verificarVictoriaPorTiempo()) {
                        activarPantallaVictoria();
                    }
                }
            }
        }
        // MODO TABLERO - EVENTOS
        while (auto event = ventana.pollEvent()) {
            eventosGlobalesVentana(*event);

            if (animando || renderer->teleportEnCurso()) {
                continue;
            }

            // CLICS CON EL RATON
            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f posMundo = ventana.mapPixelToCoords(
                    sf::Vector2i(click->position.x, click->position.y)
                );

                if (click->button == sf::Mouse::Button::Left) {
                    int fila;
                    int col;

                    if (renderer->pixelACasilla((int)posMundo.x, (int)posMundo.y, fila, col)) {
                        int fSel = renderer->getFilaSeleccionada();
                        int cSel = renderer->getColSeleccionada();

                        ModoHechizo modo = renderer->getModoHechizo();
                        Bando turno = juego->getTurnoActual();

                        // MODO HECHIZO
                        if (modo != SIN_HECHIZO) {
                            Pieza* objetivo = juego->getTablero()->getPieza(fila, col);

                            if (modo == HECHIZO_ALIADO) {
                                if (objetivo != nullptr && objetivo->getBando() == turno) {
                                    juego->lanzarHechizo(
                                        hechizoSeleccionado + 1,
                                        objetivo,
                                        fila,
                                        col
                                    );

                                    renderer->lanzarAnimHechizo(
                                        fila,
                                        col,
                                        animParaHechizo(hechizoSeleccionado)
                                    );

                                    cancelarHechizo();
                                    renderer->deseleccionar();

                                    juego->actualizarPuntuaciones();

                                    if (juego->verificarVictoria() ||
                                        juego->verificarVictoriaPorTiempo()) {
                                        activarPantallaVictoria();
                                    }
                                }
                            }
                            else if (modo == HECHIZO_ENEMIGO) {
                                if (objetivo != nullptr && objetivo->getBando() != turno) {
                                    juego->lanzarHechizo(
                                        hechizoSeleccionado + 1,
                                        objetivo,
                                        fila,
                                        col
                                    );

                                    renderer->lanzarAnimHechizo(
                                        fila,
                                        col,
                                        animParaHechizo(hechizoSeleccionado)
                                    );

                                    cancelarHechizo();
                                    renderer->deseleccionar();

                                    juego->actualizarPuntuaciones();

                                    if (juego->verificarVictoria() ||
                                        juego->verificarVictoriaPorTiempo()) {
                                        activarPantallaVictoria();
                                    }
                                }
                            }
                            else if (modo == HECHIZO_CASILLA) {
                                if (!teleportPiezaElegida) {
                                    if (objetivo != nullptr && objetivo->getBando() == turno) {
                                        piezaTeleport = objetivo;
                                        teleportPiezaElegida = true;

                                        renderer->setNombrePiezaTeleport(objetivo->getNombre());
                                    }
                                }
                                else {
                                    if (objetivo == nullptr) {
                                        teleportDestFila = fila;
                                        teleportDestCol = col;

                                        renderer->iniciarAnimTeleport(
                                            piezaTeleport,
                                            piezaTeleport->filaInicial,
                                            piezaTeleport->colInicial,
                                            fila,
                                            col
                                        );

                                        renderer->deseleccionar();
                                    }
                                }
                            }

                            continue;
                        }

                        // MODO NORMAL
                        if (fSel == -1) {
                            Pieza* p = juego->getTablero()->getPieza(fila, col);

                            if (p && p->getBando() == juego->getTurnoActual()) {
                                renderer->seleccionarCasilla(
                                    fila,
                                    col,
                                    juego->getTablero()
                                );

                                audio.playSeleccion();

                                arrastrando = true;
                                moverConTeclado = false;

                                posPixelMuneco = posMundo;
                            }
                        }
                        else if (fila == fSel && col == cSel) {
                            arrastrando = true;
                            moverConTeclado = false;

                            posPixelMuneco = posMundo;
                        }
                        else {
                            if (juego->getTablero()->esMovimientoValido(fSel, cSel, fila, col)) {
                                moverConTeclado = false;
                                animando = true;

                                audio.playMovimiento();

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
                    if (renderer->getModoHechizo() != SIN_HECHIZO) {
                        cancelarHechizo();
                    }
                    else {
                        renderer->deseleccionar();
                        moverConTeclado = false;
                    }
                }
            }

            // MOVER RATON ARRASTRANDO
            else if (const auto* move = event->getIf<sf::Event::MouseMoved>()) {
                if (arrastrando) {
                    sf::Vector2f posMundo = ventana.mapPixelToCoords(
                        sf::Vector2i(move->position.x, move->position.y)
                    );

                    posPixelMuneco = posMundo;
                }
            }

            // SOLTAR RATON
            else if (const auto* release = event->getIf<sf::Event::MouseButtonReleased>()) {
                if (release->button == sf::Mouse::Button::Left && arrastrando) {
                    arrastrando = false;

                    int fila;
                    int col;

                    sf::Vector2f posMundo = ventana.mapPixelToCoords(
                        sf::Vector2i(release->position.x, release->position.y)
                    );

                    if (renderer->pixelACasilla((int)posMundo.x, (int)posMundo.y, fila, col)) {
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

            // TECLADO
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                // P: pausar
                if (key->code == sf::Keyboard::Key::P) {
                    aplicarConfiguracion();
                    audio.playPausa();

                    estadoJuego = EstadoJuego::PAUSA;
                    continue;
                }

                // Escape: cancela hechizo o deselecciona
                if (key->code == sf::Keyboard::Key::Escape) {
                    if (renderer->getModoHechizo() != SIN_HECHIZO) {
                        cancelarHechizo();
                    }
                    else {
                        renderer->deseleccionar();
                        moverConTeclado = false;
                    }

                    continue;
                }

                // I: mostrar sugerencia de IA
                if (key->code == sf::Keyboard::Key::I) {
                    mostrarSugerencia = !mostrarSugerencia;

                    if (mostrarSugerencia) {
                        sugerencia = juego->obtenerSugerencia();
                    }
                    else {
                        sugerencia = { -1, -1, -1, -1, 0.f };
                    }

                    continue;
                }

                // H: mostrar/ocultar panel de hechizos
                if (key->code == sf::Keyboard::Key::H) {
                    if (renderer->getModoHechizo() != SIN_HECHIZO) {
                        cancelarHechizo();
                    }
                    else {
                        bool visible = renderer->getPanelHechizosVisible();

                        renderer->setPanelHechizosVisible(!visible);

                        if (!visible) {
                            hechizoSeleccionado = -1;
                        }
                    }

                    continue;
                }

                // 1-7: seleccionar hechizo
                int teclaHechizo = -1;

                if (key->code == sf::Keyboard::Key::Num1) teclaHechizo = 0;
                else if (key->code == sf::Keyboard::Key::Num2) teclaHechizo = 1;
                else if (key->code == sf::Keyboard::Key::Num3) teclaHechizo = 2;
                else if (key->code == sf::Keyboard::Key::Num4) teclaHechizo = 3;
                else if (key->code == sf::Keyboard::Key::Num5) teclaHechizo = 4;
                else if (key->code == sf::Keyboard::Key::Num6) teclaHechizo = 5;
                else if (key->code == sf::Keyboard::Key::Num7) teclaHechizo = 6;

                if (teclaHechizo != -1) {
                    if (!renderer->getPanelHechizosVisible()) {
                        continue;
                    }

                    bool* usados = (juego->getTurnoActual() == LUZ)
                        ? juego->getHechizosUsadosLuz()
                        : juego->getHechizosUsadosOscuridad();

                    if (!usados[teclaHechizo]) {
                        if (hechizoSeleccionado == teclaHechizo) {
                            cancelarHechizo();
                        }
                        else {
                            hechizoSeleccionado = teclaHechizo;

                            renderer->setModoHechizo(modoParaHechizo(teclaHechizo));
                            renderer->setPanelHechizosVisible(true);
                            renderer->deseleccionar();

                            moverConTeclado = false;

                            piezaTeleport = nullptr;
                            teleportPiezaElegida = false;
                        }
                    }

                    continue;
                }

                // En modo hechizo: cursor libre + accion confirma objetivo
                if (renderer->getModoHechizo() != SIN_HECHIZO) {
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
                        renderer->moverCursor(dFila, dCol);
                    }

                    if (accion) {
                        int fila = renderer->getCursorFila();
                        int col = renderer->getCursorCol();

                        Pieza* objetivo = juego->getTablero()->getPieza(fila, col);
                        ModoHechizo modo = renderer->getModoHechizo();

                        if (modo == HECHIZO_ALIADO) {
                            if (objetivo != nullptr && objetivo->getBando() == turno) {
                                juego->lanzarHechizo(
                                    hechizoSeleccionado + 1,
                                    objetivo,
                                    fila,
                                    col
                                );

                                renderer->lanzarAnimHechizo(
                                    fila,
                                    col,
                                    animParaHechizo(hechizoSeleccionado)
                                );

                                cancelarHechizo();
                                renderer->deseleccionar();
                            }
                        }
                        else if (modo == HECHIZO_ENEMIGO) {
                            if (objetivo != nullptr && objetivo->getBando() != turno) {
                                juego->lanzarHechizo(
                                    hechizoSeleccionado + 1,
                                    objetivo,
                                    fila,
                                    col
                                );

                                renderer->lanzarAnimHechizo(
                                    fila,
                                    col,
                                    animParaHechizo(hechizoSeleccionado)
                                );

                                cancelarHechizo();
                                renderer->deseleccionar();
                            }
                        }
                        else if (modo == HECHIZO_CASILLA) {
                            if (!teleportPiezaElegida) {
                                if (objetivo != nullptr && objetivo->getBando() == turno) {
                                    piezaTeleport = objetivo;
                                    teleportPiezaElegida = true;

                                    renderer->setNombrePiezaTeleport(objetivo->getNombre());
                                }
                            }
                            else {
                                if (objetivo == nullptr) {
                                    teleportDestFila = fila;
                                    teleportDestCol = col;

                                    renderer->iniciarAnimTeleport(
                                        piezaTeleport,
                                        piezaTeleport->filaInicial,
                                        piezaTeleport->colInicial,
                                        fila,
                                        col
                                    );

                                    renderer->deseleccionar();
                                }
                            }
                        }

                        juego->actualizarPuntuaciones();

                        if (juego->verificarVictoria() ||
                            juego->verificarVictoriaPorTiempo()) {
                            activarPantallaVictoria();
                        }
                    }

                    continue;
                }

                // MOVIMIENTO NORMAL CON TECLADO
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

                    if (fSel == -1) {
                        Pieza* p = juego->getTablero()->getPieza(fila, col);

                        if (p && p->getBando() == turno) {
                            renderer->seleccionarCasilla(
                                fila,
                                col,
                                juego->getTablero()
                            );

                            audio.playSeleccion();

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

                            audio.playMovimiento();

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
            (arrastrando || animando || moverConTeclado) &&
            !renderer->teleportEnCurso();

        renderer->dibujarEstadoTablero(
            juego->getTablero(),
            juego->getTurnoActual(),
            juego->getHechizosUsadosLuz(),
            juego->getHechizosUsadosOscuridad(),
            hechizoSeleccionado,
            juego->getCementerioLuz(),
            juego->getCementerioOscuridad(),
            juego->getPuntuacionLuz(),
            juego->getPuntuacionOscuridad(),
            juego->getTiempoGeneral(),
            juego->getTiempoLuz(),
            juego->getTiempoOscuridad()
        );

        if ((arrastrando || animando || moverConTeclado) &&
            !renderer->teleportEnCurso() &&
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

        if (mostrarSugerencia && sugerencia.fOrigen != -1) {
            renderer->dibujarSugerencia(
                sugerencia.fOrigen,
                sugerencia.cOrigen,
                sugerencia.fDestino,
                sugerencia.cDestino
            );
        }

        ventana.display();
    }

    delete juego;
    delete renderer;
    delete arena;

    return 0;
}