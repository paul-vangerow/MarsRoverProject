#include "instructions.hpp"
#include "communication.hpp"

#include <iostream>

Mouvement::Mouvement(int _instr, val_t _val){
    instr = Instruction(_instr);
    val = _val;
}

int Mouvement::get_instruction() const{
    return instr;
}

val_t Mouvement::get_value() const{
            return val;
}

bool Mouvement::is_forward() const{
    return instr==1;
}

bool Mouvement::is_backwards() const{
    return instr==-1;
}

bool Mouvement::is_spin() const{
    return instr==0;
}

Instruction_queue::Instruction_queue(){
    std::queue<Mouvement> _instruction;
    instructions = _instruction;
}

void Instruction_queue::update(){
    String payload = FetchInstruction();
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << payload << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    if(payload=="[{\"instr\":0,\"val\":100}]"){
        Mouvement inst(Instruction(0),10);
        instructions.push(inst);
        std::cout << "------------------------------------------------------" << std::endl;
    }
    // while(payload != ""){
    //     if()
    // }
}
