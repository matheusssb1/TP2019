#include "MaquinaDeBusca.hpp"
#include <fstream>
#include <iostream>
#include <cmath>
#include <locale>

using namespace std;

// Construtor
MaquinaDeBusca::MaquinaDeBusca()
{

    // Documento atual e o primeiro
    docatual = 0;

}

// Le um documento
bool MaquinaDeBusca::lerDocumento(std::string documento)
{

    // Abre o documento
    ifstream f(documento.c_str());

    // Verifica se o documento pode ser lido
    if(f.is_open()) {

        // Documento a ser armazenado
        Documento d;
        d.nome = documento;
        d.maxocorr = 0;

        // Leitura
        string s;
        s.reserve(128);
        char c;

        // Checa o numero de ocorrencias de uma palavra
        unsigned int vezes;

        while (!f.eof())
        {

            // Limpa a string
            s.clear();

            while(1) {

                // Le um novo caractere e o transforma em minusculo
                f.get(c);

                // Checa se o caractere e um numero ou palavra, se nao, fecha a string
                if(f.eof() || (!isalpha(c) && !isdigit(c) && (c != '-') && (c != '\''))) break;

                // Retira caracteres especiais das palavras
                if(c != '-' && c != '\'') {
                    // Insere o caractere
                    s.insert(s.end(), tolower(c));
                }
            }

            // Adiciona a palavra
            if(s[0] != '\0') {

                vezes = adicionarPalavra(s);

                // Caso a palavra seja a de maior ocorrencia, armazena o seu numero de ocorrencias
                if(vezes > d.maxocorr) {
                    d.maxocorr = vezes;
                }
            }


        }

        // Indica o proximo documento apos o fim da leitura
        docatual++;

        // Armazena o documento
        documentos.push_back(d);

        // Arquivo lido com sucesso
        return true;
    }

    // Erro de leitura do documento
    return false;
}

// Adiciona uma palavra
unsigned int MaquinaDeBusca::adicionarPalavra(const std::string palavra)
{

    // Percorre a lista e ve se encontra a palavra
    for(list<Palavra>::iterator it=lpal.begin(); it != lpal.end(); ++it)
    {

        // Se encontrado a palavra, aumenta o seu numero de ocorrencias
        if(!it->palavra.compare(palavra))
        {

            // Verifica se o documento ja possui esta palavra
            if(it->docs.back() == docatual)
            {
                // Aumenta o numero de ocorrencias no documento
                it->vezes.back()++;
            }
            else {

                // Adiciona o documento na palavra
                it->docs.push_back(docatual);
                it->vezes.push_back(1);
            }

            // Aumenta o numero de ocorrencias totais da palavra
            it->total++;

            // Retorna o numero de ocorrencias da palavra
            return it->vezes.back();
        }
    }

    // Se chegou aqui, a palavra ainda nao foi adicionada entao a adiciona
    Palavra p;
    p.palavra = palavra;
    p.total = 1;
    p.docs.push_back(docatual);
    p.vezes.push_back(1);

    // Adiciona a palavra
    lpal.push_back(p);

    return 1;

}

// Apaga os dados da classe
void MaquinaDeBusca::apagar()
{

    // Limpa a memoria
    lpal.clear();
    documentos.clear();

    // Documento atual e o primeiro
    docatual = 0;

}

// Retorna a frequencia normalizada de um termo em um documento
double MaquinaDeBusca::tf(const Palavra pal, const unsigned int doc)
{

    for(unsigned int i = 0; i < pal.docs.size(); i++) {
        // Caso encontre a palavra, retorna a sua frequencia normalizada no documento
        if(pal.docs[i] == doc) {
            return ((double)pal.vezes[i]);//documentos[doc].maxocorr;
        }
    }

    // Palavra nao encontrada no documento
    return 0;
}

// Retorna a idf de um termo
double MaquinaDeBusca::idf(const Palavra pal)
{
    return log((double)documentos.size()/(double)pal.docs.size());
}

