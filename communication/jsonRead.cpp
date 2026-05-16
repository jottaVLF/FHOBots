#include "jsonRead.hpp"
#include "json.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>


const std::string jsonRead::STORAGE_FILENAME = "calibrations.json";

json jsonRead::loadData() {
    
    std::ifstream infile(STORAGE_FILENAME);
    if (infile.is_open()) {
        try {
            // Tenta ler e parsear o JSON do arquivo
            json data = json::parse(infile);
            infile.close();
            return data;
        } catch (const json::parse_error& e) {
            std::cerr << "AVISO: Arquivo " << STORAGE_FILENAME << " invalido. Criando novo." << std::endl;
            infile.close();
        }
    }
    // Retorna um objeto JSON vazio se o arquivo não existir ou for inválido
    return json::object();
}

void jsonRead::saveData(const json& consolidatedData) {
    std::ofstream outfile(STORAGE_FILENAME);

    if (!outfile.is_open()) {
        std::cout << "ERRO: Nao foi possivel abrir o arquivo " << STORAGE_FILENAME << " para escrita." << std::endl;
        return;
    }

    // Escreve o objeto JSON no arquivo
    outfile << consolidatedData.dump(4);
    outfile.close();

    std::cout << "SUCESSO: Dados salvos em " << STORAGE_FILENAME << std::endl;
}


void jsonRead::handlePacket(const std::string& json_string) {
    try {
        //Tenta fazer o parsing da string JSON
        json incomingData = json::parse(json_string);
        
        std::cout << "JSON PARSEADO com sucesso. Atualizando dados existentes..." << std::endl;

        //Carrega o arquivo JSON de calibração existente
        json consolidatedData = loadData();

        //Itera sobre os dados recebidos (deve ter apenas uma chave raiz: 'atacante', 'defender', etc.)
        for (auto const& [key, value] : incomingData.items()) {
            std::string vertente = key; 
            
            if (value.is_object()) {
                std::cout << "Vertente atualizada: '" << vertente << "'" << std::endl;
                
                //Substitui/Atualiza a vertente específica no objeto consolidado
                consolidatedData[vertente] = value;
                
                //Salva o objeto JSON consolidado de volta no disco
                saveData(consolidatedData);
                return; 
            }
        }
        
        std::cerr << "AVISO: JSON recebido nao contem uma vertente valida." << std::endl;

    } catch (const json::parse_error& e) {
        std::cerr << "ERRO de Parsing JSON: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "ERRO geral no processamento: " << e.what() << std::endl;
    }
}

