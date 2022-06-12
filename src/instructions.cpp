#include "instructions.hpp"
#include "communication.hpp"

#include <iostream>
#include <cstring>

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

    parse(instructions, payload);

    Serial.println(instructions.front().get_instruction());
    Serial.println(instructions.front().get_value());
    instructions.pop();

    // std::cout << "------------------------------------------------------" << std::endl;
    // // Serial.println(payload);
    // std::cout << "------------------------------------------------------" << std::endl;
    // if(payload=="[{\"instr\":0,\"val\":100}]"){
    //     Mouvement inst(Instruction(0),10);
    //     instructions.push(inst);
    //     std::cout << "------------------------------------------------------" << std::endl;
    // }
    // // while(payload != ""){
    // //     if()
    // // }
}

int count(std::string msg, std::string charr){
    int i = 0;
    int found = msg.find(charr);
    while(found!=std::string::npos){
        found = msg.find(charr, found + 1);
        i++;
    }
    return i;
}

void parse(std::queue<Mouvement> &q, String message){
    if((message[0]=='[') && (message.charAt(message.length()-1)==']')){
        message = message.substring(1,message.length()-1);
        parse(q, message);
        return;
    }
    if(message==""){
        return;
    }
    std::string msg = message.c_str();

    if(count(msg, "{")>=2){
        // seperate the string into substrings of s single object
        String message1, message2;
        message1 = String(msg.substr(msg.find_first_of('{'), msg.find_first_of('}')+1).c_str());
        message2 = String(msg.substr(msg.find_first_of('}')+1,std::string::npos).c_str());
        parse(q, message1);
        parse(q, message2);
    }
    else if (count(msg, "{")==1){
        int instr_index_start = msg.find("instr") + 7;
        int instr_index_end = msg.substr(instr_index_start, std::string::npos).find_first_of('"')-1;
        int val_index_start = msg.find("val") + 5;
        int val_index_end = msg.substr(val_index_start, std::string::npos).find_first_of('}');
        Mouvement new_mouv(std::stoi(msg.substr(instr_index_start, instr_index_end)), std::stod(msg.substr(val_index_start, val_index_end)));
        q.push(new_mouv);
        return;
    }
    else{
        return;
    }
}
