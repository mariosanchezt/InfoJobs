#include "Menu.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "IAJugador.h"

static constexpr float VENTANA_ANCHO = 1200.f;
static constexpr float VENTANA_ALTO = 900.f;

Menu::Menu(sf::RenderWindow& vent, sf::Font& f)
    : ventana(vent), fuente(f), spriteFondo(texFondo), spriteConfiguracion(texConfiguracion), spritePlantaTriste(texPlantaTriste)
{
    botonSeleccionado = 0;
    fondoCargado = false;
    configuracionCargada = false;
    plantaTristeCargada = false;
    opcionSalidaSeleccionada = 1; // Por defecto seleccionamos NO

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

    // Icono de configuracion
    if (texConfiguracion.loadFromFile("assets/Configuracion.png")) {
        configuracionCargada = true;
        texConfiguracion.setSmooth(true);
        spriteConfiguracion = sf::Sprite(texConfiguracion);

        sf::Vector2u texSize = texConfiguracion.getSize();

        float tamIcono = 64.f;
        float escala = tamIcono / std::max((float)texSize.x, (float)texSize.y);

        spriteConfiguracion.setScale(sf::Vector2f(escala, escala));

        float spriteW = texSize.x * escala;
        float spriteH = texSize.y * escala;

        // Posicion aproximada sobre la etiqueta blanca de las llaves.
        // Si lo quieres mover, cambia estos valores.
        float iconoX = 680.f;
        float iconoY = 730.f;

        spriteConfiguracion.setPosition(sf::Vector2f(
            iconoX + (82.f - spriteW) / 2.f,
            iconoY + (82.f - spriteH) / 2.f
        ));

        zonaConfiguracion.setSize(sf::Vector2f(82.f, 82.f));
        zonaConfiguracion.setPosition(sf::Vector2f(iconoX, iconoY));
    }
    else {
        // Zona clicable aunque no cargue el PNG
        zonaConfiguracion.setSize(sf::Vector2f(82.f, 82.f));
        zonaConfiguracion.setPosition(sf::Vector2f(690.f, 760.f));
    }

    // Imagen de planta triste para confirmar salida
    if (texPlantaTriste.loadFromFile("assets/PlantaTriste.png")) {
        plantaTristeCargada = true;
        texPlantaTriste.setSmooth(true);
        spritePlantaTriste = sf::Sprite(texPlantaTriste);

        sf::Vector2u texSize = texPlantaTriste.getSize();

        float tamPlanta = 150.f;
        float escala = tamPlanta / std::max((float)texSize.x, (float)texSize.y);

        spritePlantaTriste.setScale(sf::Vector2f(escala, escala));

        float spriteW = texSize.x * escala;
        float spriteH = texSize.y * escala;

        spritePlantaTriste.setPosition(sf::Vector2f(
            VENTANA_ANCHO / 2.f - spriteW / 2.f,
            405.f - spriteH / 2.f
        ));
    }

    colorTexto = sf::Color(30, 30, 30);
    colorTextoDeshabilitado = sf::Color(90, 90, 90, 200);

    colorBotonActivo = sf::Color(255, 255, 255);
    colorBotonDeshabilitado = sf::Color(100, 100, 100);
    colorBotonHover = sf::Color(200, 255, 50);

    float anchoBoton = 300.f;
    float altoBoton = 65.f;

    // Botones dentro de la lapida
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

    // Botones del submenu de dificultad
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

    // Boton HISTORIAL
    Boton b4;
    b4.forma.setSize(sf::Vector2f(anchoBoton, altoBoton));
    b4.forma.setPosition(sf::Vector2f(xBoton, 540.f));
    b4.forma.setRotation(sf::degrees(2.0f));
    b4.texto = "HISTORIAL";
    b4.habilitado = true;
    botones.push_back(b4);

    paginaComoJugar = 0;

    // Valores iniciales de configuracion
    opcionConfiguracionSeleccionada = 0;
    volumenGeneral = 100.f;
    volumenMusica = 25.f;
    volumenEfectos = 50.f;
    sonidoSilenciado = false;
    pantallaCompletaActivada = false;
    solicitudCambiarPantallaCompleta = false;
}

EstadoJuego Menu::confirmarBoton(int indice) {
    if (indice == 0) return EstadoJuego::CARGANDO;
    if (indice == 1) { botonSeleccionado = 1; return EstadoJuego::SELECCION_DIFICULTAD; }
    if (indice == 2) { paginaComoJugar = 0; return EstadoJuego::COMO_JUGAR; }
    if (indice == 3) { entradasRanking = Ranking::cargarPartidas(10); return EstadoJuego::RANKING; }
    return EstadoJuego::MENU;
}

