#pragma once

#include <string>
#include <vector>
#include <memory>
#include <cstddef> 
#include <cstdint> // Para uint32_t

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
    
    // SVG fields (opcionalmente mantenha, ou crie uma classe base e herança)
    std::vector<std::string> vector_elements; 
};

// Funções de carregamento
ImageData load_jpeg(const std::string& filename); 
ImageData load_gif(const std::string& filename);  
ImageData load_png(const std::string& filename);  
ImageData load_svg(const std::string& filename); 
ImageData load_webp(const std::string& filename); // NOVO: Adicione esta linha

// Assinaturas de Arquivo (apenas WebP adicionada por brevidade)
// ...
constexpr unsigned char PNG_SIGNATURE[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
constexpr size_t PNG_SIGNATURE_SIZE = 8;

// Assinaturas WebP (RIFF Chunks)
constexpr uint32_t RIFF_TAG = 0x46464952; // 'RIFF' em Little-Endian
constexpr uint32_t WEBP_TAG = 0x50424557; // 'WEBP' em Little-Endian

// Tags de Chunk WebP
constexpr uint32_t VP8_TAG  = 0x20385056; // 'VP8 ' (com espaço)
constexpr uint32_t VP8L_TAG = 0x4C385056; // 'VP8L'
constexpr uint32_t VP8X_TAG = 0x58385056; // 'VP8X' (para formato estendido)

} // namespace CustomImageLoader
