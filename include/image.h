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

/**
 * @brief Carrega um arquivo JPEG do caminho especificado.
 * * Esta função deve implementar a lógica completa de decodificação JPEG.
 * * @param filename Caminho para o arquivo JPEG.
 * @return ImageData A estrutura contendo os pixels e metadados da imagem.
 */
ImageData load_jpeg(const std::string& filename);

// Assinatura de Início de Imagem JPEG (Start of Image - SOI)
constexpr unsigned char JPEG_SOI[] = {0xFF, 0xD8};
constexpr size_t JPEG_SOI_SIZE = 2;

} // namespace CustomImageLoader
