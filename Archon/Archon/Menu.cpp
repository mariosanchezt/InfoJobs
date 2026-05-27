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
            sf::Vector2f pos = ventana.mapPixelToCoords(
                sf::Vector2i(click->position.x, click->position.y),
                ventana.getView()
            );

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
    sf::Vector2f mouse = ventana.mapPixelToCoords(mousePx, ventana.getView());

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
            sf::Vector2f pos = ventana.mapPixelToCoords(
                sf::Vector2i(click->position.x, click->position.y),
                ventana.getView()
            );

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
    sf::Vector2f mouse = ventana.mapPixelToCoords(mousePx, ventana.getView());

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

        if (key->code == sf::Keyboard::Key::Left ||
            key->code == sf::Keyboard::Key::A) {
            if (paginaComoJugar > 0) paginaComoJugar--;
        }

        if (key->code == sf::Keyboard::Key::Right ||
            key->code == sf::Keyboard::Key::D) {
            if (paginaComoJugar < TOTAL_PAGINAS - 1) paginaComoJugar++;
        }
    }

    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = ventana.mapPixelToCoords(
                sf::Vector2i(click->position.x, click->position.y),
                ventana.getView()
            );

            float x = pos.x;

            // Click en mitad izquierda -> pagina anterior, mitad derecha -> siguiente
            if (x < VENTANA_ANCHO / 2.f) {
                if (paginaComoJugar > 0) paginaComoJugar--;
            }
            else {
                if (paginaComoJugar < TOTAL_PAGINAS - 1) paginaComoJugar++;
            }
        }
    }

    return EstadoJuego::COMO_JUGAR;
}

