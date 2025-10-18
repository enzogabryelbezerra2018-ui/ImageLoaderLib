#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstddef> // Para size_t

namespace CustomImageLoader {

// Estrutura para os dados da imagem (os pixels brutos)
struct ImageData {
    int width = 0;
    int height = 0;
    int channels = 0; // 3 para RGB, 4 para RGBA, etc.
    // Ponteiro inteligente para os dados dos pixels bratos (ex: R, G, B, R, G, B, ...)
    std::unique_ptr<unsigned char[]> pixel_data = nullptr; 
    size_t data_size = 0;
    bool is_valid = false;
};

// Funções de carregamento
ImageData load_jpeg(const std::string& filename); // Já implementado
ImageData load_gif(const std::string& filename);  // NOVO: Adicione esta linha

// Assinaturas de Arquivo
constexpr unsigned char JPEG_SOI[] = {0xFF, 0xD8};
constexpr size_t JPEG_SOI_SIZE = 2;

constexpr unsigned char GIF_SIGNATURE_87A[] = {'G', 'I', 'F', '8', '7', 'a'};
constexpr unsigned char GIF_SIGNATURE_89A[] = {'G', 'I', 'F', '8', '9', 'a'};
constexpr size_t GIF_SIGNATURE_SIZE = 6;

} // namespace CustomImageLoader
