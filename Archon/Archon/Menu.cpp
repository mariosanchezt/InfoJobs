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
        "TABLERO Y REGLAS",
        "PIEZAS Y MOVIMIENTO",
        "ARENA DE COMBATE",
        "HECHIZOS"
    };

    static const std::string contenidos[4] = {
        // Pagina 1 - Tablero y reglas
        "OBJETIVO: gana el primero que cumpla una de estas dos condiciones:\n"
        "  - Eliminar todas las piezas enemigas.\n"
        "  - Ocupar los 5 Puntos de Poder al mismo tiempo.\n\n"
        "PUNTOS DE PODER: las 5 casillas con diamante dorado\n"
        "  (centro del tablero + centros de los 4 bordes).\n"
        "  Los paneles laterales muestran cuantos controla cada bando.\n\n"
        "TURNOS: LUZ (plantas) siempre empieza. Se alternan.\n"
        "  Solo puedes mover tus propias piezas.\n\n"
        "CICLO DE OSCILACION: las casillas grises cambian de color\n"
        "  periodicamente. El color de la casilla puede dar ventaja\n"
        "  o desventaja en el combate segun el bando.\n\n"
        "CONTROLES TABLERO:\n"
        "  Raton:   click en pieza para seleccionar, click en destino para mover.\n"
        "  Teclado: WASD / Flechas para mover cursor  |  Espacio / Enter para confirmar.\n"
        "  P: pausar  |  H: abrir panel de hechizos  |  Esc: deseleccionar / cancelar.",

        // Pagina 2 - Piezas y movimiento
        "Hay 6 tipos de pieza por bando mas un lider especial:\n\n"
        "  TANQUE    Pomelo / All-Star\n"
        "            Vida muy alta. Lento (radio 2). Aguanta bien en puntos de poder.\n\n"
        "  MELEE     Carnivora / Supercerebroz\n"
        "            Daño muy alto. Radio 3-4. Ideal para eliminar piezas.\n\n"
        "  DISTANCIA Lanzaguisantes / Soldado\n"
        "            Dispara lejos. Radio de movimiento 4. Bueno para cubrir zona.\n\n"
        "  RAPIDA    Frutaestrella / Zombidito\n"
        "            Velocidad de ataque alta. Util para hostigar al enemigo.\n\n"
        "  VOLADORA  Mazorca / Ingeniero\n"
        "            Puede saltar por encima de otras piezas al moverse.\n\n"
        "  LIDER     Dave el Loco / Dr. Zomboss\n"
        "            Se teletransporta a cualquier casilla libre del tablero.\n\n"
        "Al seleccionar una pieza, los cuadros VERDES indican a donde puede ir.",

        // Pagina 3 - Arena
        "Cuando mueves una pieza a una casilla ocupada por el enemigo\n"
        "se abre la ARENA: un combate en tiempo real entre ambas piezas.\n\n"
        "CONTROLES EN LA ARENA:\n"
        "  PLANTAS (LUZ)     Moverse: W A S D        Disparar: ESPACIO\n"
        "  ZOMBIES (OSC)     Moverse: flechas         Disparar: ENTER\n\n"
        "MECANICAS:\n"
        "  - Cada disparo tiene tiempo de recarga, no puedes spamear.\n"
        "  - Los proyectiles se destruyen al chocar con obstaculos.\n"
        "  - Los obstaculos del escenario se generan al azar cada combate.\n"
        "  - La vida de las piezas que entran al combate es la que tienen\n"
        "    en ese momento (puede estar reducida por hechizos previos).\n\n"
        "RESULTADO:\n"
        "  - Gana quien deje al rival sin vida.\n"
        "  - Si los dos mueren a la vez, la casilla queda vacia.\n"
        "  - El ganador ocupa la casilla del perdedor.",

        // Pagina 4 - Hechizos
        "Pulsa H para abrir el panel de hechizos durante tu turno.\n"
        "Cada bando tiene 7 hechizos, cada uno de UN SOLO USO (teclas 1-7).\n"
        "Usar un hechizo consume el turno igual que mover una pieza.\n\n"
        "  1 - CURACION      Recupera vida de una pieza aliada.\n"
        "  2 - TELEPORTE     Mueve una pieza aliada a cualquier casilla libre.\n"
        "                    Primero seleccionas la pieza, luego el destino.\n"
        "  3 - DANO          Reduce directamente la vida de una pieza enemiga.\n"
        "  4 - RALENTIZAR    Reduce la velocidad de ataque de una pieza enemiga.\n"
        "  5 - FORTALECER    Aumenta la fuerza de ataque de una pieza aliada.\n"
        "  6 - ESCUDO        Aumenta la resistencia de una pieza aliada.\n"
        "  7 - CONGELAR      Inmoviliza una pieza enemiga durante un turno.\n\n"
        "Los efectos activos se muestran con un borde de color sobre la pieza:\n"
        "  Azul claro = congelada  |  Amarillo = ralentizada\n"
        "  Dorado = fortalecida    |  Azul oscuro = escudo"
    };

    // Panel central — alto ampliado para que quepa todo el texto
    float pw = 880.f, ph = 720.f;
    float px = (VENTANA_ANCHO - pw) / 2.f;
    float py = (VENTANA_ALTO  - ph) / 2.f;

    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(sf::Vector2f(px, py));
    panel.setFillColor(sf::Color(20, 20, 35, 230));
    panel.setOutlineColor(sf::Color(100, 100, 180));
    panel.setOutlineThickness(3.f);
    ventana.draw(panel);

    // Titulo de pagina
    sf::Text txtTitulo(fuente, titulos[pagina], 32);
    txtTitulo.setFillColor(sf::Color(180, 200, 255));
    txtTitulo.setStyle(sf::Text::Bold);
    sf::FloatRect tb = txtTitulo.getLocalBounds();
    txtTitulo.setPosition(sf::Vector2f(VENTANA_ANCHO / 2.f - tb.size.x / 2.f, py + 18.f));
    ventana.draw(txtTitulo);

    // Separador
    sf::RectangleShape sep(sf::Vector2f(pw - 60.f, 2.f));
    sep.setPosition(sf::Vector2f(px + 30.f, py + 62.f));
    sep.setFillColor(sf::Color(100, 100, 180, 180));
    ventana.draw(sep);

    // Contenido: lineas normales 22px, lineas vacias solo 8px
    float yTexto = py + 74.f;
    std::string linea;
    std::istringstream stream(contenidos[pagina]);
    while (std::getline(stream, linea)) {
        if (!linea.empty()) {
            sf::Text t(fuente, linea, 17);
            t.setFillColor(sf::Color(220, 220, 220));
            t.setPosition(sf::Vector2f(px + 28.f, yTexto));
            ventana.draw(t);
            yTexto += 23.f;
        }
        else {
            yTexto += 8.f;
        }
    }

    // Separador inferior
    sf::RectangleShape sep2(sf::Vector2f(pw - 60.f, 1.f));
    sep2.setPosition(sf::Vector2f(px + 30.f, py + ph - 68.f));
    sep2.setFillColor(sf::Color(80, 80, 140, 140));
    ventana.draw(sep2);

    // Fila inferior: ANTERIOR  |  pagina X/4  |  SIGUIENTE  +  ESC
    float yNav = py + ph - 52.f;

    if (pagina > 0) {
        sf::Text fIzq(fuente, "< ANTERIOR", 19);
        fIzq.setFillColor(sf::Color(160, 160, 255));
        fIzq.setPosition(sf::Vector2f(px + 20.f, yNav));
        ventana.draw(fIzq);
    }

    std::string indicador = std::to_string(pagina + 1) + " / " + std::to_string(TOTAL_PAGINAS);
    sf::Text txtPag(fuente, indicador, 19);
    txtPag.setFillColor(sf::Color(140, 140, 190));
    sf::FloatRect pb2 = txtPag.getLocalBounds();
    txtPag.setPosition(sf::Vector2f(VENTANA_ANCHO / 2.f - pb2.size.x / 2.f, yNav));
    ventana.draw(txtPag);

    if (pagina < TOTAL_PAGINAS - 1) {
        sf::Text fDer(fuente, "SIGUIENTE >", 19);
        fDer.setFillColor(sf::Color(160, 160, 255));
        sf::FloatRect fd = fDer.getLocalBounds();
        fDer.setPosition(sf::Vector2f(px + pw - fd.size.x - 20.f, yNav));
        ventana.draw(fDer);
    }

    // ESC para volver — dentro del panel, debajo de la navegacion
    sf::Text txtEsc(fuente, "[ ESC ]  Volver al menu", 17);
    txtEsc.setFillColor(sf::Color(110, 110, 150));
    sf::FloatRect eb = txtEsc.getLocalBounds();
    txtEsc.setPosition(sf::Vector2f(VENTANA_ANCHO / 2.f - eb.size.x / 2.f, py + ph - 26.f));
    ventana.draw(txtEsc);
}