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

string classificador(Ppm& modelo_ia, Ppm& modelo_humano, string path_arquivo,bool teste_unitario){
    ifstream arquivo(path_arquivo, ios::binary);
    if(!arquivo.is_open()){
        cerr << "Erro ao abrir: " << path_arquivo << endl;
        return "";
    }

    double comprimento_ia = comprimento_do_arquivo(arquivo, modelo_ia);
    arquivo.clear();
    arquivo.seekg(0);
    double comprimento_humano = comprimento_do_arquivo(arquivo, modelo_humano);
    if(teste_unitario){
        cout << "Arquivo: " << path_arquivo << endl;
        cout << "Comprimento IA: " << comprimento_ia << endl;
        cout << "Comprimento Humano: " << comprimento_humano << endl;
    }
    if(comprimento_ia < comprimento_humano) return "IA";
    else return "Humano";
}

void teste_geral(Ppm& modelo_ia,Ppm&modelo_humano,string& path_ia, string& path_humano){
    string path;
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
            string classe = classificador(modelo_ia,modelo_humano,entrada.path(),false);
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
            string classe = classificador(modelo_ia,modelo_humano,entrada.path(),false);
            if(classe == "IA")corretos++;
            total++;
        }
    }

    double acuracia = 100.0 * corretos / total;

    cout << "Corretos: " << corretos << "/" << total << endl;
    cout << "Porcentagem de Acerto: " << acuracia << "%" << endl;
}

void teste_geral_tipo(Ppm& modelo_ia,Ppm& modelo_humano, string tipo){

    vector<string> arquivos_ia = {
    "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/ai_teste/c",
    "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/ai_teste/cpp",
    "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/ai_teste/java",
    "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/ai_teste/py"};

    vector<string> arquivos_humano{
        "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/human_teste/c",
        "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/human_teste/cpp",
        "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/human_teste/java",
        "/home/eduardo/Faculdade/Introdução a Teoria da Informação/Projeto-2/dataset/human_teste/py"
    };
    if(tipo == "c"){
        teste_geral(modelo_ia,modelo_humano,arquivos_ia[0],arquivos_humano[0]);
    }else if(tipo == "cpp"){
        teste_geral(modelo_ia,modelo_humano,arquivos_ia[1],arquivos_humano[1]);
    }else if(tipo == "java"){
        teste_geral(modelo_ia,modelo_humano,arquivos_ia[2],arquivos_humano[2]);
    }else if(tipo == "py"){
        teste_geral(modelo_ia,modelo_humano,arquivos_ia[3],arquivos_humano[3]);
    }   
}


void teste_arquivo(Ppm& modelo_ia,Ppm&modelo_humano,string& path_teste){
    string classe = classificador(modelo_ia,modelo_humano,path_teste,true);
    cout << "Arquivo: " << path_teste << " -> Classe: " << classe << endl;
}

