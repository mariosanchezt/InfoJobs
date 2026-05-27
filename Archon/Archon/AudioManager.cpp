#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() 
: sonidoGolpe(bufferGolpe)
, sonidoMuerte(bufferMuerte)
, sonidoDisparo(bufferDisparo)
{
	volumenMusica = 50.f;
	volumenEfectos = 50.f;
}

void AudioManager::cargar() {
	if (!musicaMenu.openFromFile("assets/musica_menu.wav"))
		std::cout << "Error: no se pudo cargar musica_menu.wav" << std::endl;
	musicaMenu.setLooping(true);
	musicaMenu.setVolume(volumenMusica);

	if (!musicaJuego.openFromFile("assets/musica_juego.wav"))
		std::cout << "Error: no se pudo cargar musica_juego.wav" << std::endl;
	musicaJuego.setLooping(true);
	musicaJuego.setVolume(volumenMusica);

	if (!bufferGolpe.loadFromFile("assets/sonido_golpe.wav"))
		std::cout << "Error: no se pudo cargar sonido_golpe.wav" << std::endl;

	if (!bufferMuerte.loadFromFile("assets/sonido_muerte.wav"))
		std::cout << "Error: no se pudo cargar sonido_muerte.wav" << std::endl;

	if (!bufferDisparo.loadFromFile("assets/sonido_disparo.wav"))
		std::cout << "Error: no se ha podido cargar sonido_disparo.wav" << std::endl;

	sonidoGolpe.setVolume(volumenEfectos);
	sonidoMuerte.setVolume(volumenEfectos);
	sonidoDisparo.setVolume(volumenEfectos);
}

void AudioManager::playMenuMusic() {
	musicaJuego.stop();
	musicaMenu.play();
}

void AudioManager::playGameMusic() {
	musicaMenu.stop();
	musicaJuego.play();
}

void AudioManager::stopMusic() {
	musicaMenu.stop();
	musicaJuego.stop();
}

void AudioManager::playGolpe() {
	sonidoGolpe.play();
}

void AudioManager::playMuerte() {
	sonidoMuerte.play();
}

void AudioManager::playDisparo() {
	sonidoDisparo.play();
}

void AudioManager::setVolumenMusica(float vol) {
	volumenMusica = vol;
	musicaMenu.setVolume(vol);
	musicaJuego.setVolume(vol);
}

void AudioManager::setVolumenEfectos(float vol) {
	volumenEfectos = vol;
	sonidoGolpe.setVolume(vol);
	sonidoMuerte.setVolume(vol);
	sonidoDisparo.setVolume(vol);
}