#include <iostream> 
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

int main(int argc, char **argv){
    int fname_index;
    fname_index = argc == 2 ? 1 : 2;
    string fname = argv[fname_index];

    int PC, n, memory, comand , last;
    float ACC;
    vector <int> program;

    ifstream arquivo(fname); // ABRE O ARQUIVO

    while (arquivo >> n) { // COLOCA O ARQUIVO EM UM ARRAY
        program.push_back(n);   
    }
    auto iterador = find(program.begin(), program.end(), 14); // Tentativa de encontrar o segmantation fault (não consegui ainda)
    
    arquivo.close();
    ACC=0;
    PC=0;
    string entrada;

    if (fname_index == 2) return 0;

    while (comand!=14){

        std::getline(std::cin, entrada);
        if (entrada.empty()) {
           
            cout << "PC <- " << PC << " ACC <- "<< ACC << std::endl;
            comand = program[PC];
            switch (comand)
            {
                case 1://ADD
                    ACC+= program[program[PC+1]];
                    PC+=2;
                    break;
                case 2://SUB
                    ACC-= program[program[PC+1]];
                    PC+=2;
                    break;           
                
                case 3://MULT
                    ACC= ACC*(program[program[PC+1]]);
                    PC+=2;
                    break;           

                case 4://DIV./montador <file.asm>
                    ACC= ACC/(program[program[PC+1]]);
                    PC+=2;
                    break;  

                case 5: //JMP
                    PC = program[PC+1];
                    break;
            
                case 6://JMPN
                    if(ACC<0){
                        PC = program[PC+1];
                    }
                    else{
                        PC = PC+2;
                    }
                    break;

                case 7: //JMPP
                    if(ACC>0){
                        PC = program[PC+1];
                    }
                    else{
                        PC = PC+2;
                    }
                    break;

                case 8: //JMPZ
                    if(ACC==0){
                        PC = program[PC+1];
                    }
                    else{
                        PC = PC+2;
                    }
                    break;

                case 9://COPY
                    program[program[PC+2]]=program[program[PC+1]];
                    PC = PC+3;
                    break;

                case 10: //LOAD
                    ACC = program[program[PC+1]];
                    PC = PC + 2;
                    break;

                case 11: //STORE
                    program[program[PC+1]] = ACC ;
                    PC = PC + 2;
                    break;
                
                case 12: //INPUT
                    cout << "Enter the value of the variable to be used in the program: ";
                    cin>>program[program[PC+1]];
                    PC = PC + 2;
                    break;

                case 13: //OUTPUT
                    cout << " SAÍDA: " << program[program[PC+1]]<< std::endl;
                    PC = PC + 2;
                    break;
            }
        }
        else {
            std::cout << "Invalid input. Press 'Enter' to execute the next line." << std::endl;
        }
    }
    return 0;
}

   

