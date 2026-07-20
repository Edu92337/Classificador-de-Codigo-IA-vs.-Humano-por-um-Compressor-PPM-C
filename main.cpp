#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <system_error>
#include <cstdlib>
#include "codificador_aritmetico.hpp"
#include "estrutura_contexto.hpp"
#include "ppm.hpp"

using namespace std;
namespace fs = filesystem;


void codifica_arquivo(ifstream& arquivo, Ppm& modelo, bool metricas){
    char byte;
    while(arquivo.get(byte)){
        modelo.processa_simbolo((uint8_t)byte);
    }
}

double comprimento_do_arquivo(ifstream& arquivo, Ppm& modelo){
    uint64_t bits_antes = modelo.aritmetico.bits_emitidos_total;
    uint64_t simbolos_antes = modelo.total_simbolos_processados;

    codifica_arquivo(arquivo, modelo, false);
    uint64_t n = modelo.total_simbolos_processados - simbolos_antes;
    if(n == 0) return 0.0;
    uint64_t bits_depois = modelo.aritmetico.bits_emitidos_total;
    return (double)(bits_depois - bits_antes) / (double)n;
}

void treina_modelo(string path, Ppm& modelo){
    error_code ec;
    for(auto& entrada : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied, ec)){
        if(ec){ cerr << "Erro: " << ec.message() << endl; return; }

        if(fs::is_regular_file(entrada.status())){
            ifstream arquivo(entrada.path(), ios::binary);
            if(!arquivo.is_open()){
                cerr << "Erro ao abrir: " << entrada.path() << endl;
                return;
            }
            codifica_arquivo(arquivo, modelo, false);
        }
    }
}

string classificador(Ppm& modelo_ia, Ppm& modelo_humano, string path_arquivo){
    ifstream arquivo(path_arquivo, ios::binary);
    if(!arquivo.is_open()){
        cerr << "Erro ao abrir: " << path_arquivo << endl;
        return "";
    }

    double comprimento_ia = comprimento_do_arquivo(arquivo, modelo_ia);
    arquivo.clear();
    arquivo.seekg(0);
    double comprimento_humano = comprimento_do_arquivo(arquivo, modelo_humano);

    /*cout << "Bits IA: " << comprimento_ia << endl;
    cout << "Bits Humano: " << comprimento_humano << endl;
    */
    if(comprimento_ia < comprimento_humano) return "IA";
    else return "Humano";
}

void teste_geral(Ppm& modelo_ia,Ppm&modelo_humano){
    string path;
    string path_ia = "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/ai_teste";
    string path_humano = "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/human_teste";

    long long corretos = 0;
    long long total = 0;
    // TESTE DOS CÓDIGOS HUMANOS
    path = path_humano;
    error_code ec;
    for(auto& entrada : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied, ec)){
        if(ec){ cerr << "Erro: " << ec.message() << endl; return; }

        if(fs::is_regular_file(entrada.status())){
            ifstream arquivo(entrada.path(), ios::binary);
            if(!arquivo.is_open()){
                cerr << "Erro ao abrir: " << entrada.path() << endl;
                return;
            }
            string classe = classificador(modelo_ia,modelo_humano,entrada.path());
            if(classe == "Humano")corretos++;
            total++;
        }
    }
    // TESTE DOS CÓDIGOS GERADOS POR IA
    path = path_ia;
    for(auto& entrada : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied, ec)){
        if(ec){ cerr << "Erro: " << ec.message() << endl; return; }

        if(fs::is_regular_file(entrada.status())){
            ifstream arquivo(entrada.path(), ios::binary);
            if(!arquivo.is_open()){
                cerr << "Erro ao abrir: " << entrada.path() << endl;
                return;
            }
            string classe = classificador(modelo_ia,modelo_humano,entrada.path());
            if(classe == "IA")corretos++;
            total++;
        }
    }

    double acuracia = 100.0 * corretos / total;

    cout << "Corretos: " << corretos << "/" << total << endl;
    cout << "Porcentagem de Acerto: " << acuracia << "%" << endl;
}

void teste_arquivo(Ppm& modelo_ia,Ppm&modelo_humano,string& path_teste){
    string classe = classificador(modelo_ia,modelo_humano,path_teste);
    cout << "Arquivo: " << path_teste << " -> Classe: " << classe << endl;
}

int main(int argc, char* argv[]){
    int k;
    if(argc < 2){
        cerr << "Uso: " << argv[0] << " <Kmax> <path_arquivo>" << endl;
        return 1;
    }
    if(argc == 3){
        //Teste de um arquivo 
        k = atoi(argv[1]);
        string path_arquivo = argv[2];
        Ppm modelo_ia(k);
        Ppm modelo_humano(k);

        string path = "dataset/ai_treino";
        treina_modelo(path, modelo_ia);
        
        path = "dataset/human_treino";
        treina_modelo(path, modelo_humano);

        teste_arquivo(modelo_ia,modelo_humano,path_arquivo);
    }else if(argc<3){
        //Teste geral de todos os arquivos do dataset
        k = atoi(argv[1]);
        Ppm modelo_ia(k);
        Ppm modelo_humano(k);

        string path = "dataset/ai_treino";
        treina_modelo(path, modelo_ia);
        
        path = "dataset/human_treino";
        treina_modelo(path, modelo_humano);
        teste_geral(modelo_ia,modelo_humano);
    }
    

    
    return 0;
}