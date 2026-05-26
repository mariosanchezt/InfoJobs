#include "Menu.h"
#include <algorithm>
#include <sstream>
#include "IAJugador.h"

static constexpr float VENTANA_ANCHO = 1200.f;
static constexpr float VENTANA_ALTO = 900.f;

Menu::Menu(sf::RenderWindow& vent, sf::Font& f)
    : ventana(vent), fuente(f), spriteFondo(texFondo)
{
    botonSeleccionado = 0;
    fondoCargado = false;

    // Intentamos cargar el fondo del menu
    if (texFondo.loadFromFile("assets/menu_bg.png")) {
        fondoCargado = true;
        texFondo.setSmooth(true);
        spriteFondo = sf::Sprite(texFondo);

        // Escalamos la imagen para que cubra la ventana 1200x900
        sf::Vector2u texSize = texFondo.getSize();

        float escalaX = VENTANA_ANCHO / texSize.x;
        float escalaY = VENTANA_ALTO / texSize.y;

        spriteFondo.setScale(sf::Vector2f(escalaX, escalaY));
        spriteFondo.setPosition(sf::Vector2f(0.f, 0.f));
    }

    colorTexto = sf::Color(30, 30, 30);
    colorTextoDeshabilitado = sf::Color(90, 90, 90, 200);

    colorBotonActivo = sf::Color(255, 255, 255);
    colorBotonDeshabilitado = sf::Color(100, 100, 100);
    colorBotonHover = sf::Color(200, 255, 50);

    float anchoBoton = 300.f;
    float altoBoton = 65.f;

    // Botones movidos hacia la derecha para quedar dentro de la lapida
    float xBoton = 635.f;

    Boton b1;
    b1.forma.setSize(sf::Vector2f(anchoBoton, altoBoton));
    b1.forma.setPosition(sf::Vector2f(xBoton, 210.f));
    b1.forma.setRotation(sf::degrees(2.0f));
    b1.texto = "1 VS 1 LOCAL";
    b1.habilitado = true;
    botones.push_back(b1);

    Boton b2;
    b2.forma.setSize(sf::Vector2f(anchoBoton, altoBoton));
    b2.forma.setPosition(sf::Vector2f(xBoton, 320.f));
    b2.forma.setRotation(sf::degrees(2.0f));
    b2.texto = "1 VS IA";
    b2.habilitado = true;
    botones.push_back(b2);

    // Botones del submenu de dificultad, tambien dentro de la lapida
    float anchoBotonDif = 300.f;
    float altoBotonDif = 65.f;
    float xBotonDif = 635.f;

    Boton d1;
    d1.forma.setSize(sf::Vector2f(anchoBotonDif, altoBotonDif));
    d1.forma.setPosition(sf::Vector2f(xBotonDif, 180.f));
    d1.forma.setRotation(sf::degrees(2.0f));
    d1.texto = "FACIL";
    d1.habilitado = true;
    botonesDificultad.push_back(d1);

    Boton d2;
    d2.forma.setSize(sf::Vector2f(anchoBotonDif, altoBotonDif));
    d2.forma.setPosition(sf::Vector2f(xBotonDif, 290.f));
    d2.forma.setRotation(sf::degrees(2.0f));
    d2.texto = "MEDIO";
    d2.habilitado = true;
    botonesDificultad.push_back(d2);

    Boton d3;
    d3.forma.setSize(sf::Vector2f(anchoBotonDif, altoBotonDif));
    d3.forma.setPosition(sf::Vector2f(xBotonDif, 400.f));
    d3.forma.setRotation(sf::degrees(2.0f));
    d3.texto = "DIFICIL";
    d3.habilitado = true;
    botonesDificultad.push_back(d3);

    dificultadSeleccionada = MEDIO;

    // Boton COMO JUGAR
    Boton b3;
    b3.forma.setSize(sf::Vector2f(anchoBoton, altoBoton));
    b3.forma.setPosition(sf::Vector2f(xBoton, 430.f));
    b3.forma.setRotation(sf::degrees(2.0f));
    b3.texto = "COMO JUGAR";
    b3.habilitado = true;
    botones.push_back(b3);

    paginaComoJugar = 0;
}

EstadoJuego Menu::confirmarBoton(int indice) {
    if (indice == 0) return EstadoJuego::CARGANDO;
    if (indice == 1) { botonSeleccionado = 1; return EstadoJuego::SELECCION_DIFICULTAD; }
    if (indice == 2) { paginaComoJugar = 0; return EstadoJuego::COMO_JUGAR; }
    return EstadoJuego::MENU;
}

