#include "image.h" 
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <stdexcept>
#include <arpa/inet.h> // Para ntohl (Network to Host Long) - necessário para big-endian

namespace CustomImageLoader {

// Estrutura para um Chunk PNG
struct PngChunk {
    uint32_t length; // 4 bytes - Tamanho dos dados do chunk
    char type[4];    // 4 bytes - Tipo do chunk (ex: "IHDR", "IDAT")
    // data é lido separadamente
    // crc é lido separadamente
};

// Helper function para converter Big Endian (formato PNG) para Endianness do Host
// PNG usa Big-Endian. A maioria dos PCs usa Little-Endian, então a conversão é crítica.
uint32_t read_big_endian_u32(std::ifstream& file) {
    uint32_t value;
    file.read(reinterpret_cast<char*>(&value), 4);
    // Usamos ntohl (network to host long) para garantir a ordem de bytes correta
    // Se o seu sistema for Big-Endian, isso não fará nada (o que é correto).
    // Se for Little-Endian (mais comum), ele inverte os bytes (o que é correto).
    return ntohl(value); 
}

// ====================================================================
// FUNÇÃO PRINCIPAL PARA CARREGAR O PNG
// ====================================================================

ImageData load_png(const std::string& filename) {
    ImageData img;
    // Abre o arquivo em modo binário
    std::ifstream file(filename, std::ios::binary | std::ios::in);

    if (!file.is_open()) {
        std::cerr << "ERRO PNG: Nao foi possivel abrir o arquivo: " << filename << std::endl;
        return img;
    }

    // 1. Verificar a Assinatura (Magic Number de 8 bytes)
    unsigned char file_signature[PNG_SIGNATURE_SIZE];
    if (!file.read(reinterpret_cast<char*>(file_signature), PNG_SIGNATURE_SIZE) ||
        std::memcmp(file_signature, PNG_SIGNATURE, PNG_SIGNATURE_SIZE) != 0)
    {
        std::cerr << "ERRO PNG: O arquivo " << filename << " nao e um PNG valido (assinatura incorreta)." << std::endl;
        file.close();
        return img;
    }

    std::cout << "SUCESSO PNG: Assinatura de 8 bytes verificada." << std::endl;


    // 2. Processar Chunks
    // O PNG é uma série de chunks (Length, Type, Data, CRC)
    
    // O primeiro chunk DEVE ser o IHDR
    PngChunk chunk;
    
    // Loop principal de leitura de chunks
    while (file.peek() != EOF) {
        if (!file.read(reinterpret_cast<char*>(&chunk.length), 4)) break;
        chunk.length = ntohl(chunk.length); // Converter Length de Big-Endian

        if (!file.read(chunk.type, 4)) break;
        chunk.type[4] = '\0'; // Adicionar null-terminator (para impressão, type tem tamanho 4)
        
        std::cout << "INFO PNG: Chunk encontrado: " << chunk.type << " (Tamanho: " << chunk.length << " bytes)" << std::endl;

        // IHDR - Cabeçalho (Obrigatório e sempre o primeiro)
        if (std::strncmp(chunk.type, "IHDR", 4) == 0) {
            if (chunk.length != 13) {
                 std::cerr << "ERRO PNG: Tamanho incorreto para o chunk IHDR." << std::endl;
                 file.close();
                 return img;
            }
            
            // Ler dados do IHDR
            img.width = read_big_endian_u32(file);
            img.height = read_big_endian_u32(file);
            
            unsigned char bit_depth, color_type, compression_method, filter_method, interlace_method;
            file.read(reinterpret_cast<char*>(&bit_depth), 1);
            file.read(reinterpret_cast<char*>(&color_type), 1);
            file.read(reinterpret_cast<char*>(&compression_method), 1);
            file.read(reinterpret_cast<char*>(&filter_method), 1);
            file.read(reinterpret_cast<char*>(&interlace_method), 1);

            // Determinar o número de canais a partir do color_type (simplificado)
            if (color_type == 2) img.channels = 3;      // Truecolor RGB
            else if (color_type == 6) img.channels = 4; // Truecolor com Alpha (RGBA)
            // Outros tipos de cor (0, 3, 4) são mais complexos de lidar do zero.

            img.is_valid = true;
            std::cout << "SUCESSO PNG: Dimensoes lidas: " << img.width << "x" << img.height << std::endl;

            // O próximo é o CRC (4 bytes)
            file.seekg(4, std::ios_base::cur); 
        } 
        // IDAT - Dados de Imagem (Contém os pixels comprimidos com Deflate)
        else if (std::strncmp(chunk.type, "IDAT", 4) == 0) {
            
            // ***************************************************************
            // LÓGICA COMPLEXA: DECOMPRESSÃO DEFLATE E FILTRAGEM DE PIXELS
            // Você precisará de um algoritmo Deflate/zlib aqui!
            // ***************************************************************
            
            // Por enquanto, apenas pulamos os dados e o CRC (4 bytes)
            file.seekg(chunk.length + 4, std::ios_base::cur); 

        } 
        // IEND - Fim do Arquivo (Obrigatório e sempre o último)
        else if (std::strncmp(chunk.type, "IEND", 4) == 0) {
            // Fim do arquivo, saímos do loop
            file.seekg(4, std::ios_base::cur); // Pular CRC
            break;
        }
        // Outros Chunks (PLTE, tRNS, etc.) - Pular para simplificação
        else {
            // Pular o bloco de dados (chunk.length) e o CRC (4 bytes)
            file.seekg(chunk.length + 4, std::ios_base::cur); 
        }
    }
    
    // ====================================================================
    // APÓS O PROCESSAMENTO: Alocar e preencher os pixels
    // ====================================================================
    /*
    if (img.is_valid) {
        img.data_size = img.width * img.height * img.channels;
        img.pixel_data = std::make_unique<unsigned char[]>(img.data_size);
        // ... O resultado da descompressão Deflate e filtragem preenche img.pixel_data ...
    }
    */
    
    file.close();
    return img;
}

} // namespace CustomImageLoader

// =========================================================================
// Exemplo de Uso (Função main)
// NOTA: Este main é apenas para demonstração do cabeçalho.
// Para testar, você precisará de um arquivo PNG real ou de um mock mais complexo.
// =========================================================================

/*
int main() {
    // Para testar este carregador, você precisará de um arquivo PNG.
    // O processo de "mockar" um PNG é muito complicado devido aos CRCs e à compressão Deflate.
    const std::string test_file = "imagem_real.png"; 

    std::cout << "\n--- Iniciando teste do carregador PNG ---" << std::endl;
    // Tente carregar o arquivo real (ou um mock, se você criar um)
    CustomImageLoader::ImageData image = CustomImageLoader::load_png(test_file);

    if (image.is_valid) {
        std::cout << "\nPNG carregado (Estrutura e Cabecalho IHDR OK)." << std::endl;
        std::cout << "Dimensoes detectadas: " << image.width << "x" << image.height << std::endl;
        std::cout << "Canais (baseado no IHDR): " << image.channels << std::endl;
        std::cout << "O proximo passo e implementar a descompressao Deflate (zlib) e a filtragem." << std::endl;
    } else {
        std::cout << "\nFalha na validacao do cabecalho PNG (Verifique se '" << test_file << "' existe e e um PNG)." << std::endl;
    }
    
    return 0;
}
*/