EstadoJuego Menu::procesarEvento(const sf::Event& event) {
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = ventana.mapPixelToCoords(
                sf::Vector2i(click->position.x, click->position.y),
                ventana.getView()
            );

            // Click en el icono de configuracion
            if (zonaConfiguracion.getGlobalBounds().contains(pos)) {
                opcionConfiguracionSeleccionada = 0;
                return EstadoJuego::CONFIGURACION;
            }

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
        if (key->code == sf::Keyboard::Key::Escape) {
            opcionSalidaSeleccionada = 1;
            return EstadoJuego::CONFIRMAR_SALIDA;
        }

        if (key->code == sf::Keyboard::Key::Up && botonSeleccionado > 0) {
            botonSeleccionado--;
        }

        if (key->code == sf::Keyboard::Key::Down &&
            botonSeleccionado < (int)botones.size() - 1) {
            botonSeleccionado++;
        }

        // Tecla C como acceso rapido a configuracion
        if (key->code == sf::Keyboard::Key::C) {
            opcionConfiguracionSeleccionada = 0;
            return EstadoJuego::CONFIGURACION;
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

    // Dibujar icono de configuracion encima de la etiqueta blanca
    bool hoverConfig = zonaConfiguracion.getGlobalBounds().contains(mouse);

    if (hoverConfig) {
        sf::CircleShape brillo(42.f);
        brillo.setFillColor(sf::Color(200, 255, 50, 80));
        brillo.setOutlineColor(sf::Color(200, 255, 50, 180));
        brillo.setOutlineThickness(3.f);
        brillo.setPosition(sf::Vector2f(
            zonaConfiguracion.getPosition().x - 1.f,
            zonaConfiguracion.getPosition().y - 1.f
        ));
        ventana.draw(brillo);
    }

    if (configuracionCargada) {
        ventana.draw(spriteConfiguracion);
    }
    else {
        sf::Text fallback(fuente, "CFG", 24);
        fallback.setFillColor(hoverConfig ? sf::Color(200, 255, 50) : sf::Color::Black);
        fallback.setStyle(sf::Text::Bold);

        sf::FloatRect b = fallback.getLocalBounds();
        fallback.setPosition(sf::Vector2f(
            zonaConfiguracion.getPosition().x + zonaConfiguracion.getSize().x / 2.f - b.size.x / 2.f,
            zonaConfiguracion.getPosition().y + zonaConfiguracion.getSize().y / 2.f - b.size.y / 2.f
        ));
        ventana.draw(fallback);
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

// COMO JUGAR

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
    if (fondoCargado) {
        ventana.draw(spriteFondo);
    }
    else {
        ventana.clear(sf::Color(15, 15, 25));
    }

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

        "#TIPOS DE PIEZA\n"
        "Cada bando tiene piezas con funciones distintas.\n"
        "- Tanque: mucha vida y buena defensa.\n"
        "- Melee: mucho dano en combate directo.\n"
        "- Distancia: buena para atacar desde lejos.\n"
        "- Rapida: mas agilidad y velocidad de ataque.\n"
        "- Voladora: puede saltar por encima de piezas.\n"
        "- Lider: puede teletransportarse.\n\n"

        "#PERSONAJES\n"
        "Plantas: Cactus, Carnivora, Lanzaguisantes,\n"
        "Girasol, LanzaLechugas y Mazorca.\n"
        "Zombies: All-Star, Yeti, Minero,\n"
        "Balloon, Catapulta y Dr. Zomboss.\n\n"

        "#MOVIMIENTO\n"
        "Al seleccionar una pieza se muestran casillas validas.\n"
        "Las piezas GROUND no pueden saltar otras piezas.\n"
        "Las piezas FLYING si pueden saltarlas.\n"
        "Las piezas TELEPORT pueden ir a cualquier casilla libre.",

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

    float pw = 1060.f;
    float ph = 800.f;
    float px = (VENTANA_ANCHO - pw) / 2.f;
    float py = (VENTANA_ALTO - ph) / 2.f;

    sf::RectangleShape sombra(sf::Vector2f(pw + 12.f, ph + 12.f));
    sombra.setPosition(sf::Vector2f(px - 6.f, py + 6.f));
    sombra.setFillColor(sf::Color(0, 0, 0, 160));
    ventana.draw(sombra);

    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(sf::Vector2f(px, py));
    panel.setFillColor(sf::Color(18, 18, 34, 245));
    panel.setOutlineColor(sf::Color(125, 125, 220));
    panel.setOutlineThickness(3.f);
    ventana.draw(panel);

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

    sf::RectangleShape sep(sf::Vector2f(pw - 80.f, 2.f));
    sep.setPosition(sf::Vector2f(px + 40.f, py + 78.f));
    sep.setFillColor(sf::Color(120, 120, 210, 200));
    ventana.draw(sep);

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

        if (esTituloSeccion) yTexto += 36.f;
        else yTexto += 30.f;
    }

    sf::RectangleShape sep2(sf::Vector2f(pw - 80.f, 1.f));
    sep2.setPosition(sf::Vector2f(px + 40.f, py + ph - 76.f));
    sep2.setFillColor(sf::Color(90, 90, 160, 160));
    ventana.draw(sep2);

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

// CONFIGURACION

void Menu::restablecerConfiguracion() {
    volumenGeneral = 100.f;
    volumenMusica = 25.f;
    volumenEfectos = 50.f;
    sonidoSilenciado = false;
    pantallaCompletaActivada = false;
    solicitudCambiarPantallaCompleta = true;
}

EstadoJuego Menu::procesarEventoConfiguracion(const sf::Event& event) {
    auto cambiarVolumen = [&](float& valor, float cambio) {
        valor += cambio;
        if (valor < 0.f) valor = 0.f;
        if (valor > 100.f) valor = 100.f;
        };

    auto activarOpcion = [&]() {
        switch (opcionConfiguracionSeleccionada) {
        case 3:
            sonidoSilenciado = !sonidoSilenciado;
            break;

        case 4:
            pantallaCompletaActivada = !pantallaCompletaActivada;
            solicitudCambiarPantallaCompleta = true;
            break;

        case 5:
            restablecerConfiguracion();
            break;

        case 6:
            return EstadoJuego::MENU;

        default:
            break;
        }

        return EstadoJuego::CONFIGURACION;
        };

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            return EstadoJuego::MENU;
        }

        if (key->code == sf::Keyboard::Key::Up ||
            key->code == sf::Keyboard::Key::W) {
            opcionConfiguracionSeleccionada--;

            if (opcionConfiguracionSeleccionada < 0) {
                opcionConfiguracionSeleccionada = TOTAL_OPCIONES_CONFIG - 1;
            }
        }

        if (key->code == sf::Keyboard::Key::Down ||
            key->code == sf::Keyboard::Key::S) {
            opcionConfiguracionSeleccionada++;

            if (opcionConfiguracionSeleccionada >= TOTAL_OPCIONES_CONFIG) {
                opcionConfiguracionSeleccionada = 0;
            }
        }

        if (key->code == sf::Keyboard::Key::Left ||
            key->code == sf::Keyboard::Key::A) {
            if (opcionConfiguracionSeleccionada == 0) cambiarVolumen(volumenGeneral, -5.f);
            if (opcionConfiguracionSeleccionada == 1) cambiarVolumen(volumenMusica, -5.f);
            if (opcionConfiguracionSeleccionada == 2) cambiarVolumen(volumenEfectos, -5.f);
        }

        if (key->code == sf::Keyboard::Key::Right ||
            key->code == sf::Keyboard::Key::D) {
            if (opcionConfiguracionSeleccionada == 0) cambiarVolumen(volumenGeneral, 5.f);
            if (opcionConfiguracionSeleccionada == 1) cambiarVolumen(volumenMusica, 5.f);
            if (opcionConfiguracionSeleccionada == 2) cambiarVolumen(volumenEfectos, 5.f);
        }

        if (key->code == sf::Keyboard::Key::Enter ||
            key->code == sf::Keyboard::Key::Space) {
            return activarOpcion();
        }
    }

    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = ventana.mapPixelToCoords(
                sf::Vector2i(click->position.x, click->position.y),
                ventana.getView()
            );

            float primeraY = 250.f;
            float separacion = 70.f;

            for (int i = 0; i < TOTAL_OPCIONES_CONFIG; i++) {
                float y = primeraY + i * separacion;

                sf::FloatRect fila(
                    sf::Vector2f(260.f, y - 12.f),
                    sf::Vector2f(680.f, 54.f)
                );

                if (fila.contains(pos)) {
                    opcionConfiguracionSeleccionada = i;

                    if (i == 0 || i == 1 || i == 2) {
                        if (pos.x < 665.f) {
                            if (i == 0) cambiarVolumen(volumenGeneral, -5.f);
                            if (i == 1) cambiarVolumen(volumenMusica, -5.f);
                            if (i == 2) cambiarVolumen(volumenEfectos, -5.f);
                        }
                        else {
                            if (i == 0) cambiarVolumen(volumenGeneral, 5.f);
                            if (i == 1) cambiarVolumen(volumenMusica, 5.f);
                            if (i == 2) cambiarVolumen(volumenEfectos, 5.f);
                        }
                    }
                    else {
                        return activarOpcion();
                    }
                }
            }
        }
    }

    return EstadoJuego::CONFIGURACION;
}