EstadoJuego Menu::procesarEvento(const sf::Event& event) {
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos((float)click->position.x, (float)click->position.y);

            for (int i = 0; i < (int)botones.size(); i++) {
                if (botones[i].habilitado &&
                    botones[i].forma.getGlobalBounds().contains(pos)) {
                    botonSeleccionado = i;
                    return confirmarBoton(i);
                }
            }
        }
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Up && botonSeleccionado > 0) {
            botonSeleccionado--;
        }

        if (key->code == sf::Keyboard::Key::Down &&
            botonSeleccionado < (int)botones.size() - 1) {
            botonSeleccionado++;
        }

        if (key->code == sf::Keyboard::Key::Enter ||
            key->code == sf::Keyboard::Key::Space) {
            if (botones[botonSeleccionado].habilitado) {
                return confirmarBoton(botonSeleccionado);
            }
        }
    }

    return EstadoJuego::MENU;
}

void Menu::dibujar() {
    if (fondoCargado) {
        ventana.draw(spriteFondo);
    }
    else {
        ventana.clear(sf::Color(15, 15, 25));
    }

    dibujarTitulo();

    sf::Vector2i mousePx = sf::Mouse::getPosition(ventana);
    sf::Vector2f mouse((float)mousePx.x, (float)mousePx.y);

    for (int i = 0; i < (int)botones.size(); i++) {
        bool hover = botones[i].habilitado &&
            botones[i].forma.getGlobalBounds().contains(mouse);

        bool sel = (i == botonSeleccionado);

        dibujarBoton(botones[i], hover || sel);
    }
}

void Menu::dibujarTitulo() {
    // De momento se deja vacio porque el titulo ya forma parte del fondo del menu.
}

void Menu::dibujarBoton(const Boton& b, bool seleccionado) {
    sf::RectangleShape forma = b.forma;

    // Boton transparente, solo sirve como zona clicable
    forma.setFillColor(sf::Color::Transparent);
    forma.setOutlineThickness(0.f);
    ventana.draw(forma);

    sf::Text texto(fuente, b.texto, 40);

    if (!b.habilitado) {
        texto.setFillColor(colorTextoDeshabilitado);
    }
    else if (seleccionado) {
        texto.setFillColor(sf::Color(200, 255, 50));
    }
    else {
        texto.setFillColor(colorTexto);
    }

    texto.setRotation(forma.getRotation());

    sf::FloatRect textBounds = texto.getLocalBounds();

    texto.setOrigin(sf::Vector2f(
        textBounds.position.x + textBounds.size.x / 2.f,
        textBounds.position.y + textBounds.size.y / 2.f
    ));

    sf::FloatRect formaBounds = forma.getLocalBounds();

    sf::Vector2f centerLocal(
        formaBounds.size.x / 2.f,
        formaBounds.size.y / 2.f
    );

    sf::Vector2f centerGlobal = forma.getTransform().transformPoint(centerLocal);

    texto.setPosition(centerGlobal);

    ventana.draw(texto);
}

EstadoJuego Menu::procesarEventoDificultad(const sf::Event& event) {
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos((float)click->position.x, (float)click->position.y);

            for (int i = 0; i < (int)botonesDificultad.size(); i++) {
                if (botonesDificultad[i].habilitado &&
                    botonesDificultad[i].forma.getGlobalBounds().contains(pos)) {

                    botonSeleccionado = i;

                    switch (i) {
                    case 0:
                        dificultadSeleccionada = FACIL;
                        break;
                    case 1:
                        dificultadSeleccionada = MEDIO;
                        break;
                    case 2:
                        dificultadSeleccionada = DIFICIL;
                        break;
                    }

                    return EstadoJuego::JUGANDO_IA;
                }
            }
        }
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            botonSeleccionado = 0;
            return EstadoJuego::MENU;
        }

        if (key->code == sf::Keyboard::Key::Up && botonSeleccionado > 0) {
            botonSeleccionado--;
        }

        if (key->code == sf::Keyboard::Key::Down &&
            botonSeleccionado < (int)botonesDificultad.size() - 1) {
            botonSeleccionado++;
        }

        if (key->code == sf::Keyboard::Key::Enter ||
            key->code == sf::Keyboard::Key::Space) {

            switch (botonSeleccionado) {
            case 0:
                dificultadSeleccionada = FACIL;
                break;
            case 1:
                dificultadSeleccionada = MEDIO;
                break;
            case 2:
                dificultadSeleccionada = DIFICIL;
                break;
            }

            return EstadoJuego::JUGANDO_IA;
        }
    }

    return EstadoJuego::SELECCION_DIFICULTAD;
}

