#include "Menu.h"
#include <algorithm>
#include "IAJugador.h"


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
        spriteFondo.setPosition(sf::Vector2f(0.f, 0.f));
    }

    colorTexto = sf::Color(30, 30, 30); // Gris muy oscuro
    colorTextoDeshabilitado = sf::Color(90, 90, 90, 200);


    float anchoBoton = 240.f;
    float altoBoton = 60.f;

    Boton b1;
    b1.forma.setSize(sf::Vector2f(anchoBoton, altoBoton));
    b1.forma.setPosition(sf::Vector2f(400.f, 120.f));
    b1.forma.setRotation(sf::degrees(2.0f));
    b1.texto = "1 VS 1 LOCAL";
    b1.habilitado = true;
    botones.push_back(b1);

    Boton b2;
    b2.forma.setSize(sf::Vector2f(anchoBoton, altoBoton));
    b2.forma.setPosition(sf::Vector2f(400.f, 220.f));
    b2.forma.setRotation(sf::degrees(2.0f));
    b2.texto = "1 VS 1 VS IA";
    b2.habilitado = true;
    botones.push_back(b2);

    float anchoBoton2 = 240.f;
    float altoBoton2 = 60.f;

    Boton d1;
    d1.forma.setSize(sf::Vector2f(anchoBoton2, altoBoton2));
    d1.forma.setPosition(sf::Vector2f(400.f, 120.f));
    d1.forma.setRotation(sf::degrees(2.0f));
    d1.texto = "FACIL";
    d1.habilitado = true;
    botonesDificultad.push_back(d1);

    Boton d2;
    d2.forma.setSize(sf::Vector2f(anchoBoton2, altoBoton2));
    d2.forma.setPosition(sf::Vector2f(400.f, 220.f));
    d2.forma.setRotation(sf::degrees(2.0f));
    d2.texto = "MEDIO";
    d2.habilitado = true;
    botonesDificultad.push_back(d2);

    Boton d3;
    d3.forma.setSize(sf::Vector2f(anchoBoton2, altoBoton2));
    d3.forma.setPosition(sf::Vector2f(400.f, 320.f));
    d3.forma.setRotation(sf::degrees(2.0f));
    d3.texto = "DIFICIL";
    d3.habilitado = true;
    botonesDificultad.push_back(d3);

    dificultadSeleccionada = MEDIO;
}

EstadoJuego Menu::confirmarBoton(int indice) {
    if (indice == 0) return EstadoJuego::CARGANDO;
    if (indice == 1) return EstadoJuego::SELECCION_DIFICULTAD;
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
   
}

void Menu::dibujarBoton(const Boton& b, bool seleccionado) {
    sf::RectangleShape forma = b.forma;

    // Boton totalmente transparente, solo sirve para detectar clics
    forma.setFillColor(sf::Color::Transparent);
    forma.setOutlineThickness(0.f);
    ventana.draw(forma);

    // Tamanho de letra ajustado a 40 para que encaje bien en la lapida achatada
    sf::Text texto(fuente, b.texto, 40);

    if (!b.habilitado) {
        texto.setFillColor(colorTextoDeshabilitado);
    }
    else if (seleccionado) {
        texto.setFillColor(sf::Color(200, 255, 50)); // Amarillo brillante al seleccionar
    }
    else {
        texto.setFillColor(colorTexto); // Oscuro por defecto
    }

    // Giramos el texto los mismos grados que la caja
    texto.setRotation(forma.getRotation());

    sf::FloatRect textBounds = texto.getLocalBounds();
    texto.setOrigin(sf::Vector2f(textBounds.position.x + textBounds.size.x / 2.f,
        textBounds.position.y + textBounds.size.y / 2.f));

    sf::FloatRect formaBounds = forma.getLocalBounds();
    sf::Vector2f centerLocal(formaBounds.size.x / 2.f, formaBounds.size.y / 2.f);
    sf::Vector2f centerGlobal = forma.getTransform().transformPoint(centerLocal);

    texto.setPosition(centerGlobal);
    ventana.draw(texto);
}

EstadoJuego Menu::procesarEventoDificultad(const sf::Event& event) {
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape)
            return EstadoJuego::MENU;

        if (key->code == sf::Keyboard::Key::Up && botonSeleccionado > 0)
            botonSeleccionado--;
        if (key->code == sf::Keyboard::Key::Down && botonSeleccionado < 2)
            botonSeleccionado++;
        if (key->code == sf::Keyboard::Key::Enter || key->code == sf::Keyboard::Key::Space) {
            switch (botonSeleccionado) {
            case 0: dificultadSeleccionada = FACIL; break;
            case 1: dificultadSeleccionada = MEDIO; break;
            case 2: dificultadSeleccionada = DIFICIL; break;
            }
            return EstadoJuego::JUGANDO_IA;
        }
    }
    if (const auto* key = event.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Escape)
            return EstadoJuego::MENU;
    }
    return EstadoJuego::SELECCION_DIFICULTAD;
}

void Menu::dibujarDificultad() {
    if (fondoCargado) ventana.draw(spriteFondo);
    else ventana.clear(sf::Color(15, 15, 25));

    sf::Vector2i mousePx = sf::Mouse::getPosition(ventana);
    sf::Vector2f mouse((float)mousePx.x, (float)mousePx.y);

    for (int i = 0; i < (int)botonesDificultad.size(); i++) {
        bool hover = botonesDificultad[i].forma.getGlobalBounds().contains(mouse);
        dibujarBoton(botonesDificultad[i], hover || i == botonSeleccionado);
    }
}