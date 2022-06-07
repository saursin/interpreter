# interpreter
A header-only library to implement basic command line interpreters in cpp.
inspired from python Cmd module.

## Prerequisites
Install readline library

```bash
$ sudo apt-get install -y libreadline-dev
```

## Getting started
Lets make a basic interactive calculator program. Create a class that inherits from the Intrerpreter class.

```cpp
#include "interpreter.hpp"

class Calc: public Interpreter
{ };

```

### Adding commands
To add commands, simply define a static method that implements the functionality of the command. The method should take a Line object as argument and return an int. Name of the method can be anything.

```cpp
class Calc: public Interpreter {

    static int fn_add(Line l) {
        std::cout << " = " << stoi(l.tokens[1]) + stoi(l.tokens[1])
        << std::endl;
        return 0;
    }

};
```
Now in the class constructor, add command by constructing a command object and using the add_command method.

```cpp
class Calc: public Interpreter
{
...
public:
    Calc()
    {
        add_command(Command("add", &fn_add));
    }
}
```

### Adding Command Help
Adding command help is optional. define a static method that takes a Line object as parameter and returns a string object.
First line of the string must contain a one line description of the command. The lines must be separated by a `\n` character.

```cpp
    static std::string help_left(Line l) {
        return "move turtle left\n"
                "  left [degrees]";
    }

```cpp

Now add the help method pointer to the command

```cpp
class Calc: public Interpreter
{
...
public:
    Calc()
    {
        add_command(Command("add", &fn_add, &help_add));
    }
}
```
