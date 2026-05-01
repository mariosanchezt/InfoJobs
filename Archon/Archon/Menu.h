#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

enum class EstadoJuego { MENU, CARGANDO, JUGANDO_LOCAL, JUGANDO_IA };

struct Boton {
    sf::RectangleShape forma;
    std::string texto;
    bool habilitado;
};

class Menu {
private:
    sf::RenderWindow& ventana;
    sf::Font& fuente;

    std::vector<Boton> botones;
    int botonSeleccionado;

    // Imagen de fondo del menu
    sf::Texture texFondo;
    sf::Sprite  spriteFondo;
    bool        fondoCargado;

    sf::Color colorBotonActivo;
    sf::Color colorBotonDeshabilitado;
    sf::Color colorBotonHover;
    sf::Color colorTexto;
    sf::Color colorTextoDeshabilitado;

    void dibujarTitulo();
    void dibujarBoton(const Boton& b, bool seleccionado);
    EstadoJuego confirmarBoton(int indice);

public:
    Menu(sf::RenderWindow& vent, sf::Font& f);

    EstadoJuego procesarEvento(const sf::Event& event);
    void dibujar();
};