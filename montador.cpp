#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <regex>
#include <algorithm>
#include <string>

using namespace std;


//Declarando funções
string scanner(string file);
bool isValidLabel(const string& label);
vector<string> getToken(string l);


//Gerando tokens
vector<string> getToken(string l){
    vector<string> tokens;
    string aux;
    stringstream check(l);

    while(getline(check, aux, ' ')){
        tokens.push_back(aux);
    }

    return tokens;
}

//Funcão que verifica se é número
bool isNumeric(std::string const &str)
{ 
    auto it = str.begin();
    auto x = str.begin();
    if(*x == '-'){
        it++;
        while (it != str.end() && (std::isdigit(*it))) {
        it++;
        }
    } else {
        while (it != str.end() && (std::isdigit(*it))) {
        it++; 
        }
    }
    return !str.empty() && it == str.end();
}

//FUNÇÃO PARA VALIDAR SIMBOLOS
bool isValidLabel(const string& label){
    if(!isNumeric(label)){
        if(!(label.size() <= 30)) { 
            cout << "O rótulo "  << label << " possui mais de 30 caracteres" << endl;
            return false;
        }

        if(!((label[0] >= 'a' && label[0] <= 'z') || (label[0] >= 'A' && label[0] <= 'Z') || (label[0] == '_'))) {
            cout << "Invalid label " << label << "\n";
            return false;
        }

        int i = 1; 
        while((int)label.size() > i){
            if(!((label[i] >= 'a' && label[i] <= 'z') || (label[i] >= 'A' && label[i] <= 'Z') || (label[i] >= '0' && label[i] <= '9') || (label[i] == '_'))) return false;
            i++;
        }

        return true;
    }
    return true;
}

//FUNÇÃO PARA O PRÉ PROCESSAMENTO
string pre_processing(string fname){
    ifstream inputFile(fname);
    ofstream outputFile("pre_processed.txt");
    string output = "pre_processed.txt";
    string code_data, code_text, code_section = "";
    int flag = 0;

    if (!inputFile) {
        cerr << "Error to create the input file." << endl;
    }

    if (!outputFile) {
        cerr << "Error to create the output file." << endl;
    }

    regex pattern("\\s+"); // Expressão regular para encontrar espaços em branco repetidos
    string line;

    while (getline(inputFile, line)) {

        //Remove comentários
        int pev = 0; 
        if(line.find(";")) pev = line.find(";");
        if(pev != string::npos) line = line.substr(0, pev);

        // Remove tabulações, quebras de linha e espaços desnecessários
        string processedLine = regex_replace(line, pattern, " ");

        //Tornando o código não sensível ao caso
        transform(processedLine.begin(), processedLine.end(), processedLine.begin(), ::toupper);

        //Removendo espaço do final das linhas 
        while (!processedLine.empty() && std::isspace(processedLine.back())) {
            processedLine.pop_back();
        }

        if(processedLine == "SECAO DATA") flag = 1;
        if(processedLine == "SECAO TEXT") flag = 0;

        if(flag == 0){
            code_text += processedLine + "\n";
        }
        if(flag == 1){
            code_data += processedLine + "\n";
        }
    }
    code_section += code_text;
    code_section += code_data;

    // Escreve a linha processada no arquivo de saída
    outputFile << code_section << endl;

    cout << "Arquivo pré-processado com sucesso!" << std::endl;

    return output;
}


