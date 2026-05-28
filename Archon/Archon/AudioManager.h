#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <vector>

class AudioManager {
private:
    sf::Music musicaMenu;
    sf::Music musicaJuego;
    sf::Music musicaArena;
    sf::Music musicaPausa;

    sf::SoundBuffer bufferGolpe;
    sf::SoundBuffer bufferMuerte;
    sf::SoundBuffer bufferDisparo;
    sf::SoundBuffer bufferVictoria;
    sf::SoundBuffer bufferDerrota;
    sf::SoundBuffer bufferMovimiento;
    sf::SoundBuffer bufferSeleccion;

    static constexpr int CANALES_GOLPE = 4;
    std::vector<sf::Sound> sonidosGolpe;
    int canalActualGolpe;

    sf::Sound* sonidoMuerte;
    sf::Sound* sonidoDisparo;
    sf::Sound* sonidoVictoria;
    sf::Sound* sonidoDerrota;
    sf::Sound* sonidoMovimiento;
    sf::Sound* sonidoSeleccion;

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
    void playArenaMusic();
    void playVictoria();
    void playDerrota();
    void playPausa();
    void playMovimiento();
    void stopMovimiento();
    void playSeleccion();

    void setVolumenMusica(float vol);
    void setVolumenEfectos(float vol);
    float getVolumenMusica() const { return volumenMusica; }
    float getVolumenEfectos() const { return volumenEfectos; }
};