void Menu::dibujarComoJugar() {
    // Fondo
    if (fondoCargado) {
        ventana.draw(spriteFondo);
    }
    else {
        ventana.clear(sf::Color(15, 15, 25));
    }

    // Overlay mas oscuro para mejorar la lectura del texto
    sf::RectangleShape overlay(sf::Vector2f(VENTANA_ANCHO, VENTANA_ALTO));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));
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
        "#OBJETIVO\n"
        "Gana el primer bando que cumpla una condicion:\n"
        "- Eliminar todas las piezas enemigas.\n"
        "- Controlar los 5 puntos de poder a la vez.\n\n"

        "#PUNTOS DE PODER\n"
        "Son las casillas con diamante dorado.\n"
        "Hay uno en el centro y cuatro en los bordes.\n"
        "Los paneles laterales indican cuantos controla cada bando.\n\n"

        "#TURNOS\n"
        "Las plantas empiezan la partida.\n"
        "Despues, los turnos se alternan.\n"
        "Solo puedes mover tus propias piezas.\n\n"

        "#CONTROLES DEL TABLERO\n"
        "Raton: click en una pieza y click en el destino.\n"
        "Teclado: WASD / flechas para mover el cursor.\n"
        "Espacio / Enter: seleccionar o confirmar.\n"
        "P: pausar   H: hechizos   Esc: cancelar.",

        // Pagina 2 - Piezas y movimiento
        "#TIPOS DE PIEZA\n"
        "Cada bando tiene piezas con funciones distintas.\n"
        "- Tanque: mucha vida y buena defensa.\n"
        "- Melee: mucho dano en combate directo.\n"
        "- Distancia: buena para atacar desde lejos.\n"
        "- Rapida: mas agilidad y velocidad de ataque.\n"
        "- Voladora: puede saltar por encima de piezas.\n"
        "- Lider: puede teletransportarse.\n\n"

        "#PERSONAJES\n"
        "Plantas: Pomelo, Carnivora, Lanzaguisantes,\n"
        "Girasol, Mazorca y Dave el Loco.\n"
        "Zombies: All-Star, Supercerebroz, Soldado,\n"
        "Zombidito, Ingeniero y Dr. Zomboss.\n\n"

        "#MOVIMIENTO\n"
        "Al seleccionar una pieza se muestran casillas validas.\n"
        "Las piezas GROUND no pueden saltar otras piezas.\n"
        "Las piezas FLYING si pueden saltarlas.\n"
        "Las piezas TELEPORT pueden ir a cualquier casilla libre.",

        // Pagina 3 - Arena
        "#ARENA DE COMBATE\n"
        "Si atacas una casilla con una pieza enemiga,\n"
        "se abre la arena de combate en tiempo real.\n\n"

        "#CONTROLES EN LA ARENA\n"
        "Plantas: W A S D para moverse y Espacio para disparar.\n"
        "Zombies: flechas para moverse y Enter para disparar.\n\n"

        "#MECANICAS\n"
        "Cada disparo tiene tiempo de recarga.\n"
        "Los proyectiles se destruyen al chocar con obstaculos.\n"
        "Las nueces bloquean disparos y movimiento.\n"
        "La vida al entrar en combate es la vida real de la pieza.\n\n"

        "#RESULTADO DEL COMBATE\n"
        "Gana quien deje al rival sin vida.\n"
        "Si los dos mueren, la casilla queda vacia.\n"
        "El ganador ocupa la casilla del perdedor.",

        // Pagina 4 - Hechizos
        "#HECHIZOS\n"
        "Pulsa H para abrir el panel de hechizos.\n"
        "Cada bando tiene 7 hechizos de un solo uso.\n"
        "Usar un hechizo tambien consume el turno.\n\n"

        "#LISTA DE HECHIZOS\n"
        "1 - Curacion: recupera vida de una pieza aliada.\n"
        "2 - Teleporte: mueve una pieza aliada a una casilla libre.\n"
        "3 - Dano: reduce vida de una pieza enemiga.\n"
        "4 - Ralentizar: reduce velocidad de ataque enemiga.\n"
        "5 - Fortalecer: aumenta la fuerza de una pieza aliada.\n"
        "6 - Escudo: protege a una pieza aliada.\n"
        "7 - Congelar: bloquea a una pieza enemiga.\n\n"

        "#EFECTOS VISUALES\n"
        "Los efectos se muestran con bordes de color:\n"
        "Azul claro = congelada    Amarillo = ralentizada\n"
        "Dorado = fortalecida      Azul oscuro = escudo"
    };

    // Panel mas grande para aprovechar mejor la pantalla
    float pw = 1060.f;
    float ph = 800.f;
    float px = (VENTANA_ANCHO - pw) / 2.f;
    float py = (VENTANA_ALTO - ph) / 2.f;

    // Sombra exterior
    sf::RectangleShape sombra(sf::Vector2f(pw + 12.f, ph + 12.f));
    sombra.setPosition(sf::Vector2f(px - 6.f, py + 6.f));
    sombra.setFillColor(sf::Color(0, 0, 0, 160));
    ventana.draw(sombra);

    // Panel central
    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(sf::Vector2f(px, py));
    panel.setFillColor(sf::Color(18, 18, 34, 245));
    panel.setOutlineColor(sf::Color(125, 125, 220));
    panel.setOutlineThickness(3.f);
    ventana.draw(panel);

    // Titulo de pagina
    sf::Text txtTitulo(fuente, titulos[pagina], 38);
    txtTitulo.setFillColor(sf::Color(200, 215, 255));
    txtTitulo.setOutlineColor(sf::Color(10, 10, 25));
    txtTitulo.setOutlineThickness(1.5f);
    txtTitulo.setStyle(sf::Text::Bold);

    sf::FloatRect tb = txtTitulo.getLocalBounds();
    txtTitulo.setPosition(sf::Vector2f(
        VENTANA_ANCHO / 2.f - tb.size.x / 2.f,
        py + 22.f
    ));
    ventana.draw(txtTitulo);

    // Separador superior
    sf::RectangleShape sep(sf::Vector2f(pw - 80.f, 2.f));
    sep.setPosition(sf::Vector2f(px + 40.f, py + 78.f));
    sep.setFillColor(sf::Color(120, 120, 210, 200));
    ventana.draw(sep);

    // Contenido
    float yTexto = py + 102.f;
    float xTexto = px + 55.f;

    std::string linea;
    std::istringstream stream(contenidos[pagina]);

    while (std::getline(stream, linea)) {
        if (linea.empty()) {
            yTexto += 10.f;
            continue;
        }

        bool esTituloSeccion = false;

        // Las lineas que empiezan por # son titulos internos.
        // El # no se dibuja, solo sirve para saber que esa linea va destacada.
        if (linea[0] == '#') {
            esTituloSeccion = true;
            linea = linea.substr(1);
        }

        unsigned int tamTexto = esTituloSeccion ? 29 : 24;

        sf::Text texto(fuente, linea, tamTexto);

        if (esTituloSeccion) {
            texto.setFillColor(sf::Color(190, 205, 255));
            texto.setStyle(sf::Text::Bold);
            texto.setOutlineColor(sf::Color(5, 5, 20));
            texto.setOutlineThickness(1.f);
        }
        else {
            texto.setFillColor(sf::Color(240, 240, 245));
            texto.setOutlineColor(sf::Color(5, 5, 20));
            texto.setOutlineThickness(0.8f);
        }

        texto.setPosition(sf::Vector2f(xTexto, yTexto));
        ventana.draw(texto);

        if (esTituloSeccion) {
            yTexto += 36.f;
        }
        else {
            yTexto += 30.f;
        }
    }

    // Separador inferior
    sf::RectangleShape sep2(sf::Vector2f(pw - 80.f, 1.f));
    sep2.setPosition(sf::Vector2f(px + 40.f, py + ph - 76.f));
    sep2.setFillColor(sf::Color(90, 90, 160, 160));
    ventana.draw(sep2);

    // Navegacion inferior
    float yNav = py + ph - 57.f;

    if (pagina > 0) {
        sf::Text fIzq(fuente, "< ANTERIOR", 22);
        fIzq.setFillColor(sf::Color(180, 180, 255));
        fIzq.setOutlineColor(sf::Color(5, 5, 20));
        fIzq.setOutlineThickness(1.f);
        fIzq.setStyle(sf::Text::Bold);
        fIzq.setPosition(sf::Vector2f(px + 35.f, yNav));
        ventana.draw(fIzq);
    }

    std::string indicador = std::to_string(pagina + 1) + " / " + std::to_string(TOTAL_PAGINAS);

    sf::Text txtPag(fuente, indicador, 22);
    txtPag.setFillColor(sf::Color(180, 180, 220));
    txtPag.setOutlineColor(sf::Color(5, 5, 20));
    txtPag.setOutlineThickness(1.f);
    txtPag.setStyle(sf::Text::Bold);

    sf::FloatRect pb2 = txtPag.getLocalBounds();
    txtPag.setPosition(sf::Vector2f(
        VENTANA_ANCHO / 2.f - pb2.size.x / 2.f,
        yNav
    ));
    ventana.draw(txtPag);

    if (pagina < TOTAL_PAGINAS - 1) {
        sf::Text fDer(fuente, "SIGUIENTE >", 22);
        fDer.setFillColor(sf::Color(180, 180, 255));
        fDer.setOutlineColor(sf::Color(5, 5, 20));
        fDer.setOutlineThickness(1.f);
        fDer.setStyle(sf::Text::Bold);

        sf::FloatRect fd = fDer.getLocalBounds();
        fDer.setPosition(sf::Vector2f(
            px + pw - fd.size.x - 35.f,
            yNav
        ));
        ventana.draw(fDer);
    }

    // ESC para volver
    sf::Text txtEsc(fuente, "[ ESC ]  Volver al menu", 18);
    txtEsc.setFillColor(sf::Color(150, 150, 190));
    txtEsc.setOutlineColor(sf::Color(5, 5, 20));
    txtEsc.setOutlineThickness(0.8f);

    sf::FloatRect eb = txtEsc.getLocalBounds();
    txtEsc.setPosition(sf::Vector2f(
        VENTANA_ANCHO / 2.f - eb.size.x / 2.f,
        py + ph - 28.f
    ));
    ventana.draw(txtEsc);
}