void Menu::dibujarDificultad() {
    if (fondoCargado) {
        ventana.draw(spriteFondo);
    }
    else {
        ventana.clear(sf::Color(15, 15, 25));
    }

    sf::Vector2i mousePx = sf::Mouse::getPosition(ventana);
    sf::Vector2f mouse((float)mousePx.x, (float)mousePx.y);

    for (int i = 0; i < (int)botonesDificultad.size(); i++) {
        bool hover = botonesDificultad[i].forma.getGlobalBounds().contains(mouse);
        bool sel = (i == botonSeleccionado);

        dibujarBoton(botonesDificultad[i], hover || sel);
    }
}

void Menu::dibujarSubmenuDificultad() {
    dibujarDificultad();
}

// ── COMO JUGAR ──────────────────────────────────────────────────────────────

EstadoJuego Menu::procesarEventoComoJugar(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape)
            return EstadoJuego::MENU;
        if (key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::A) {
            if (paginaComoJugar > 0) paginaComoJugar--;
        }
        if (key->code == sf::Keyboard::Key::Right || key->code == sf::Keyboard::Key::D) {
            if (paginaComoJugar < TOTAL_PAGINAS - 1) paginaComoJugar++;
        }
    }
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            float x = (float)click->position.x;
            // Click en mitad izquierda → pagina anterior, mitad derecha → siguiente
            if (x < VENTANA_ANCHO / 2.f) { if (paginaComoJugar > 0) paginaComoJugar--; }
            else                          { if (paginaComoJugar < TOTAL_PAGINAS - 1) paginaComoJugar++; }
        }
    }
    return EstadoJuego::COMO_JUGAR;
}

void Menu::dibujarComoJugar() {
    // Fondo
    if (fondoCargado) ventana.draw(spriteFondo);
    else              ventana.clear(sf::Color(15, 15, 25));

    // Overlay semitransparente para mejorar legibilidad
    sf::RectangleShape overlay(sf::Vector2f(VENTANA_ANCHO, VENTANA_ALTO));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    ventana.draw(overlay);

    dibujarPaginaComoJugar(paginaComoJugar);
}

