#include "image.h" 
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex> // Usamos expressões regulares para parsing básico (C++11+)
#include <algorithm>

namespace CustomImageLoader {

// ====================================================================
// FUNÇÃO PRINCIPAL PARA CARREGAR O SVG
// ====================================================================

ImageData load_svg(const std::string& filename) {
    ImageData img;
    img.channels = 4; // Assume RGBA para a saída final (se for rasterizado)

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "ERRO SVG: Nao foi possivel abrir o arquivo: " << filename << std::endl;
        return img;
    }

    // Leitura do arquivo inteiro em uma string (SVG são tipicamente pequenos)
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string svg_content = buffer.str();
    file.close();

    // 1. Verificação e Leitura das Dimensões (Parsing Básico)
    
    // A tag <svg> contém width, height e, opcionalmente, viewBox.
    // Usamos regex para uma análise muito simplificada e básica da tag SVG.
    // NOTE: Isto não é um parser XML robusto! É apenas para extrair os dados mais básicos.
    std::regex svg_tag_regex("<svg[^>]*>");
    std::smatch match;

    if (std::regex_search(svg_content, match, svg_tag_regex)) {
        std::string svg_tag = match.str(0);
        
        // Extrair Largura
        std::regex width_regex("width=\"([^\"]+)\"");
        if (std::regex_search(svg_tag, match, width_regex)) {
            // Simplificamos: assume que é em pixels e converte (ex: "100px" -> 100)
            std::string width_str = match.str(1);
            try {
                img.width = std::stoi(width_str);
            } catch (...) { img.width = 0; }
        }

        // Extrair Altura
        std::regex height_regex("height=\"([^\"]+)\"");
        if (std::regex_search(svg_tag, match, height_regex)) {
            std::string height_str = match.str(1);
            try {
                img.height = std::stoi(height_str);
            } catch (...) { img.height = 0; }
        }
    }

    if (img.width <= 0 || img.height <= 0) {
        std::cerr << "ERRO SVG: Nao foi possivel extrair dimensoes validas do <svg>." << std::endl;
        // Permite continuar para análise de elementos, mas a rasterização falharia.
    }
    
    // 2. Identificação de Elementos Vetoriais (Básico)
    
    // Identificamos algumas tags vetoriais chave (rect, circle, path).
    std::regex element_regex("<(rect|circle|path)[^>]*>");
    std::sregex_iterator next(svg_content.begin(), svg_content.end(), element_regex);
    std::sregex_iterator end;

    while (next != end) {
        img.vector_elements.push_back(next->str());
        next++;
    }

    img.is_valid = true;

    // ====================================================================
    // A PARTIR DAQUI, VOCÊ IMPLEMENTARÁ A RASTERIZAÇÃO
    // 
    // Para realmente carregar a imagem em um formato de pixels (ImageData),
    // é necessário:
    // 1. Interpretar a sintaxe de cada elemento em 'img.vector_elements'.
    // 2. Implementar um algoritmo de rasterização (preencher o vetor
    //    'img.pixel_data') usando geometria e antialiasing.
    // ====================================================================
    
    std::cout << "SUCESSO SVG: Cabecalho e elementos vetoriais lidos." << std::endl;
    std::cout << "Dimensoes Sugeridas: " << img.width << "x" << img.height << std::endl;
    std::cout << "Elementos Vetoriais detectados: " << img.vector_elements.size() << std::endl;
    std::cout << "O proximo passo (MUITO COMPLEXO) e a Rasterizacao desses elementos." << std::endl;

    // A rasterização preencheria:
    /*
    if (img.width > 0 && img.height > 0) {
        img.data_size = img.width * img.height * img.channels;
        img.pixel_data = std::make_unique<unsigned char[]>(img.data_size);
        // ... (Algoritmo de Rasterização preenche o pixel_data) ...
    }
    */
    
    return img;
}

} // namespace CustomImageLoader

// =========================================================================
// Exemplo de Uso (Função main)
// =========================================================================

/*
int main() {
    const std::string test_file = "TESTE_SVG_SIMPLES.svg"; 

    // Cria um arquivo SVG de teste simples
    std::ofstream test_out(test_file);
    if (test_out.is_open()) {
        test_out << "<svg width=\"200\" height=\"100\">" << std::endl;
        test_out << "  <rect x=\"10\" y=\"10\" width=\"50\" height=\"50\" fill=\"red\" />" << std::endl;
        test_out << "  <circle cx=\"150\" cy=\"50\" r=\"40\" fill=\"blue\" />" << std::endl;
        test_out << "  <path d=\"M10 80 L90 80 L50 20 Z\" fill=\"green\" />" << std::endl;
        test_out << "</svg>" << std::endl;
        test_out.close();
        std::cout << "Arquivo de teste '" << test_file << "' criado." << std::endl;
    }

    std::cout << "\n--- Iniciando teste do carregador SVG ---" << std::endl;
    CustomImageLoader::ImageData image = CustomImageLoader::load_svg(test_file);

    if (image.is_valid) {
        std::cout << "\nSVG carregado (Parsing OK)." << std::endl;
        std::cout << "Dimensoes lidas: " << image.width << "x" << image.height << std::endl;
        std::cout << "Elementos encontrados: " << image.vector_elements.size() << std::endl;
        // Se você quisesse ver os elementos lidos:
        // for (const auto& elem : image.vector_elements) {
        //     std::cout << "  - " << elem << std::endl;
        // }
    } else {
        std::cout << "\nFalha na validacao ou leitura do arquivo SVG." << std::endl;
    }
    
    // std::remove(test_file.c_str());

    return 0;
}
*/
