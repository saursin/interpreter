#include "inter.hpp"

class Turtle: public Interpreter
{
public:  
    static int fn_forward(std::string s) {
        std::cout << "called function: forward" << std::endl;
        return 0;
    }

    static int help_forward(std::string s) {
        std::cout << "Help for funtion: forward" << std::endl;
        return 0;
    }


    static int fn_backward(std::string s) {
        std::cout << "called function: backward" << std::endl;
        return 0;
    }

    static int help_backward(std::string s) {
        std::cout << "Help for funtion: backward" << std::endl;
        return 0;
    }


    static int fn_left(std::string s) {
        std::cout << "called function: left" << std::endl;
        return 0;
    }

    static int help_left(std::string s) {
        std::cout << "Help for funtion: left" << std::endl;
        return 0;
    }


    static int fn_right(std::string s) {
        std::cout << "called function: right" << std::endl;
        return 0;
    }

    static int help_right(std::string s) {
        std::cout << "Help for funtion: right" << std::endl;
        return 0;
    }


    Turtle()
    {
        add_command(Command("forward", &fn_forward, &help_forward));
        add_command(Command("backward", &fn_backward, &help_backward));
        add_command(Command("left", &fn_left, &help_left));
        add_command(Command("right", &fn_right, &help_right));
    }
};

int main()
{
  
    Turtle t;
    t.cmdloop();
}