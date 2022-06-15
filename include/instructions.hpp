#ifndef instruction_hpp
#define instruction_hpp

typedef double val_t;


enum Instruction{
    forward = 0,
    spinCCW = 1,
    spinCW = -1
};

enum Orientation{
    up,
    down,
    left,
    right
};

enum Colour{
    red,
    pink,
    teal,
    yellow,
    blue,
    green
};

#include <Arduino.h>
#include <string.h>
#include <queue>


#include <communication.hpp>

class Mouvement{
    private:
        Instruction instr;
        val_t val;
    
    public:
        Mouvement();
        Mouvement(int _instr, val_t _val);

        ~Mouvement(){};

        virtual int get_instruction() const;
        virtual val_t get_value() const;
        virtual bool is_forward() const;
        virtual bool is_backwards() const;
        virtual bool is_spin() const;

};

class Instruction_queue{
    private:
        std::queue<Mouvement> instructions;
    
    public:

        Instruction_queue();
        ~Instruction_queue(){};

        bool isEmpty(){
            return instructions.empty();
        }

        Mouvement get_instruction(){
            Mouvement temp = instructions.front();
            instructions.pop();
            return temp;
        }

        virtual void update();
};

// returns the number of substrings in the string
int count(std::string msg);

// updates the queue according to the json formated string
void parse(std::queue<Mouvement> &q, String message);

#endif