void Menu::dibujarFilaConfiguracion(
    const std::string& nombre,
    const std::string& valor,
    float x,
    float y,
    bool seleccionada
) {
    sf::Color colorFondo = seleccionada
        ? sf::Color(80, 90, 150, 190)
        : sf::Color(25, 25, 45, 170);

    sf::Color colorBorde = seleccionada
        ? sf::Color(200, 255, 50, 230)
        : sf::Color(100, 100, 160, 150);

    sf::RectangleShape fila(sf::Vector2f(680.f, 54.f));
    fila.setPosition(sf::Vector2f(x, y));
    fila.setFillColor(colorFondo);
    fila.setOutlineColor(colorBorde);
    fila.setOutlineThickness(seleccionada ? 3.f : 1.f);
    ventana.draw(fila);

    sf::Text txtNombre(fuente, nombre, 24);
    txtNombre.setFillColor(sf::Color(235, 235, 245));
    txtNombre.setOutlineColor(sf::Color(5, 5, 20));
    txtNombre.setOutlineThickness(0.8f);
    txtNombre.setStyle(sf::Text::Bold);
    txtNombre.setPosition(sf::Vector2f(x + 22.f, y + 13.f));
    ventana.draw(txtNombre);

    sf::Text txtValor(fuente, valor, 24);
    txtValor.setFillColor(seleccionada ? sf::Color(200, 255, 50) : sf::Color(220, 220, 230));
    txtValor.setOutlineColor(sf::Color(5, 5, 20));
    txtValor.setOutlineThickness(0.8f);
    txtValor.setStyle(sf::Text::Bold);

    sf::FloatRect bv = txtValor.getLocalBounds();
    txtValor.setPosition(sf::Vector2f(x + 650.f - bv.size.x, y + 13.f));
    ventana.draw(txtValor);
}

