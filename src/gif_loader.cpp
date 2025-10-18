#include "image.h" 
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm> // Para std::min

namespace CustomImageLoader {

// Estrutura para o Cabeçalho do Descritor de Tela Lógica GIF (Logical Screen Descriptor)
// (Usamos uma struct para facilitar a organização, mas a leitura é feita byte a byte)
struct LogicalScreenDescriptor {
    unsigned short width;
    unsigned short height;
    unsigned char packed_fields; // Contém informações sobre cores, resolução e tabela global
    unsigned char background_color_index;
    unsigned char pixel_aspect_ratio;
};

// ====================================================================
// FUNÇÃO PRINCIPAL PARA CARREGAR O GIF
// ====================================================================

ImageData load_gif(const std::string& filename) {
    ImageData img;
    // Abre o arquivo em modo binário
    std::ifstream file(filename, std::ios::binary | std::ios::in);

    if (!file.is_open()) {
        std::cerr << "ERRO GIF: Nao foi possivel abrir o arquivo: " << filename << std::endl;
        return img;
    }

    // 1. Verificar a Assinatura (Header)
    unsigned char file_signature[GIF_SIGNATURE_SIZE];
    if (!file.read((char*)file_signature, GIF_SIGNATURE_SIZE) ||
        (std::memcmp(file_signature, GIF_SIGNATURE_87A, GIF_SIGNATURE_SIZE) != 0 &&
         std::memcmp(file_signature, GIF_SIGNATURE_89A, GIF_SIGNATURE_SIZE) != 0))
    {
        std::cerr << "ERRO GIF: O arquivo " << filename << " nao e um GIF valido (assinatura incorreta)." << std::endl;
        file.close();
        return img;
    }

    std::cout << "SUCESSO GIF: Assinatura (" << std::string((char*)file_signature, GIF_SIGNATURE_SIZE) << ") verificada." << std::endl;


    // 2. Ler o Descritor de Tela Lógica (Logical Screen Descriptor - 7 bytes)
    LogicalScreenDescriptor lsd;
    
    // O formato GIF é little-endian. Lemos os bytes sequencialmente.
    // Largura (2 bytes)
    unsigned char width_bytes[2];
    file.read((char*)width_bytes, 2);
    lsd.width = width_bytes[0] | (width_bytes[1] << 8);

    // Altura (2 bytes)
    unsigned char height_bytes[2];
    file.read((char*)height_bytes, 2);
    lsd.height = height_bytes[0] | (height_bytes[1] << 8);

    // Campos Empacotados, Cor de Fundo, Relação de Aspecto (3 bytes restantes)
    file.read((char*)&lsd.packed_fields, 1);
    file.read((char*)&lsd.background_color_index, 1);
    file.read((char*)&lsd.pixel_aspect_ratio, 1);

    // Configurar a estrutura ImageData
    img.width = lsd.width;
    img.height = lsd.height;
    img.channels = 3; // GIFs simples (sem transparência) são geralmente convertidos para RGB
    img.is_valid = true;

    // Verificar se há uma Tabela Global de Cores (Global Color Table)
    // O bit mais significativo do packed_fields (bit 7) indica se a GCT existe.
    bool has_global_color_table = (lsd.packed_fields & 0x80);

    if (has_global_color_table) {
        // Os 3 bits de menor significância (bits 0-2) indicam o tamanho da tabela: 2^(N+1)
        int color_table_power = (lsd.packed_fields & 0x07) + 1;
        size_t gct_size = 1 << color_table_power; // 2^(N+1)
        size_t table_bytes = gct_size * 3; // Cada entrada tem 3 bytes (R, G, B)

        std::cout << "INFO GIF: Lendo Tabela Global de Cores com " << gct_size << " entradas (" << table_bytes << " bytes)..." << std::endl;
        
        // *******************************************************************
        // LÓGICA DE LEITURA DA TABELA GLOBAL AQUI
        // *******************************************************************
        // A Tabela Global precisa ser salva para a etapa de descompactação.
        file.seekg(table_bytes, std::ios_base::cur); // Pula a tabela por enquanto
    }

    // ====================================================================
    // A PARTIR DAQUI, VOCÊ IMPLEMENTARÁ A DECODIFICAÇÃO DE DADOS DE IMAGEM
    // 
    // O próximo passo seria ler os Blocos (Extensions, Image Descriptor, etc.)
    // O Bloco de Dados de Imagem usa o algoritmo LZW (Lempel-Ziv-Welch), 
    // que é a parte mais complexa.
    // 
    // Para um GIF estático simples:
    // 1. Ler o Image Descriptor (0x2C).
    // 2. Ler o tamanho inicial do código LZW.
    // 3. Implementar a descompactação LZW para obter os índices de cor.
    // 4. Mapear os índices para cores RGB usando a Tabela de Cores (GCT ou LCT).
    // 5. Preencher img.pixel_data.
    // ====================================================================

    // Exemplo de como alocaríamos memória após a decodificação LZW
    /*
    img.data_size = img.width * img.height * img.channels;
    img.pixel_data = std::make_unique<unsigned char[]>(img.data_size);
    // ... Preencha img.pixel_data com os dados R, G, B dos pixels ...
    */
    
    std::cout << "SUCESSO GIF: Largura=" << img.width << ", Altura=" << img.height << ". Decodificacao LZW pendente." << std::endl;
    
    file.close();
    return img;
}

} // namespace CustomImageLoader


// =========================================================================
// Exemplo de Uso (Função main)
// Para testar, você pode usar a função main do jpg_loader.cpp, 
// ou criar um main simples aqui.
// =========================================================================

/*
int main() {
    // Crie um arquivo de teste com a assinatura correta (GIF87a)
    const std::string test_file = "TESTE_GIF_INVALIDO.gif"; 

    std::ofstream test_out(test_file, std::ios::binary);
    if (test_out.is_open()) {
        // GIF87a
        test_out.write((const char*)CustomImageLoader::GIF_SIGNATURE_87A, CustomImageLoader::GIF_SIGNATURE_SIZE);
        // LSD (Largura=10, Altura=10, Packed=0x07 -> GCT de 256 cores, BG=0, Aspect=0)
        unsigned char lsd_data[] = {0x0A, 0x00, 0x0A, 0x00, 0x87, 0x00, 0x00};
        test_out.write((const char*)lsd_data, sizeof(lsd_data));
        // EOI (Trailer GIF)
        test_out.write("\x3B", 1); 
        test_out.close();
        std::cout << "Arquivo de teste '" << test_file << "' criado." << std::endl;
    }

    std::cout << "\n--- Iniciando teste do carregador GIF ---" << std::endl;
    CustomImageLoader::ImageData image = CustomImageLoader::load_gif(test_file);

    if (image.is_valid) {
        std::cout << "\nGIF carregado (Estrutura e Cabecalho OK). LZW pendente." << std::endl;
        std::cout << "Dimensoes detectadas: " << image.width << "x" << image.height << std::endl;
    } else {
        std::cout << "\nFalha na validacao do cabecalho GIF." << std::endl;
    }
    
    // std::remove(test_file.c_str());

    return 0;
}
*/
