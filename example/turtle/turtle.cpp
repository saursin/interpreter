#include "interpreter.hpp"

class Turtle: public Interpreter
{
public:  
    static int fn_forward(Line l) {
        std::cout << "moving turtle forward ▲" << std::endl;
        return 0;
    }

    static std::string help_forward(Line l) {
        return "Move turtle forward\n"
               "  forward [steps]";
    }


    static int fn_backward(Line l) {
        std::cout << "moving turtle backward ▼" << std::endl;
        return 0;
    }

    static std::string help_backward(Line l) {
        return "move turtle backward"
               "  backward [steps]";
    }


    static int fn_left(Line l) {
        std::cout << "moving turtle left ◄" << std::endl;
        return 0;
    }

    static std::string help_left(Line l) {
        return "move turtle left\n"
               "  left [degrees]";
    }


    static int fn_right(Line l) {
        std::cout << "moving turtle right ►" << std::endl;
        return 0;
    }

    static std::string help_right(Line l) {
        return "move turtle right\n"
               "  right [degrees]";
    }


    static int fn_demo(Line l) {
        std::cout << "***** Demo function called *****" << std::endl;
        return 0;
    }

    static std::string help_demo(Line l) {
        return "One line destiption of demo function\n" 
               "  demo [arg1] [arg2]\n"
               "   arg1  arg1 description\n";
               "   arg2  arg2 description\n";
    }

    Turtle()
    {
        prompt = "turtle> ";
        intro = "******* Turtle Interpreter *******";
        add_command(Command("forward", &Turtle::fn_forward, &help_forward));
        add_command(Command("backward", &fn_backward, &help_backward));
        add_command(Command("left", &fn_left, &help_left));
        add_command(Command("right", &fn_right, &help_right));
        add_command(Command("demo", &fn_demo, &help_demo));
    }
};

int main()
{
  
    Turtle t;
    t.cmdloop();
}