void Menu::dibujarBarraVolumen(
    float valor,
    float x,
    float y,
    bool seleccionada
) {
    float ancho = 230.f;
    float alto = 18.f;

    sf::RectangleShape fondo(sf::Vector2f(ancho, alto));
    fondo.setPosition(sf::Vector2f(x, y));
    fondo.setFillColor(sf::Color(40, 40, 55));
    fondo.setOutlineColor(seleccionada ? sf::Color(200, 255, 50) : sf::Color(120, 120, 150));
    fondo.setOutlineThickness(2.f);
    ventana.draw(fondo);

    float porcentaje = valor / 100.f;

    sf::RectangleShape relleno(sf::Vector2f(ancho * porcentaje, alto));
    relleno.setPosition(sf::Vector2f(x, y));
    relleno.setFillColor(seleccionada ? sf::Color(200, 255, 50) : sf::Color(130, 190, 255));
    ventana.draw(relleno);

    sf::Text texto(fuente, std::to_string((int)valor) + "%", 20);
    texto.setFillColor(sf::Color::White);
    texto.setOutlineColor(sf::Color(5, 5, 20));
    texto.setOutlineThickness(0.8f);

    sf::FloatRect b = texto.getLocalBounds();
    texto.setPosition(sf::Vector2f(
        x + ancho / 2.f - b.size.x / 2.f,
        y - 5.f
    ));
    ventana.draw(texto);
}

