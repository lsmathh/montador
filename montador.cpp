#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <set>

using namespace std; 

vector<string> getToken(string); 
bool verifyToken(string);



//GERANDO TOKENS
vector<string> getToken(string l){
    vector<string> tokens;
    string aux;
    stringstream check(l);

    while(getline(check, aux, ' ')){
        tokens.push_back(aux);
    }

    return tokens;
}


//VERIFICANDO TOKENS
bool verifyToken(string v){

    //tamanho do token
    if((int)v.size() > 30) return false;

    //verificando inicial
    if(!(v[0] >= 'a' && v[0] <= 'z') || (v[0] >= 'A' && v[0] <= 'Z') || (v[0] == '_')) return false;

    //verificando token
    int i = 1;  ;
    while((int)v.size() > i){
        if(!(v[i] >= 'a' && v[i] <= 'z') || (v[i] >= 'A' && v[i] <= 'Z') || (v[i] >= '0' && v[i] <= '9') || (v[i] == '_')) return false;
        i++;
    }
    
    return true;
}


//MAIN
int main(int argc, char **argv){

    //opcodes
    vector<string> opcodes = {"ADD", "SUB", "MUL", "DIV", "JMP", "JMPN", "JMPP", "JMPZ",
                                    "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP"};
    //set de opcodes
    set<string> def_opcodes;

    //diretivas
    vector<string> directives = {"CONST", "SPACE"};

    //set de diretivas
    set<string> def_directives;

    //valores dos opcodes
    map<string, string> opcodes_values;

    //tamanhho de cada instrução
    map<string, int> inst_size;

    //codigo objeto
    vector<string> cod_objeto;

    //tabela de simbolos
    map<string, int> symb_table;

    //conjunto de simbolos definidos
    set<string> def_symbols;

    //tabela de definição
    map<string, int> definitions_table;

    //Verificar se possui seção TEXT
    bool text_section = false;


    
    //Inicializando os valores dos opcodes
    for(int i = 0; i < (int)opcodes.size(); i++){
        opcodes_values[opcodes[i]] = std::to_string(i + 1);
        inst_size[opcodes[i]] = 2;
        def_opcodes.insert(opcodes[i]);
    }
    inst_size["COPY"] = 3;
    inst_size["STOP"] = 1;

    //Inicializando diretivas
    for(int i = 0; i < (int)directives.size(); i++) def_directives.insert(directives[i]);


    /* Precisamos pre processar o arquivo para remover espaços duplicados, tornar não case-sensitive e outros problemas*/
    string line;
    ifstream input_file;
    ofstream output_file;

    input_file.open("myprogram.asm");
    output_file.open("pre_processed_file.txt");

    //Verificando se ocorreu tudo certo com os arquivos
    //Input
    if(!input_file.is_open()){
        cout << "Input file not found. \n";
        return 0;
    }
    //Output
    if(!output_file.is_open()){
        cout << " Output file not found. \n";
        return 0;
    }


    while(getline(input_file, line)){
    //Removendo comentários
    //Removendo TAB
    //Removendo espaçoes duplicados
    //Transformando o código em uppercase
    //Extrair o token da linha
    //Gerar linh    a processada
    }

    input_file.close();
    output_file.close();
    //Dar inicio a primeira passagem para gerar a tabela de simbolos
}