int main(int argc, char **argv) {

    vector<string> instructions = {"ADD", "SUB", "MUL", "DIV", "JMP", "JMPN", "JMPP", "JMPZ",
                                    "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP"};

    set<string> reserved_words = {"ADD", "SUB", "MUL", "DIV", "JMP", "JMPN", "JMPP", "JMPZ",
                                    "COPY", "LOAD", "STORE", "INPUT", "OUTPUT", "STOP", "CONST", "SPACE", "MACRO", "ENDMACRO"};
    set<string> def_instructions;

    vector<string> directives = {"CONST", "SPACE"};

    set<string> def_directives;
    
    map<string, string> opcode_values;

    map<string, int> inst_size;

    vector<string> cod_objeto; 

    map<string, int> symb_table;

    set<string> def_symb;
    

    //Iniciando os valores dos OPCODES e declarando o tamanho das instruções;
    for(int i = 0; i < (int)instructions.size(); i++){
        opcode_values[instructions[i]] = to_string(i + 1);
        inst_size[instructions[i]] = 2;
        def_instructions.insert(instructions[i]);
    }
    inst_size["COPY"] = 3;
    inst_size["STOP"] = 1; 


    //Inicializando os valores das diretivas
    for(int i = 0; i < (int)directives.size(); i++) def_directives.insert(directives[i]);

    //Aqui vamos pré processar o código asm através da linha de comando;
    string fname = argv[1];
    pre_processing(fname);


    //Dar inicio a primeira passagem para gerar a tabela de simbolos
    
    ifstream input_f("pre_processed.txt");
    int PC = 0, line_counter = 1;
    string line;
    int c_section;
    bool text_section = false;

    

    while(getline(input_f, line)){
        vector<string> tokens = getToken(line);
        int has_label = 0;

        //verifica a secao
        if(line == "SECAO TEXT"){ 
            text_section = true;
            c_section = 1;
            line_counter++;
            continue;
        }else if(line == "SECAO DATA"){
            c_section = 2;
            line_counter++;
            continue;
        }

        //processamento da linha
        for(int i = 0; i < (int)tokens.size(); i++){
            string token = tokens[i];

            //verifica se label tem ":"
            if(!reserved_words.count(token) && i == 0 && token.back() != ':' && token != "SECAO"){
                cout << token << " label has missing ':' at line " << line_counter << "; \n";
                
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
                if(def_symb.count(label)){
                    cout << "Semantic  error: label redefined at line " << line_counter << "\n";
                }else{
                    //verifica se esta escrito corretamente
                    if(!isValidLabel(label)) cout << "Lexical error: Token is not valid at " << line_counter << "\n"; 
                    else{
                        def_symb.insert(label);
                        symb_table[label] = PC; 
                    }
                has_label = 1;
                }
            }else if(def_instructions.count(token)){
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


    ifstream new_input_file("pre_processed.txt");
    ofstream out_file("myprogram.obj");
    PC = 0, line_counter = 1;


    //segunda passagem
    while(getline(new_input_file, line)){
        //verifica a secao
        if(line == "SECAO TEXT"){
            text_section = true;
            c_section = 1;
            line_counter++;
            continue;
        }else if(line == "SECAO DATA"){
            c_section = 2;
            line_counter++;
            continue;
        }

        //Removendo labels
        int label_pos = line.find_first_of(":");
        if (label_pos != string::npos) line = line.substr(label_pos + 2);

        //Removendo virgula
        int virgula_pos = line.find_first_of(",");
        if (virgula_pos != string::npos) line = line.replace(virgula_pos, 1, " ");

        

        //extraindo token
        vector<string> tokens = getToken(line);
        

        for(int i = 0; i < tokens.size(); i++){
            
            if(isValidLabel(tokens[i])){

                //verifica se é operador
                if(i == 0){
                    //verifica se eh uma instrucao
                    if(def_instructions.count(tokens[i])){
                        //verifica numeros de operandos das instrucoes
                        if(inst_size[tokens[i]] == (int)tokens.size()){
                            PC += inst_size[tokens[i]];
                        if(tokens.size() == 3){ //if COPY, cria cod obj para COPY
                            cod_objeto.push_back(opcode_values[tokens[i]]);
                            cod_objeto.push_back(to_string(symb_table[tokens[i+1]]));
                            cod_objeto.push_back(to_string(symb_table[tokens[i+2]]));
                        }else if(tokens.size() == 2 && i == 0){
                            cod_objeto.push_back(opcode_values[tokens[i]]);
                            cod_objeto.push_back(to_string(symb_table[tokens[i+1]]));
                        }else if(tokens.size() == 1){
                            cod_objeto.push_back(opcode_values[tokens[i]]);
                        }
                        }else{
                            cout << "Syntactic error: missing operators at line " << line_counter << "\n"; 
                        }
                    //verifica se eh uma diretiva
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
                    //nao eh diretiva e nem instrucao.. entao verifica se é o valor do const, caso nao seja, palavra invalida
                    }else{
                        if(!isNumeric(tokens[i])){
                            cout << "Semantic error: Symbol not defined at line " << line_counter << "\n";
                        }
                    }
                }else if((i == 1 || i == 2) && tokens[i-1] != "CONST"){
                    
                    if(!(def_symb.count(tokens[i]) || def_instructions.count(tokens[i]))) cout << "Semantic error: Symbol not defined at line " << line_counter << "\n";
                }
        
            }else{
                //Codigo esta dando erro lexico quando ADD recebe operando como "X+2" (NECESSARIO ARRUMAR)
                if(!isNumeric(tokens[i])) cout << ": token " << tokens[i] <<  " is not valid at line " << line_counter << "\n";
            }
        }
        line_counter++;
    }
            

    for(int i = 0; i < (int)cod_objeto.size(); i++) out_file << cod_objeto[i] << " ";

    out_file.close();
    new_input_file.close();

}
