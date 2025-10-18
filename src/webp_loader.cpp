#include "image.h" 
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <stdexcept>

namespace CustomImageLoader {

// Estrutura para um Chunk RIFF básico
// WebP usa a estrutura Little-Endian, então a leitura é mais direta na maioria dos PCs
struct RiffChunk {
    uint32_t fourcc;    // 4 bytes - 'RIFF', 'WEBP', 'VP8 ', 'VP8L', etc.
    uint32_t size;      // 4 bytes - Tamanho do bloco de dados que segue
};

// Helper function para ler um uint32_t (4 bytes)
// RIFF e, por extensão, WebP, usam Little-Endian, o que é nativo na maioria dos hosts.
uint32_t read_u32(std::ifstream& file) {
    uint32_t value = 0;
    file.read(reinterpret_cast<char*>(&value), 4);
    return value;
}

// ====================================================================
// FUNÇÃO PRINCIPAL PARA CARREGAR O WEBP
// ====================================================================

ImageData load_webp(const std::string& filename) {
    ImageData img;
    
    std::ifstream file(filename, std::ios::binary | std::ios::in);

    if (!file.is_open()) {
        std::cerr << "ERRO WEBP: Nao foi possivel abrir o arquivo: " << filename << std::endl;
        return img;
    }

    // 1. Ler o Cabeçalho RIFF (RIFF - FileSize - WEBP)
    RiffChunk riff_header;
    riff_header.fourcc = read_u32(file);
    riff_header.size = read_u32(file);
    uint32_t file_format = read_u32(file); // Deveria ser 'WEBP'

    if (riff_header.fourcc != RIFF_TAG || file_format != WEBP_TAG) {
        std::cerr << "ERRO WEBP: Assinatura RIFF ou formato WEBP invalida." << std::endl;
        file.close();
        return img;
    }
    
    // O tamanho do arquivo RIFF é o tamanho dos dados restantes (excluindo os 8 bytes do RIFF)
    std::cout << "SUCESSO WEBP: Assinatura RIFF/WEBP verificada. Tamanho do Conteudo: " << riff_header.size << " bytes." << std::endl;

    // 2. Loop de Processamento de Sub-chunks (VP8, VP8L, VP8X, etc.)
    uint32_t current_position = 12; // Já lemos 4+4+4 bytes
    
    while (current_position < riff_header.size + 8) { // +8 porque file_size é o tamanho total - 8
        RiffChunk chunk;
        
        if (!file.read(reinterpret_cast<char*>(&chunk.fourcc), 4) ||
            !file.read(reinterpret_cast<char*>(&chunk.size), 4)) 
        {
            // Fim inesperado do arquivo
            break; 
        }

        current_position += 8;

        // Converção dos 4-chars para string (apenas para impressão)
        char tag_chars[5] = {0};
        std::memcpy(tag_chars, &chunk.fourcc, 4);
        std::cout << "INFO WEBP: Chunk encontrado: " << tag_chars << " (Tamanho: " << chunk.size << " bytes)" << std::endl;

        // ----------------------------------------------------------------------
        // A. Chunk VP8 (Compressão com Perda)
        // ----------------------------------------------------------------------
        if (chunk.fourcc == VP8_TAG) { 
            // Os dados VP8 contêm o cabeçalho VP8, que tem largura e altura
            
            // O cabeçalho VP8 é de 10 bytes: 3 bytes de cabeçalho VP8 + 3 bytes de frame size + 1 byte de flags + 3 bytes de header
            // A largura e altura estão codificadas em 14 bits após os 6 bytes de tamanho e flags
            
            // NOTE: A leitura do cabeçalho VP8 é binária complexa e dependente de bit, 
            // e está fora do escopo de um simples carregador de cabeçalho.
            // Para simplificar, lemos apenas a primeira parte do cabeçalho VP8 para ver se é válido (Start Code de 3 bytes)
            
            unsigned char vp8_start_code[3];
            file.read(reinterpret_cast<char*>(vp8_start_code), 3);
            current_position += 3;

            // Pula o restante dos dados do chunk VP8, pois o decode é muito complexo
            file.seekg(chunk.size - 3, std::ios_base::cur); 
            current_position += (chunk.size - 3);

            // A largura e altura DEVERIAM ser lidas aqui (codificadas em 14 bits no cabeçalho VP8)
            img.width = 640;  // Valor de placeholder após decodificar o VP8/VP8L
            img.height = 480; // Valor de placeholder
            img.channels = 3; 
            img.is_valid = true;
            std::cout << "SUCESSO WEBP: Chunk VP8 encontrado. Largura e altura a serem decodificadas do bitstream VP8." << std::endl;
            break; // A imagem principal foi encontrada.

        }
        // ----------------------------------------------------------------------
        // B. Chunk VP8L (Compressão sem Perda)
        // ----------------------------------------------------------------------
        else if (chunk.fourcc == VP8L_TAG) {
            // VP8L também tem largura e altura no seu cabeçalho de bitstream
            
            // NOTE: O VP8L é ainda mais complexo do que o VP8 para decodificação.
            // Pula o chunk inteiro para simular a leitura do VP8L.
            file.seekg(chunk.size, std::ios_base::cur);
            current_position += chunk.size;
            
            img.width = 640;  // Valor de placeholder
            img.height = 480; // Valor de placeholder
            img.channels = 4; // VP8L geralmente suporta Alpha (transparência)
            img.is_valid = true;
            std::cout << "SUCESSO WEBP: Chunk VP8L encontrado. Decodificacao VP8L pendente." << std::endl;
            break; 
        }
        // ----------------------------------------------------------------------
        // C. Outros Chunks (VP8X, ALPH, ICCP, etc.)
        // ----------------------------------------------------------------------
        else {
            // Pula o bloco de dados
            file.seekg(chunk.size, std::ios_base::cur);
            current_position += chunk.size;
        }

        // Chunks RIFF sempre são alinhados em 2 bytes. Se o tamanho for ímpar, há um byte de padding.
        if (chunk.size % 2 != 0) {
            file.seekg(1, std::ios_base::cur);
            current_position += 1;
        }
    }
    
    file.close();

    if (img.is_valid) {
        std::cout << "Carregamento WebP iniciado. Decodificacao VP8/VP8L e o proximo passo!" << std::endl;
    }
    
    return img;
}

} // namespace CustomImageLoader


// =========================================================================
// Exemplo de Uso (Função main)
// =========================================================================

/*
int main() {
    // Para testar, voce precisara de um arquivo WebP real, pois criar um mock e muito dificil.
    const std::string test_file = "imagem_real.webp"; 

    std::cout << "\n--- Iniciando teste do carregador WebP ---" << std::endl;
    // Tente carregar o arquivo real
    CustomImageLoader::ImageData image = CustomImageLoader::load_webp(test_file);

    if (image.is_valid) {
        std::cout << "\nWebP carregado (Estrutura RIFF OK)." << std::endl;
        std::cout << "Dimensoes estimadas: " << image.width << "x" << image.height << std::endl;
        std::cout << "O proximo passo e a complexa implementacao do codec VP8/VP8L." << std::endl;
    } else {
        std::cout << "\nFalha na validacao do cabecalho WebP (Verifique se '" << test_file << "' existe e e um WebP valido)." << std::endl;
    }
    
    return 0;
}
*/