void Menu::dibujarConfiguracion() {
    if (fondoCargado) {
        ventana.draw(spriteFondo);
    }
    else {
        ventana.clear(sf::Color(15, 15, 25));
    }

    sf::RectangleShape overlay(sf::Vector2f(VENTANA_ANCHO, VENTANA_ALTO));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));
    ventana.draw(overlay);

    float pw = 820.f;
    float ph = 700.f;
    float px = (VENTANA_ANCHO - pw) / 2.f;
    float py = (VENTANA_ALTO - ph) / 2.f;

    sf::RectangleShape sombra(sf::Vector2f(pw + 14.f, ph + 14.f));
    sombra.setPosition(sf::Vector2f(px - 7.f, py + 7.f));
    sombra.setFillColor(sf::Color(0, 0, 0, 170));
    ventana.draw(sombra);

    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(sf::Vector2f(px, py));
    panel.setFillColor(sf::Color(18, 18, 34, 245));
    panel.setOutlineColor(sf::Color(125, 125, 220));
    panel.setOutlineThickness(3.f);
    ventana.draw(panel);

    sf::Text titulo(fuente, "CONFIGURACION", 44);
    titulo.setFillColor(sf::Color(200, 215, 255));
    titulo.setOutlineColor(sf::Color(10, 10, 25));
    titulo.setOutlineThickness(1.5f);
    titulo.setStyle(sf::Text::Bold);

    sf::FloatRect bt = titulo.getLocalBounds();
    titulo.setPosition(sf::Vector2f(
        VENTANA_ANCHO / 2.f - bt.size.x / 2.f,
        py + 28.f
    ));
    ventana.draw(titulo);

    sf::RectangleShape sep(sf::Vector2f(pw - 80.f, 2.f));
    sep.setPosition(sf::Vector2f(px + 40.f, py + 90.f));
    sep.setFillColor(sf::Color(120, 120, 210, 200));
    ventana.draw(sep);

    sf::Text ayuda(fuente, "Usa ARRIBA/ABAJO para elegir  |  IZQ/DER para cambiar valores  |  ENTER para activar", 18);
    ayuda.setFillColor(sf::Color(185, 185, 210));
    ayuda.setOutlineColor(sf::Color(5, 5, 20));
    ayuda.setOutlineThickness(0.7f);

    sf::FloatRect ba = ayuda.getLocalBounds();
    ayuda.setPosition(sf::Vector2f(
        VENTANA_ANCHO / 2.f - ba.size.x / 2.f,
        py + 108.f
    ));
    ventana.draw(ayuda);

    float filaX = px + 70.f;
    float filaY = py + 170.f;
    float separacion = 70.f;

    dibujarFilaConfiguracion(
        "Volumen general",
        "",
        filaX,
        filaY + 0 * separacion,
        opcionConfiguracionSeleccionada == 0
    );
    dibujarBarraVolumen(
        volumenGeneral,
        filaX + 385.f,
        filaY + 0 * separacion + 18.f,
        opcionConfiguracionSeleccionada == 0
    );

    dibujarFilaConfiguracion(
        "Volumen musica",
        "",
        filaX,
        filaY + 1 * separacion,
        opcionConfiguracionSeleccionada == 1
    );
    dibujarBarraVolumen(
        volumenMusica,
        filaX + 385.f,
        filaY + 1 * separacion + 18.f,
        opcionConfiguracionSeleccionada == 1
    );

    dibujarFilaConfiguracion(
        "Volumen efectos",
        "",
        filaX,
        filaY + 2 * separacion,
        opcionConfiguracionSeleccionada == 2
    );
    dibujarBarraVolumen(
        volumenEfectos,
        filaX + 385.f,
        filaY + 2 * separacion + 18.f,
        opcionConfiguracionSeleccionada == 2
    );

    dibujarFilaConfiguracion(
        "Silenciar todo",
        sonidoSilenciado ? "SI" : "NO",
        filaX,
        filaY + 3 * separacion,
        opcionConfiguracionSeleccionada == 3
    );

    dibujarFilaConfiguracion(
        "Pantalla completa",
        pantallaCompletaActivada ? "SI" : "NO",
        filaX,
        filaY + 4 * separacion,
        opcionConfiguracionSeleccionada == 4
    );

    dibujarFilaConfiguracion(
        "Restablecer valores",
        "ENTER",
        filaX,
        filaY + 5 * separacion,
        opcionConfiguracionSeleccionada == 5
    );

    dibujarFilaConfiguracion(
        "Volver al menu",
        "ESC",
        filaX,
        filaY + 6 * separacion,
        opcionConfiguracionSeleccionada == 6
    );

    sf::Text txtEsc(fuente, "[ ESC ]  Volver al menu principal", 18);
    txtEsc.setFillColor(sf::Color(150, 150, 190));
    txtEsc.setOutlineColor(sf::Color(5, 5, 20));
    txtEsc.setOutlineThickness(0.8f);

    sf::FloatRect eb = txtEsc.getLocalBounds();
    txtEsc.setPosition(sf::Vector2f(
        VENTANA_ANCHO / 2.f - eb.size.x / 2.f,
        py + ph - 38.f
    ));
    ventana.draw(txtEsc);
}

// CONFIRMAR SALIDA

