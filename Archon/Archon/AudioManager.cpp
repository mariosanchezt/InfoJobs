#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager()
    : canalActualGolpe(0)
    , sonidoMuerte(nullptr)
    , sonidoDisparo(nullptr)
    , sonidoVictoria(nullptr)
    , sonidoDerrota(nullptr)
    , sonidoMovimiento(nullptr)
    , sonidoSeleccion(nullptr)
{
    volumenMusica = 25.f;
    volumenEfectos = 50.f;
}

void AudioManager::cargar() {
    if (!musicaMenu.openFromFile("assets/musica_menu.wav")) {
        std::cout << "Error: no se pudo cargar musica_menu.wav" << std::endl;
    }
    musicaMenu.setLooping(true);
    musicaMenu.setVolume(volumenMusica);

    if (!musicaJuego.openFromFile("assets/musica_juego.wav")) {
        std::cout << "Error: no se pudo cargar musica_juego.wav" << std::endl;
    }
    musicaJuego.setLooping(true);
    musicaJuego.setVolume(volumenMusica);

    if (!musicaArena.openFromFile("assets/musica_arena.wav")) {
        std::cout << "Error: no se pudo cargar musica_arena.wav" << std::endl;
    }
    musicaArena.setLooping(true);
    musicaArena.setVolume(volumenMusica);

    if (!musicaPausa.openFromFile("assets/sonido_pausa.wav")) {
        std::cout << "Error: no se pudo cargar sonido_pausa.wav" << std::endl;
    }
    musicaPausa.setLooping(true);
    musicaPausa.setVolume(volumenMusica);

    if (!bufferGolpe.loadFromFile("assets/sonido_golpe.wav")) {
        std::cout << "Error: no se pudo cargar sonido_golpe.wav" << std::endl;
    }

    sonidosGolpe.clear();
    for (int i = 0; i < CANALES_GOLPE; i++) {
        sonidosGolpe.emplace_back(bufferGolpe);
    }

    if (!bufferMuerte.loadFromFile("assets/sonido_muerte.wav")) {
        std::cout << "Error: no se pudo cargar sonido_muerte.wav" << std::endl;
    }

    if (!bufferDisparo.loadFromFile("assets/sonido_disparo.wav")) {
        std::cout << "Error: no se ha podido cargar sonido_disparo.wav" << std::endl;
    }

    if (!bufferVictoria.loadFromFile("assets/sonido_victoria.wav")) {
        std::cout << "Error: no se pudo cargar sonido_victoria.wav" << std::endl;
    }

    if (!bufferDerrota.loadFromFile("assets/sonido_derrota.wav")) {
        std::cout << "Error: no se pudo cargar sonido_derrota.wav" << std::endl;
    }

    if (!bufferMovimiento.loadFromFile("assets/sonido_movimiento.wav")) {
        std::cout << "Error: no se pudo cargar sonido_movimiento.wav" << std::endl;
    }

    if (!bufferSeleccion.loadFromFile("assets/sonido_seleccion.wav")) {
        std::cout << "Error: no se pudo cargar sonido_seleccion.wav" << std::endl;
    }

    sonidoMuerte = new sf::Sound(bufferMuerte);
    sonidoDisparo = new sf::Sound(bufferDisparo);
    sonidoVictoria = new sf::Sound(bufferVictoria);
    sonidoDerrota = new sf::Sound(bufferDerrota);
    sonidoMovimiento = new sf::Sound(bufferMovimiento);
    sonidoSeleccion = new sf::Sound(bufferSeleccion);

    if (sonidoMovimiento) {
        sonidoMovimiento->setLooping(true);
    }

    setVolumenMusica(volumenMusica);
    setVolumenEfectos(volumenEfectos);
}

void AudioManager::playMenuMusic() {
    musicaJuego.stop();
    musicaArena.stop();
    musicaPausa.stop();

    if (musicaMenu.getStatus() != sf::Music::Status::Playing) {
        musicaMenu.play();
    }
}

void AudioManager::playGameMusic() {
    musicaMenu.stop();
    musicaArena.stop();
    musicaPausa.stop();

    if (musicaJuego.getStatus() != sf::Music::Status::Playing) {
        musicaJuego.play();
    }
}

void AudioManager::playArenaMusic() {
    musicaMenu.stop();
    musicaJuego.stop();
    musicaPausa.stop();

    if (musicaArena.getStatus() != sf::Music::Status::Playing) {
        musicaArena.play();
    }
}

void AudioManager::playPausa() {
    musicaMenu.stop();
    musicaJuego.stop();
    musicaArena.stop();

    if (musicaPausa.getStatus() != sf::Music::Status::Playing) {
        musicaPausa.play();
    }
}

void AudioManager::stopMusic() {
    musicaMenu.stop();
    musicaJuego.stop();
    musicaArena.stop();
    musicaPausa.stop();
}

void AudioManager::playGolpe() {
    if (sonidosGolpe.empty()) return;

    sonidosGolpe[canalActualGolpe].setPlayingOffset(sf::Time::Zero);
    sonidosGolpe[canalActualGolpe].play();

    canalActualGolpe = (canalActualGolpe + 1) % CANALES_GOLPE;
}

void AudioManager::playMuerte() {
    if (sonidoMuerte) {
        sonidoMuerte->setPlayingOffset(sf::Time::Zero);
        sonidoMuerte->play();
    }
}

void AudioManager::playDisparo() {
    if (sonidoDisparo) {
        sonidoDisparo->setPlayingOffset(sf::Time::Zero);
        sonidoDisparo->play();
    }
}

void AudioManager::playVictoria() {
    if (sonidoVictoria) {
        sonidoVictoria->setPlayingOffset(sf::Time::Zero);
        sonidoVictoria->play();
    }
}

void AudioManager::playDerrota() {
    if (sonidoDerrota) {
        sonidoDerrota->setPlayingOffset(sf::Time::Zero);
        sonidoDerrota->play();
    }
}

void AudioManager::playMovimiento() {
    if (sonidoMovimiento &&
        sonidoMovimiento->getStatus() != sf::Sound::Status::Playing) {
        sonidoMovimiento->play();
    }
}

void AudioManager::stopMovimiento() {
    if (sonidoMovimiento) {
        sonidoMovimiento->stop();
    }
}

void AudioManager::playSeleccion() {
    if (sonidoSeleccion) {
        sonidoSeleccion->setPlayingOffset(sf::Time::Zero);
        sonidoSeleccion->play();
    }
}

void AudioManager::setVolumenMusica(float vol) {
    volumenMusica = vol;

    musicaMenu.setVolume(vol);
    musicaJuego.setVolume(vol);
    musicaArena.setVolume(vol);
    musicaPausa.setVolume(vol);
}

void AudioManager::setVolumenEfectos(float vol) {
    volumenEfectos = vol;

    for (int i = 0; i < (int)sonidosGolpe.size(); i++) {
        sonidosGolpe[i].setVolume(vol);
    }

    if (sonidoMuerte) sonidoMuerte->setVolume(vol);
    if (sonidoDisparo) sonidoDisparo->setVolume(vol);
    if (sonidoVictoria) sonidoVictoria->setVolume(vol);
    if (sonidoDerrota) sonidoDerrota->setVolume(vol);
    if (sonidoMovimiento) sonidoMovimiento->setVolume(vol);
    if (sonidoSeleccion) sonidoSeleccion->setVolume(vol);
}

AudioManager::~AudioManager() {
    delete sonidoMuerte;
    delete sonidoDisparo;
    delete sonidoVictoria;
    delete sonidoDerrota;
    delete sonidoMovimiento;
    delete sonidoSeleccion;
}