#include "Menu.h"

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

        // Escalamos la imagen pa q cubra toda la ventana (800x800)
        sf::Vector2u texSize = texFondo.getSize();
        float escalaX = 800.f / texSize.x;
        float escalaY = 800.f / texSize.y;
        spriteFondo.setScale(sf::Vector2f(escalaX, escalaY));
    }

    colorBotonActivo = sf::Color(30, 80, 40, 210);
    colorBotonDeshabilitado = sf::Color(30, 30, 35, 180);
    colorBotonHover = sf::Color(50, 130, 70, 230);
    colorTexto = sf::Color::White;
    colorTextoDeshabilitado = sf::Color(150, 150, 160);

    float anchoBoton = 320.f;
    float altoBoton = 65.f;
    float centroX = 400.f - anchoBoton / 2.f;

    Boton b1;
    b1.forma.setSize(sf::Vector2f(anchoBoton, altoBoton));
    b1.forma.setPosition(sf::Vector2f(centroX, 480.f));
    b1.texto = "1 vs 1  Local";
    b1.habilitado = true;
    botones.push_back(b1);

    Boton b2;
    b2.forma.setSize(sf::Vector2f(anchoBoton, altoBoton));
    b2.forma.setPosition(sf::Vector2f(centroX, 565.f));
    b2.texto = "1 vs 1  vs IA  (Proximamente)";
    b2.habilitado = false;
    botones.push_back(b2);
}

EstadoJuego Menu::confirmarBoton(int indice) {
    if (indice == 0) return EstadoJuego::CARGANDO;
    if (indice == 1) return EstadoJuego::JUGANDO_IA;
    return EstadoJuego::MENU;
}

EstadoJuego Menu::procesarEvento(const sf::Event& event) {
    if (const auto* click = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (click->button == sf::Mouse::Button::Left) {
            sf::Vector2f pos((float)click->position.x, (float)click->position.y);
            for (int i = 0; i < (int)botones.size(); i++) {
                if (botones[i].habilitado &&
                    botones[i].forma.getGlobalBounds().contains(pos)) {
                    return confirmarBoton(i);
                }
            }
        }
    }

    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Up && botonSeleccionado > 0)
            botonSeleccionado--;
        if (key->code == sf::Keyboard::Key::Down &&
            botonSeleccionado < (int)botones.size() - 1)
            botonSeleccionado++;
        if (key->code == sf::Keyboard::Key::Enter ||
            key->code == sf::Keyboard::Key::Space) {
            if (botones[botonSeleccionado].habilitado)
                return confirmarBoton(botonSeleccionado);
        }
    }

    return EstadoJuego::MENU;
}

void Menu::dibujar() {
    // Fondo
    if (fondoCargado) {
        ventana.draw(spriteFondo);
        // Overlay oscuro semitransparente pa q los botones se lean bien
        sf::RectangleShape overlay(sf::Vector2f(800.f, 800.f));
        overlay.setFillColor(sf::Color(0, 0, 0, 100));
        ventana.draw(overlay);
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
    // Titulo con sombra pa q se lea sobre el fondo
    sf::Text sombra(fuente, "ARCHON PvZ", 60);
    sombra.setFillColor(sf::Color(0, 0, 0, 200));
    sombra.setStyle(sf::Text::Bold);
    sf::FloatRect bs = sombra.getLocalBounds();
    sombra.setPosition(sf::Vector2f(402.f - bs.size.x / 2.f, 382.f));
    ventana.draw(sombra);

    sf::Text titulo(fuente, "ARCHON PvZ", 60);
    titulo.setFillColor(sf::Color(255, 220, 80));
    titulo.setStyle(sf::Text::Bold);
    sf::FloatRect b = titulo.getLocalBounds();
    titulo.setPosition(sf::Vector2f(400.f - b.size.x / 2.f, 380.f));
    ventana.draw(titulo);

    // Instrucciones
    sf::Text instr(fuente, "Click para seleccionar  |  Flechas + Enter para navegar", 13);
    instr.setFillColor(sf::Color(220, 220, 220, 200));
    sf::FloatRect bi = instr.getLocalBounds();
    instr.setPosition(sf::Vector2f(400.f - bi.size.x / 2.f, 670.f));
    ventana.draw(instr);
}

void Menu::dibujarBoton(const Boton& b, bool seleccionado) {
    sf::RectangleShape forma = b.forma;

    sf::Color colorBase = b.habilitado
        ? (seleccionado ? colorBotonHover : colorBotonActivo)
        : colorBotonDeshabilitado;

    forma.setFillColor(colorBase);
    forma.setOutlineThickness(2.f);
    forma.setOutlineColor(b.habilitado && seleccionado
        ? sf::Color(100, 220, 140)
        : sf::Color(60, 60, 70, 150));
    ventana.draw(forma);

    sf::Text texto(fuente, b.texto, 20);
    texto.setFillColor(b.habilitado ? colorTexto : colorTextoDeshabilitado);
    if (b.habilitado) texto.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = texto.getLocalBounds();
    sf::FloatRect formaB = forma.getGlobalBounds();
    texto.setPosition(sf::Vector2f(
        formaB.position.x + formaB.size.x / 2.f - bounds.size.x / 2.f,
        formaB.position.y + formaB.size.y / 2.f - bounds.size.y / 2.f - 4.f
    ));
    ventana.draw(texto);
}