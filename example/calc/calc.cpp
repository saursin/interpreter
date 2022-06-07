#include "interpreter.hpp"

class Calc: public Interpreter
{

    static int fn_add(Line l) {
        std::cout << " = " << stoi(l.tokens[1]) + stoi(l.tokens[2]) << std::endl;        
        return 0;
    }

    static std::string help_add(Line l) {
        return "move turtle left\n"
               "  left [degrees]";
    }


    static int fn_sub(Line l) {
        std::cout << " = " << stoi(l.tokens[1]) - stoi(l.tokens[2]) << std::endl;        
        return 0;
    }

    static std::string help_sub(Line l) {
        return "move turtle left\n"
               "  left [degrees]";
    }


public:
    Calc()
    {
        add_command(Command("add", &fn_add, &help_add));
        add_command(Command("sub", &fn_sub, &help_sub));
    }
};


int main()
{
    Calc c;
    c.cmdloop();
}