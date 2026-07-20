#pragma once
#include<iostream>
#include<map>
#include<unordered_map>
#include<algorithm>
#include<array>
#include<deque>
#include<cstdint>
#include<vector>
#include<queue>
using namespace std;
typedef struct No No;

const uint32_t ESCAPE = 256;

// cada No é um contexto 
// a ideia seria percorrer até a folha e codificar na subida, para dar prioridade ao maior contexto
// Kmax <= 10 => O(Kmax)<= O(10)
struct No{
    // implementação original uint8_t filhos[257]
    unordered_map<uint8_t, No*> filhos;
    // armazena as frequencias de cada byte no contexto
    array<uint32_t,257>frequencias{};
    No* pai; 
    uint32_t total; //soma de todas as frequências
    uint16_t distintos = 0;
    No(){
        frequencias.fill(0);
        pai = nullptr;
        total = 0;

    }
    
};

// Estrutura relacional que conecta contextos semelhantes
// EX: simbolo : abc => r - c -bc -abc
struct trie_contexto{
    No* raiz;
    uint64_t num_nos = 1; // conta nós alocados (começa com 1 pela raiz)

    trie_contexto(){
        raiz = new No();
    }
    ~trie_contexto(){
        libera(raiz);
    }

    void libera(No* no){
        if(!no) return;
        for(auto& [_, filho] : no->filhos)
            libera(filho);
        num_nos--;
        delete no;
    }

    // a partir do no Raiz vai percorrendo e fazendo a ligação do byte com todos os outros Nós.
    // Retorna true se todos os nós foram inseridos, false se o limite foi atingido
    // e a inserção foi interrompida (nós parcialmente inseridos até o ponto do limite).
    bool insere_byte_em_contexto(const deque<uint8_t>&bytes){
        No* atual = raiz;
        for(auto it = bytes.rbegin(); it != bytes.rend(); it++){
            uint8_t b = *it;
            if(atual->filhos.find(b) == atual->filhos.end()){
                atual->filhos[b] = new No;
                atual->filhos[b]->pai = atual;
                num_nos++;
            }
            atual = atual->filhos[b];
        }
        return true;
    }
    // A partir da raiz vai percorrendo os nos que estão conectados/ contextos conectados 
    //até encontrar o maior contexto possível 
    No* busca_contexto_byte(const deque<uint8_t>&contexto){
        No* atual = raiz;
        for(auto it = contexto.rbegin();it!=contexto.rend();it++){
            uint8_t b = *it;
            if(atual->filhos.find(b) != atual->filhos.end())atual = atual->filhos[b];
            else return atual;
        }
        return atual;
    }


    // faz a propagação para todos os contextos menores até a raiz
    void atualiza_frequencia(No* contexto, uint8_t simbolo) {
        while (contexto != nullptr) {
            if (contexto->frequencias[simbolo] == 0) contexto->distintos++;
            if (contexto->frequencias[simbolo] == 0) contexto->distintos++;

            contexto->frequencias[simbolo]++;
            contexto->total++;
            
            contexto = contexto->pai;
        }
    }

    


};