void Menu::dibujarPaginaComoJugar(int pagina) {
    static const std::string titulos[4] = {
        "MECANICAS BASICAS",
        "LAS PIEZAS",
        "LA ARENA DE COMBATE",
        "LOS HECHIZOS"
    };

    static const std::string contenidos[4] = {
        "El tablero es una cuadricula de 9x9 casillas.\n"
        "Cada turno, un jugador mueve una pieza.\n"
        "LUZ (plantas) empieza a la izquierda, OSCURIDAD (zombies) a la derecha.\n\n"
        "Hay 5 PUNTOS DE PODER marcados con diamantes dorados:\n"
        "el centro y los 4 centros de los bordes.\n\n"
        "Se gana de dos formas:\n"
        "- Eliminar todas las piezas del enemigo.\n"
        "- Ocupar los 5 puntos de poder a la vez.\n\n"
        "Las casillas grises cambian de color periodicamente\n"
        "(ciclo de oscilacion) afectando a las bonificaciones.",

        "Hay 6 tipos de piezas por bando (+ 1 lider):\n\n"
        "TANQUE  (Pomelo / All-Star)     Vida alta, movimiento lento.\n"
        "MELEE   (Carnivora / Supercerebroz)  Daño alto, rango corto.\n"
        "DISTANCIA (Lanzaguisantes / Soldado)  Dispara lejos.\n"
        "RAPIDA  (Frutaestrella / Zombidito)  Velocidad alta.\n"
        "VOLADORA (Mazorca / Ingeniero)  Puede saltar piezas.\n"
        "LIDER   (Dave el Loco / Dr. Zomboss) Teletransporte.\n\n"
        "Al seleccionar una pieza se muestran sus casillas\n"
        "de movimiento disponibles en verde.",

        "Cuando dos piezas enemigas chocan en una casilla\n"
        "se abre una ARENA de combate en tiempo real.\n\n"
        "PLANTAS (LUZ):   WASD para moverse  |  ESPACIO para disparar.\n"
        "ZOMBIES (OSC):   Flechas para moverse  |  ENTER para disparar.\n\n"
        "Gana quien reduzca la vida del rival a 0 primero.\n"
        "Si ambos mueren a la vez, la casilla queda libre.\n\n"
        "Los obstaculos del escenario se generan al azar.\n"
        "Puedes esquivarlos o usarlos como cover.",

        "Pulsa H durante la partida para abrir el panel de hechizos.\n"
        "Cada bando tiene 7 hechizos de un solo uso (teclas 1-7).\n\n"
        "1 - CURACION:    Recupera vida de una pieza aliada.\n"
        "2 - TELEPORTE:   Mueve una pieza aliada a cualquier casilla.\n"
        "3 - DANO:        Reduce la vida de una pieza enemiga.\n"
        "4 - RALENTIZAR:  Reduce la velocidad de ataque enemiga.\n"
        "5 - FORTALECER:  Aumenta la fuerza de una pieza aliada.\n"
        "6 - ESCUDO:      Da resistencia extra a una pieza aliada.\n"
        "7 - CONGELAR:    Inmoviliza completamente a un enemigo."
    };

    // Panel central
    float pw = 860.f, ph = 600.f;
    float px = (VENTANA_ANCHO - pw) / 2.f;
    float py = (VENTANA_ALTO  - ph) / 2.f;

    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(sf::Vector2f(px, py));
    panel.setFillColor(sf::Color(20, 20, 35, 230));
    panel.setOutlineColor(sf::Color(100, 100, 180));
    panel.setOutlineThickness(3.f);
    ventana.draw(panel);

    // Titulo de pagina
    sf::Text txtTitulo(fuente, titulos[pagina], 34);
    txtTitulo.setFillColor(sf::Color(180, 200, 255));
    txtTitulo.setStyle(sf::Text::Bold);
    sf::FloatRect tb = txtTitulo.getLocalBounds();
    txtTitulo.setPosition(sf::Vector2f(VENTANA_ANCHO / 2.f - tb.size.x / 2.f, py + 20.f));
    ventana.draw(txtTitulo);

    // Separador
    sf::RectangleShape sep(sf::Vector2f(pw - 60.f, 2.f));
    sep.setPosition(sf::Vector2f(px + 30.f, py + 70.f));
    sep.setFillColor(sf::Color(100, 100, 180, 180));
    ventana.draw(sep);

    // Contenido (linea a linea)
    float yTexto = py + 85.f;
    std::string linea;
    std::istringstream stream(contenidos[pagina]);
    while (std::getline(stream, linea)) {
        if (!linea.empty()) {
            sf::Text t(fuente, linea, 20);
            t.setFillColor(sf::Color(220, 220, 220));
            t.setPosition(sf::Vector2f(px + 30.f, yTexto));
            ventana.draw(t);
        }
        yTexto += 28.f;
    }

    // Indicador de pagina
    std::string indicador = std::to_string(pagina + 1) + " / " + std::to_string(TOTAL_PAGINAS);
    sf::Text txtPag(fuente, indicador, 22);
    txtPag.setFillColor(sf::Color(160, 160, 200));
    sf::FloatRect pb2 = txtPag.getLocalBounds();
    txtPag.setPosition(sf::Vector2f(VENTANA_ANCHO / 2.f - pb2.size.x / 2.f, py + ph - 60.f));
    ventana.draw(txtPag);

    // Flechas de navegacion
    if (pagina > 0) {
        sf::Text fIzq(fuente, "< ANTERIOR", 20);
        fIzq.setFillColor(sf::Color(180, 180, 255));
        fIzq.setPosition(sf::Vector2f(px + 20.f, py + ph - 60.f));
        ventana.draw(fIzq);
    }
    if (pagina < TOTAL_PAGINAS - 1) {
        sf::Text fDer(fuente, "SIGUIENTE >", 20);
        fDer.setFillColor(sf::Color(180, 180, 255));
        sf::FloatRect fd = fDer.getLocalBounds();
        fDer.setPosition(sf::Vector2f(px + pw - fd.size.x - 20.f, py + ph - 60.f));
        ventana.draw(fDer);
    }

    // Esc para volver
    sf::Text txtEsc(fuente, "ESC: Volver al menu", 18);
    txtEsc.setFillColor(sf::Color(130, 130, 150));
    sf::FloatRect eb = txtEsc.getLocalBounds();
    txtEsc.setPosition(sf::Vector2f(VENTANA_ANCHO / 2.f - eb.size.x / 2.f, py + ph + 10.f));
    ventana.draw(txtEsc);
}