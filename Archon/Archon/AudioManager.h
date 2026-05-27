#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

class AudioManager {
private:
    sf::Music musicaMenu;
    sf::Music musicaJuego;

    sf::SoundBuffer bufferGolpe;
    sf::SoundBuffer bufferMuerte;
    sf::SoundBuffer bufferDisparo;

    static constexpr int CANALES_GOLPE = 4;
    std::vector<sf::Sound> sonidosGolpe;
    int canalActualGolpe;

    sf::Sound* sonidoMuerte;
    sf::Sound* sonidoDisparo;

    float volumenMusica;
    float volumenEfectos;

public:
    AudioManager();
    ~AudioManager();

    void cargar();

    void playMenuMusic();
    void playGameMusic();
    void stopMusic();

    void playGolpe();
    void playMuerte();
    void playDisparo();

    void setVolumenMusica(float vol);
    void setVolumenEfectos(float vol);
    float getVolumenMusica() const { return volumenMusica; }
    float getVolumenEfectos() const { return volumenEfectos; }
};