EstadoJuego Menu::procesarEventoConfirmarSalida(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape) {
            return EstadoJuego::MENU;
        }

        if (key->code == sf::Keyboard::Key::Left ||
            key->code == sf::Keyboard::Key::A ||
            key->code == sf::Keyboard::Key::Right ||
            key->code == sf::Keyboard::Key::D) {
            opcionSalidaSeleccionada = 1 - opcionSalidaSeleccionada;
        }

        if (key->code == sf::Keyboard::Key::Enter ||
            key->code == sf::Keyboard::Key::Space) {
            if (opcionSalidaSeleccionada == 0) {
                return EstadoJuego::SALIR;
            }
            else {
                return EstadoJuego::MENU;
            }
        }
    }

    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos = ventana.mapPixelToCoords(
                sf::Vector2i(click->position.x, click->position.y),
                ventana.getView()
            );

            sf::FloatRect botonSi(
                sf::Vector2f(VENTANA_ANCHO / 2.f - 250.f, 610.f),
                sf::Vector2f(200.f, 60.f)
            );

            sf::FloatRect botonNo(
                sf::Vector2f(VENTANA_ANCHO / 2.f + 50.f, 610.f),
                sf::Vector2f(200.f, 60.f)
            );

            if (botonSi.contains(pos)) {
                opcionSalidaSeleccionada = 0;
                return EstadoJuego::SALIR;
            }

            if (botonNo.contains(pos)) {
                opcionSalidaSeleccionada = 1;
                return EstadoJuego::MENU;
            }
        }
    }

    return EstadoJuego::CONFIRMAR_SALIDA;
}

void Menu::dibujarConfirmarSalida() {
    if (fondoCargado) {
        ventana.draw(spriteFondo);
    }
    else {
        ventana.clear(sf::Color(15, 15, 25));
    }

    sf::RectangleShape overlay(sf::Vector2f(VENTANA_ANCHO, VENTANA_ALTO));
    overlay.setFillColor(sf::Color(0, 0, 0, 195));
    ventana.draw(overlay);

    float pw = 760.f;
    float ph = 610.f;
    float px = (VENTANA_ANCHO - pw) / 2.f;
    float py = (VENTANA_ALTO - ph) / 2.f;

    sf::RectangleShape sombra(sf::Vector2f(pw + 14.f, ph + 14.f));
    sombra.setPosition(sf::Vector2f(px - 7.f, py + 7.f));
    sombra.setFillColor(sf::Color(0, 0, 0, 170));
    ventana.draw(sombra);

    sf::RectangleShape panel(sf::Vector2f(pw, ph));
    panel.setPosition(sf::Vector2f(px, py));
    panel.setFillColor(sf::Color(18, 18, 34, 245));
    panel.setOutlineColor(sf::Color(125, 125, 220));
    panel.setOutlineThickness(3.f);
    ventana.draw(panel);

    sf::Text titulo(fuente, "QUIERES ABANDONAR EL JARDIN?", 38);
    titulo.setFillColor(sf::Color(200, 215, 255));
    titulo.setOutlineColor(sf::Color(10, 10, 25));
    titulo.setOutlineThickness(1.5f);
    titulo.setStyle(sf::Text::Bold);

    sf::FloatRect bt = titulo.getLocalBounds();

    titulo.setPosition(sf::Vector2f(
        VENTANA_ANCHO / 2.f - bt.size.x / 2.f,
        py + 40.f
    ));

    ventana.draw(titulo);

    sf::RectangleShape sep(sf::Vector2f(pw - 90.f, 2.f));
    sep.setPosition(sf::Vector2f(px + 45.f, py + 105.f));
    sep.setFillColor(sf::Color(120, 120, 210, 200));
    ventana.draw(sep);

    sf::Text mensaje(fuente, "Los zombies seguiran avanzando si te vas...", 26);
    mensaje.setFillColor(sf::Color(240, 240, 245));
    mensaje.setOutlineColor(sf::Color(5, 5, 20));
    mensaje.setOutlineThickness(1.f);

    sf::FloatRect bm = mensaje.getLocalBounds();

    mensaje.setPosition(sf::Vector2f(
        VENTANA_ANCHO / 2.f - bm.size.x / 2.f,
        py + 145.f
    ));

    ventana.draw(mensaje);

    if (plantaTristeCargada) {
        ventana.draw(spritePlantaTriste);
    }
    else {
        sf::CircleShape cara(70.f);
        cara.setFillColor(sf::Color(100, 200, 90));
        cara.setOutlineColor(sf::Color(20, 80, 20));
        cara.setOutlineThickness(4.f);
        cara.setPosition(sf::Vector2f(
            VENTANA_ANCHO / 2.f - 70.f,
            py + 245.f
        ));
        ventana.draw(cara);
    }

    sf::Text ayuda(fuente, "Flechas para elegir  |  ENTER para confirmar  |  ESC para volver", 18);
    ayuda.setFillColor(sf::Color(165, 165, 200));
    ayuda.setOutlineColor(sf::Color(5, 5, 20));
    ayuda.setOutlineThickness(0.8f);

    sf::FloatRect bh = ayuda.getLocalBounds();

    ayuda.setPosition(sf::Vector2f(
        VENTANA_ANCHO / 2.f - bh.size.x / 2.f,
        535.f
    ));

    ventana.draw(ayuda);

    auto dibujarBotonSalida = [&](const std::string& textoBoton, float x, float y, bool seleccionado) {
        sf::RectangleShape boton(sf::Vector2f(200.f, 60.f));
        boton.setPosition(sf::Vector2f(x, y));

        if (seleccionado) {
            boton.setFillColor(sf::Color(80, 90, 150, 210));
            boton.setOutlineColor(sf::Color(200, 255, 50, 240));
            boton.setOutlineThickness(4.f);
        }
        else {
            boton.setFillColor(sf::Color(25, 25, 45, 190));
            boton.setOutlineColor(sf::Color(100, 100, 160, 160));
            boton.setOutlineThickness(2.f);
        }

        ventana.draw(boton);

        sf::Text txt(fuente, textoBoton, 28);
        txt.setFillColor(seleccionado ? sf::Color(200, 255, 50) : sf::Color(230, 230, 240));
        txt.setOutlineColor(sf::Color(5, 5, 20));
        txt.setOutlineThickness(1.f);
        txt.setStyle(sf::Text::Bold);

        sf::FloatRect b = txt.getLocalBounds();

        txt.setPosition(sf::Vector2f(
            x + 100.f - b.size.x / 2.f,
            y + 30.f - b.size.y / 2.f - 5.f
        ));

        ventana.draw(txt);
        };

    dibujarBotonSalida(
        "SI, SALIR",
        VENTANA_ANCHO / 2.f - 250.f,
        610.f,
        opcionSalidaSeleccionada == 0
    );

    dibujarBotonSalida(
        "NO, SEGUIR",
        VENTANA_ANCHO / 2.f + 50.f,
        610.f,
        opcionSalidaSeleccionada == 1
    );
}