void MaquinaDeBusca::tabelahash(double **th, const vector<string>consulta) {

    // Vetor de IDFs calculados
    double IDF[consulta.size()];
    vector<list<Palavra>::iterator> itp(consulta.size());

    // Iterador de palavras
    list<Palavra>::iterator it;

    // Palavra atual de consulta
    string p;

    unsigned int i, j, k;

    // Percorre as consultas e armazena os IDF's
    for(i = 0; i < consulta.size(); i++)
    {
        p = consulta[i];
        IDF[i] = 0.0;

        for(it=lpal.begin(); it != lpal.end(); ++it)
        {
            // Encontrou a palavra
            if(!it->palavra.compare(p))
            {
                IDF[i] = idf(*it);
                itp[i] = it;
                break;
            }
        }
    }


    for(i = 0; i < documentos.size(); i++)
    {

        // Armazena o valor da cordenada de cada termo no documento
        for(j = 0; j < consulta.size(); j++)
        {
            if(IDF[j] != 0.0) th[i][j] = IDF[j]*tf(*itp[j], i);
            else th[i][j] = 0.0;
        }

    }

    // Numero de vezes que o termo aparece na consulta
    double vezes;

    // Armazena o valor da coordenada de cada termo no documento
    for(j = 0; j < consulta.size(); j++)
    {

        vezes = 0.0;

        if(IDF[j] != 0.0) {
            for(k = 0; k < consulta.size(); k++) {
                // Caso encontre a palavra, retorna a sua frequencia normalizada no documento
                if(!itp[j]->palavra.compare(consulta[k]))  vezes++;
            }
        }

        th[i][j] = IDF[j]*vezes;
    }

}

double MaquinaDeBusca::similaridade(double **th, const unsigned int doc, const vector<string>consulta)
{
     // Calculos
    double vd, vc;
    double num = 0.0;
    double dend = 0.0;
    double denc = 0.0;

    // Retorna a similaridade para cada termo e as soma
    for(unsigned int i = 0; i < consulta.size(); i++) {
        vd = th[doc][i];
        vc = th[documentos.size()][i];
        //cout << "VD: " << vd << " VC: " << vc << " " << endl;
        // Soma os novos valores calculados
        num += vd*vc;
        dend += vd*vd;
        denc += vc*vc;
        //cout << num << " " << dend << " " <<  denc << " " << sqrt(dend)*sqrt(denc) << " " << num/(sqrt(dend)*sqrt(denc)) << endl;
    }

    // Retorna a similaridade calculada
    if(num == 0.0)  return 0.0;
    return num/(sqrt(dend)*sqrt(denc));
}

// Ranking para dada consulta
void MaquinaDeBusca::ranking(const vector<string>consulta)
{

    // Verifica se existem documentos e consultas
    if(documentos.size()) {

        if(!consulta.size()) {
            cout << "Nenhuma consulta realizada";
            return;
        }

        Similaridade s[documentos.size()];
        Similaridade aux;

        unsigned int i, j;
        double **th = new double*[documentos.size()+1];
        for(i = 0; i <= documentos.size(); i++) th[i] = new double[consulta.size()];
        tabelahash(th, consulta);

        // Armazena o primeiro dado
        s[0].sim = similaridade(th, 0, consulta);
        s[0].pos = 0;

        // Retorna a similaridade para cada termo e as soma
        for(i = 1; i < documentos.size(); i++) {

            // Armazena um novo dado
            aux.sim = similaridade(th, i, consulta);
            aux.pos = i;

            // Coloca o dado na posicao correta
            for(j = i-1; j > 0 && s[j].sim < aux.sim; j--) {
                s[j+1] = s[j];
            }
            // Verifica a ultima posicao de j para evitar erros
            if(j == 0 && s[j].sim < aux.sim) {
                s[1] = s[0];
                s[0] = aux;
            }
            else {
                s[j+1] = aux;
            }

        }

        for(i = 0; i <= documentos.size(); i++) delete[] th[i];
        delete[] th;

        // Mostra o ranking
        unsigned int pos = 1;
        double simant = s[0].sim;

        cout << "Posicao: 1 - Documentos: " << documentos[s[0].pos].nome << " ";

        for(i = 1; i < documentos.size() ; i++)
        {
            // Se a similaridade nao for igual, cria uma nova posicao
            if(simant != s[i].sim) {
                cout << "(" << simant << ")\nPosicao: " << ++pos << " - Documentos: ";
                simant = s[i].sim;
            }

            cout << documentos[s[i].pos].nome << " ";
        }

        cout << "(" << simant << ")" << endl;

    }

}

// Mostra todas palavras armazenadas
void MaquinaDeBusca::mostrarPalavras() {

    for(list<Palavra>::iterator it=lpal.begin(); it != lpal.end(); ++it)
    {
        cout << "Palavra: " << it->palavra << " - Total: " << it->total << "\n";
    }

    cout << endl;
}
