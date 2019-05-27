#include <iostream>
#include "MaquinaDeBusca.hpp"

using namespace std;

int main(int argc, char *argv[])
{

    // Manipulacao da classe
    MaquinaDeBusca m;
    vector<string>consulta;

    // Leitura dos documentos
    string s;
    for(int i = 1; i < argc; i++) {
        s.assign(argv[i]);
        m.lerDocumento(s);
    }

    // Leitura
    char c;
    cout << "Digite a consulta desejada\n";
    do
    {
        // Limpa a string
        s.clear();

        // Le o novo caractere
        cin.get(c);

        // Checa se o caractere e valido
        while(isalpha(c) || isdigit(c)) {
            s.insert(s.end(), tolower(c));
            cin.get(c);
        }

        // Insere a string
        if(!s.empty()) consulta.push_back(s);

    } while(c != '\n');

    // Mostra as palavras presentes no documento
    //m.mostrarPalavras();

    // Mostra o ranking de dada busca
    m.ranking(consulta);

    // Apaga os dados da classe
    m.apagar();

    return 0;
}