// ---------------------------------------------------------------------------
// RANKING - Historial de partidas
// ---------------------------------------------------------------------------

EstadoJuego Menu::procesarEventoRanking(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape ||
            key->code == sf::Keyboard::Key::Enter  ||
            key->code == sf::Keyboard::Key::Space) {
            return EstadoJuego::MENU;
        }
    }
    if (event.is<sf::Event::MouseButtonPressed>()) {
        return EstadoJuego::MENU;
    }
    return EstadoJuego::RANKING;
}

void Menu::dibujarRanking() {
    // Fondo oscuro igual que el resto de pantallas secundarias
    if (fondoCargado) {
        ventana.draw(spriteFondo);
    }
    sf::RectangleShape overlay(sf::Vector2f(VENTANA_ANCHO, VENTANA_ALTO));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));
    ventana.draw(overlay);

    // Panel central
    float panelX = 130.f;
    float panelY = 90.f;
    float panelW = 940.f;
    float panelH = 660.f;

    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(sf::Vector2f(panelX, panelY));
    panel.setFillColor(sf::Color(15, 20, 35, 230));
    panel.setOutlineColor(sf::Color(255, 200, 50, 180));
    panel.setOutlineThickness(2.f);
    ventana.draw(panel);

    // Titulo
    sf::Text titulo(fuente, "HISTORIAL DE PARTIDAS", 28);
    titulo.setFillColor(sf::Color(255, 200, 50));
    titulo.setStyle(sf::Text::Bold);
    sf::FloatRect bTitulo = titulo.getLocalBounds();
    titulo.setPosition(sf::Vector2f(
        panelX + panelW / 2.f - bTitulo.size.x / 2.f,
        panelY + 18.f
    ));
    ventana.draw(titulo);

    // Separador bajo el titulo
    sf::RectangleShape sep(sf::Vector2f(panelW - 40.f, 1.f));
    sep.setPosition(sf::Vector2f(panelX + 20.f, panelY + 62.f));
    sep.setFillColor(sf::Color(255, 200, 50, 100));
    ventana.draw(sep);

    // Cabeceras de columna
    float colX[6] = { panelX + 22.f, panelX + 60.f, panelX + 230.f,
                      panelX + 460.f, panelX + 560.f, panelX + 710.f };
    const char* cabeceras[6] = { "#", "GANADOR", "VICTORIA", "TURNOS", "BAJAS P/Z", "FECHA" };
    float yHeader = panelY + 74.f;

    for (int i = 0; i < 6; i++) {
        sf::Text h(fuente, cabeceras[i], 15);
        h.setFillColor(sf::Color(180, 180, 180));
        h.setStyle(sf::Text::Bold);
        h.setPosition(sf::Vector2f(colX[i], yHeader));
        ventana.draw(h);
    }

    // Separador bajo cabeceras
    sf::RectangleShape sep2(sf::Vector2f(panelW - 40.f, 1.f));
    sep2.setPosition(sf::Vector2f(panelX + 20.f, panelY + 97.f));
    sep2.setFillColor(sf::Color(100, 100, 100, 150));
    ventana.draw(sep2);

    // Filas de datos
    if (entradasRanking.empty()) {
        sf::Text vacio(fuente, "No hay partidas guardadas todavia.", 20);
        vacio.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect bv = vacio.getLocalBounds();
        vacio.setPosition(sf::Vector2f(
            panelX + panelW / 2.f - bv.size.x / 2.f,
            panelY + panelH / 2.f - bv.size.y / 2.f
        ));
        ventana.draw(vacio);
    }
    else {
        float yFila = panelY + 108.f;
        float altoFila = 48.f;

        for (int i = 0; i < (int)entradasRanking.size(); i++) {
            const EntradaRanking& e = entradasRanking[i];

            // Fondo alternado de filas
            if (i % 2 == 0) {
                sf::RectangleShape fondoFila(sf::Vector2f(panelW - 40.f, altoFila - 4.f));
                fondoFila.setPosition(sf::Vector2f(panelX + 20.f, yFila));
                fondoFila.setFillColor(sf::Color(255, 255, 255, 12));
                ventana.draw(fondoFila);
            }

            // Color segun ganador
            bool plantasGanan = (e.ganador == "PLANTAS");
            sf::Color colorGanador = plantasGanan
                ? sf::Color(100, 230, 100)   // verde para plantas
                : sf::Color(180, 100, 255);  // morado para zombies

            // Numero de fila
            sf::Text tNum(fuente, std::to_string(i + 1), 16);
            tNum.setFillColor(sf::Color(180, 180, 180));
            tNum.setPosition(sf::Vector2f(colX[0], yFila + 8.f));
            ventana.draw(tNum);

            // Ganador
            sf::Text tGan(fuente, e.ganador, 17);
            tGan.setFillColor(colorGanador);
            tGan.setStyle(sf::Text::Bold);
            tGan.setPosition(sf::Vector2f(colX[1], yFila + 8.f));
            ventana.draw(tGan);

            // Tipo de victoria
            sf::Text tVic(fuente, e.tipoVictoria, 15);
            tVic.setFillColor(sf::Color(220, 220, 220));
            tVic.setPosition(sf::Vector2f(colX[2], yFila + 8.f));
            ventana.draw(tVic);

            // Turnos
            sf::Text tTur(fuente, std::to_string(e.turnosJugados), 16);
            tTur.setFillColor(sf::Color(220, 220, 220));
            tTur.setPosition(sf::Vector2f(colX[3], yFila + 8.f));
            ventana.draw(tTur);

            // Bajas P / Z
            std::string bajas = std::to_string(e.bajasLuz) + " / " + std::to_string(e.bajasOscuridad);
            sf::Text tBaj(fuente, bajas, 15);
            tBaj.setFillColor(sf::Color(220, 220, 220));
            tBaj.setPosition(sf::Vector2f(colX[4], yFila + 8.f));
            ventana.draw(tBaj);

            // Fecha
            sf::Text tFec(fuente, e.fecha, 14);
            tFec.setFillColor(sf::Color(160, 160, 160));
            tFec.setPosition(sf::Vector2f(colX[5], yFila + 8.f));
            ventana.draw(tFec);

            yFila += altoFila;
        }
    }

    // Pie de pagina
    sf::Text pie(fuente, "ESC / Enter / Clic -> Volver al menu", 15);
    pie.setFillColor(sf::Color(120, 120, 120));
    sf::FloatRect bPie = pie.getLocalBounds();
    pie.setPosition(sf::Vector2f(
        panelX + panelW / 2.f - bPie.size.x / 2.f,
        panelY + panelH - 30.f
    ));
    ventana.draw(pie);
}
