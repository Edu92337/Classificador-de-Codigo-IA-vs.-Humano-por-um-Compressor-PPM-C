#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <system_error>
#include <cstdlib>
#include "codificador_aritmetico.hpp"
#include "estrutura_contexto.hpp"
#include "ppm.hpp"
#include"utils.hpp"
using namespace std;
namespace fs = filesystem;

int main(int argc, char* argv[]){
    int k;
    if(argc != 3 && argc != 4){
        cerr << "Uso:" << endl;
        cerr << "  Testar um arquivo especifico: " << argv[0] << " <Kmax> <path_arquivo> <tipo>" << endl;
        cerr << "  Testar o dataset inteiro:     " << argv[0] << " <Kmax> <tipo>" << endl;
        return 1;
    }
    if(argc == 4){
        //Teste de um arquivo de um tipo 
        // Como usar : ./a k path_arquivo tipo
        k = atoi(argv[1]);
        string path_arquivo = argv[2];
        string tipo = argv[3];
        Ppm modelo_ia(k,true);
        Ppm modelo_humano(k,true);
        treina_modelo(modelo_ia,true,tipo);
        treina_modelo(modelo_humano,false,tipo);
        modelo_ia.treino = false;
        modelo_humano.treino = false;
        teste_arquivo(modelo_ia,modelo_humano,path_arquivo);
    }else {
        //Teste geral de todos os arquivos do dataset pelo tipo
        //Como usar : ./a k tipo
        k = atoi(argv[1]);
        string tipo = argv[2];
        Ppm modelo_ia(k,true);
        Ppm modelo_humano(k,true);

        treina_modelo(modelo_ia,true,tipo);
        treina_modelo(modelo_humano,false,tipo);

        modelo_ia.treino = false;
        modelo_humano.treino = false;
        teste_geral_tipo(modelo_ia,modelo_humano,tipo);
    }
    
    return 0;
}