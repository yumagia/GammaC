#include "Renderer.hpp"

#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <vector>


namespace GammaEngine {
    void Renderer::Initialize(unsigned int width, unsigned int height) {
        sf::Uint8 pixelData[width * height * 4];
        sf::Image image;
        sf::Sprite sprite;


        ResizeViewport(width, height);
    }

    void Renderer::ResizeViewport(unsigned int width, unsigned int height) {
        
    }

    struct Pixel {
        uint8_t r{ 0 }, g{ 0 }, b{ 0 }, a{ 0 };
    };
    void Renderer::Draw(unsigned int width, unsigned int height) {

    }
}