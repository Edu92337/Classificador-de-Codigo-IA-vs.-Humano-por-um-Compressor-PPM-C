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
    vector<pair<uint8_t,No*>> filhos;
    vector<pair<uint16_t,uint32_t>> frequencias; // (simbolo, frequencia) — só entradas que existem
    No* pai;
    uint32_t total;
    uint16_t distintos = 0;

    No(){
        pai = nullptr;
        total = 0;
    }

    uint32_t get_freq(uint16_t simbolo) const{
        for(const auto& [s,f] : frequencias)
            if(s == simbolo) return f;
        return 0;
    }

    uint32_t& freq_ref(uint16_t simbolo){
        for(auto& [s,f] : frequencias)
            if(s == simbolo) return f;
        frequencias.push_back({simbolo, 0});
        return frequencias.back().second;
    }

    No* busca_filho(uint8_t b) const{
        for(const auto& [byte,no] : filhos)
            if(byte == b) return no;
        return nullptr;
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
            No* filho = atual->busca_filho(b);
            if(!filho){
                filho = new No();
                filho->pai = atual;
                atual->filhos.push_back({b, filho});
                num_nos++;
            }
            atual = filho;
        }
        return true;
    }
    // A partir da raiz vai percorrendo os nos que estão conectados/ contextos conectados 
    //até encontrar o maior contexto possível 
    No* busca_contexto_byte(const deque<uint8_t>&contexto){
        No* atual = raiz;
        for(auto it = contexto.rbegin();it!=contexto.rend();it++){
            uint8_t b = *it;
            No* filho = atual->busca_filho(b);
            if(filho) atual = filho;
            else return atual;
        }
        return atual;
    }


    // faz a propagação para todos os contextos menores até a raiz
    void atualiza_frequencia(No* contexto, uint8_t simbolo) {
        while (contexto != nullptr) {
            uint32_t& f = contexto->freq_ref(simbolo);
            if(f == 0) contexto->distintos++;
            f++;
            contexto->total++;
            contexto = contexto->pai;
        }
    } 


};