#ifndef instruction_hpp
#define isntruction_hpp

#include <string.h>
#include <queue>

typedef double val_t;

enum Instruction{
    forward = 1,
    spin = 0,
    backwards = -1
};

class Mouvement{
    private:
        Instruction instr;
        val_t val;
    
    public:
        Mouvement();
        Mouvement(int _instr, val_t _val);

        ~Mouvement();

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
        ~Instruction_queue();

        bool isEmpty(){
            return instructions.empty();
        }

        Mouvement get_instruction(){
            return instructions.pop();
        }

        virtual void update();
};

#endif