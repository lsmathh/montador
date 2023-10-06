#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <regex>
#include <algorithm>

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
    if((int)v.size() > 30){
        return false;
    }

    //verificando inicial
    if(!((v[0] >= 'a' && v[0] <= 'z') || (v[0] >= 'A' && v[0] <= 'Z') || (v[0] == '_'))) {
        return false;
        }

    //verificando token
    int i = 1;  ;
    while((int)v.size() > i){
        if(!((v[i] >= 'a' && v[i] <= 'z') || (v[i] >= 'A' && v[i] <= 'Z') || (v[i] >= '0' && v[i] <= '9') || (v[i] == '_'))) return false;
        i++;
    }
    return true;
}


//MAIN
int main(int argc, char **argv){

    set<string> reserved_words = {"ADD", "SUB", "MUL", "DIV", "JMP", "JMPN", "JMPP", "JMPZ",
                                    "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP", "CONST", "SPACE"};

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

    //Verificar se possui seção TEXT
    bool text_section = false;

    int c_section = 0; // 1 - TEXT // 2 - DATA


    
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
        int pos = 0;
        if(line.find(";"))  pos = line.find(";");
        if(pos != string::npos) line = line.substr(0, pos);

    //Removendo TAB
        replace(line.begin(), line.end(), '\t', ' ');

    //Removendo espaçoes duplicados
        line = regex_replace(line, regex("^ +| +$|( ) +"), "$1");

    //Transformando o código em uppercase
        transform(line.begin(), line.end(), line.begin(), ::toupper);

    //Extrair o token da linha
        vector<string> tokens = getToken(line);


    //Gerar linha processada
        string code = "";
        for(int i = 0; i < (int)tokens.size(); i++) code += tokens[i] + " ";

        output_file << code << endl;
    }
    

    input_file.close();
    output_file.close();



    //Dar inicio a primeira passagem para gerar a tabela de simbolos
    
    ifstream input_f("pre_processed_file.txt");
    int PC = 0, line_counter = 1;

    while(getline(input_f, line)){
        vector<string> tokens = getToken(line);
        int has_label = 0;

        //verifica a secao
        if(line == "SECAO TEXT "){ 
            text_section = true;
            c_section = 1;
            line_counter++;
            continue;
        }else if(line == "SECAO DATA "){
            c_section = 2;
            line_counter++;
            continue;
        }

        //processamento da linha
        for(int i = 0; i < (int)tokens.size(); i++){
            string token = tokens[i];

            //verifica se label tem ":"
            if(!reserved_words.count(token) && i == 0 && token.back() != ':' && token != "SECAO"){
                cout << token << " label has missing ':'" << "\n";
            }

            //se for label
            if(token.back() == ':'){

                //remove o :
                string label = token.substr(0, token.size() - 1);

                //verifica se ja existe label na linha
                if(has_label == 1){
                    cout << "Syntactic error: Many labels at line " << line_counter << "\n"; 
                }
                //verifica se label possui nome reservado
                if(reserved_words.count(label)){
                    cout << "Semantic error: label with reserved name at line " << line_counter << "\n";
                }
                if(def_symbols.count(label)){
                    cout << "Semantic  error: label redefined at line" << line_counter << "\n";
                }else{
                    //verifica se esta escrito corretamente
                    if(!verifyToken(label)) cout << "Lexical error: token is not valid at line " << line_counter << "\n"; 
                    else{
                        def_symbols.insert(label);
                        symb_table[label] = PC; 
                    }
                has_label = 1;
                }
            }else if(def_opcodes.count(token)){
                if(c_section != 1) cout << "Semantic error: instruction defined at wrong section at line " << line_counter << "\n";

                PC += inst_size[token];
            }else if(def_directives.count(token)){
                if(token == "SPACE"){
                    if(c_section != 2) cout << "Semantic error: directive defined at wrong section at line " << line_counter << "\n";
                    PC++;
                }else if(token == "CONST"){
                    if(c_section != 2) cout << "Semantic error: directive defined at wrong section at line " << line_counter << "\n";
                    PC++;
                }
            }
        }
        line_counter++;
    }


    ifstream new_input_file("pre_processed_file.txt");
    ofstream out_file("montadores.obj");
    PC = 0, line_counter = 1;

    //segunda passagem
    while(getline(new_input_file, line)){
        //verifica a secao
        if(line == "SECAO TEXT "){
            text_section = true;
            c_section = 1;
            line_counter++;
            continue;
        }else if(line == "SECAO DATA "){
            c_section = 2;
            line_counter++;
            continue;
        }

        //removendo labels
        int label_pos = line.find_first_of(":");
        if (label_pos != string::npos) line = line.substr(label_pos + 2);

        int virgula_pos = line.find_first_of(",");
        if (virgula_pos != string::npos) line = line.replace(virgula_pos, 1, " ");


        //cout << line << "\n";
       

        //extraindo token
        vector<string> tokens = getToken(line);
        //cout << line << endl;
        

        for(int i = 0; i < tokens.size(); i++){
            if(i == 0){
                if(def_opcodes.count(tokens[i])){
                    PC += inst_size[tokens[i]];
                    if(tokens.size() == 3){
                        cod_objeto.push_back(opcodes_values[tokens[i]]);
                        cod_objeto.push_back(to_string(symb_table[tokens[i+1]]));
                        cod_objeto.push_back(to_string(symb_table[tokens[i+2]]));
                    }else if(tokens.size() == 2 && i == 0){
                        cod_objeto.push_back(opcodes_values[tokens[i]]);
                        cod_objeto.push_back(to_string(symb_table[tokens[i+1]]));
                    }else if(tokens.size() == 1){
                        cod_objeto.push_back(opcodes_values[tokens[i]]);
                    }
                }else if(def_directives.count(tokens[i])){
                    if(tokens[i] == "SPACE"){
                            PC++;
                            cod_objeto.push_back("0");

                        }
                        if(tokens[i] == "CONST"){
                            if(i+1 < tokens.size()){
                                cod_objeto.push_back(tokens[i+1]);
                                PC++;
                            }else{
                                cout << "Syntactic error: Missing a value of CONST at line " << line_counter << "\n"; 
                            }
                            
                        } 
                }
            }else if((i == 1 || i == 2) && tokens[i-1] != "CONST"){
                if(!def_symbols.count(tokens[i])) cout << "Semantic error: Symbol not defined at line " << line_counter << "\n";
            }
        
        }
            
        line_counter++;
    }

    for(int i = 0; i < (int)cod_objeto.size(); i++) out_file << cod_objeto[i] << " ";

    out_file.close();
    new_input_file.close();

}
    




 // for(const auto&  it: symb_table){
    //     cout << "{" << it.first << "," << it.second << "}" << "\n";
    // }

