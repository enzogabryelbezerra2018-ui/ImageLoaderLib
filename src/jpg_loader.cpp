#include "image.h" // Inclui a interface que você definiu
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdio> // Para std::remove

namespace CustomImageLoader {

ImageData load_jpeg(const std::string& filename) {
    ImageData img;
    // Abre o arquivo em modo binário
    std::ifstream file(filename, std::ios::binary | std::ios::in);

    if (!file.is_open()) {
        std::cerr << "ERRO: Nao foi possivel abrir o arquivo: " << filename << std::endl;
        return img; // Retorna estrutura inválida (is_valid = false)
    }

    // 1. Verificar a Assinatura (Start of Image - SOI)
    unsigned char file_signature[JPEG_SOI_SIZE];
    
    if (!file.read((char*)file_signature, JPEG_SOI_SIZE) || 
        std::memcmp(file_signature, JPEG_SOI, JPEG_SOI_SIZE) != 0) 
    {
        std::cerr << "ERRO: O arquivo " << filename << " nao e um JPEG valido (assinatura SOI faltando ou incorreta)." << std::endl;
        file.close();
        return img;
    }

    // ====================================================================
    // A PARTIR DAQUI, VOCÊ IMPLEMENTARÁ A LÓGICA DE DECODIFICAÇÃO JPEG
    // Esta é a parte complexa que lê os marcadores (SOF0, DHT, SOS) 
    // e executa a descompactação Huffman e a IDCT.
    // ====================================================================

    std::cout << "SUCESSO: Assinatura JPEG verificada. Lendo cabecalhos..." << std::endl;

    // --- EXEMPLO DE COMO OS DADOS SERIAM PREENCHIDOS APÓS O PARSE ---
    /*
    // APÓS PARSEAR SOF0
    img.width = 640;
    img.height = 480;
    img.channels = 3; 

    // APÓS DECODIFICAR
    img.data_size = img.width * img.height * img.channels;
    img.pixel_data = std::make_unique<unsigned char[]>(img.data_size);
    // ... Preencha img.pixel_data com os dados R, G, B dos pixels ...

    img.is_valid = true;
    */
    // -------------------------------------------------------------------
    
    file.close();
    return img; 
}

} // namespace CustomImageLoader

// =========================================================================
// Exemplo de Uso (Função main)
// =========================================================================
int main() {
    const std::string test_file = "TESTE_JPEG_INVALIDO.jpg"; 

    // Cria um arquivo mínimo com assinatura JPEG para o teste inicial
    std::ofstream test_out(test_file, std::ios::binary);
    if (test_out.is_open()) {
        // Escreve a assinatura de Início (SOI)
        test_out.write((const char*)CustomImageLoader::JPEG_SOI, CustomImageLoader::JPEG_SOI_SIZE);
        // Escreve o marcador de Fim (EOI) para fechar o arquivo
        test_out.write("\xFF\xD9", 2);         
        test_out.close();
        std::cout << "Arquivo de teste '" << test_file << "' criado." << std::endl;
    }


    std::cout << "\n--- Iniciando teste da biblioteca CustomImageLoader ---" << std::endl;
    CustomImageLoader::ImageData image = CustomImageLoader::load_jpeg(test_file);

    if (image.is_valid) {
        std::cout << "\nImagem carregada com sucesso (Lógica de Decodificacao deve ser concluida)." << std::endl;
        std::cout << "Dimensoes detectadas: " << image.width << "x" << image.height << std::endl;
    } else {
        std::cout << "\nCarregamento iniciado. Lógica de decodificacao (leitura de marcadores) é o proximo passo!" << std::endl;
    }
    
    // Remove o arquivo de teste para limpeza
    std::remove(test_file.c_str());

    return 0;
}
