#pragma once

#include <string>
#include <vector>
#include <iostream>

// sudo apt-get install -y libreadline-dev
#include <readline/readline.h>
#include <readline/history.h>


#define ANSI_ERASELINE "\33[2K\r"
#define ANSI_CLEARSCREEN "\033[2J"

const std::string WHITESPACE = " \n\r\t\f\v";

struct Line
{
    std::string text;
    std::vector<std::string> tokens;

    static std::string lStrip(const std::string& s)
    {
        size_t start = s.find_first_not_of(WHITESPACE);
        return (start == std::string::npos) ? "" : s.substr(start);
    }

    static std::string rStrip(const std::string& s)
    {
        size_t end = s.find_last_not_of(WHITESPACE);
        return (end == std::string::npos) ? "" : s.substr(0, end + 1);
    }

    static std::string strip(const std::string& s)
    {
        return rStrip(lStrip(s));
    }

    static Line parse_line(std::string line, std::string comment_pattern="#")
    {
        // remove comment
        size_t found = line.find(comment_pattern);
        if (found != std::string::npos)
            line = line.substr(0, found);

        // strip
        line = strip(line);

        Line l;
        l.text = line;

        if (line.length() == 0)
            return l;

        // tokenize
        char buffer[line.length()+1];
        strcpy(buffer, line.c_str());

        char delims[] = " \t\r\n";
        
        char *p = strtok(buffer, delims);
        while (p) {
            l.tokens.push_back(p);
            p = strtok(NULL, delims);
        }
    	return l;
    }

    void parse()
    {
        tokens = parse_line(text).tokens;
    }

    void print()
    {
        std::cout << "[";
        for (int i=0; i<tokens.size(); i++)
        {
            std::cout << tokens[i];
            if (i!=tokens.size()-1)
                std::cout << ", ";
        }
        std::cout << "]\n";
    }
};


typedef int (*Func_callback)(Line s);
typedef std::string (*Func_help)(Line s);
typedef std::vector<std::string> (*Func_complete)(Line s);


int fn_clear(Line l)
{
    std::cout << ANSI_CLEARSCREEN;
    return 0;
}

std::string help_clear(Line l)
{
    return "Clear screen\n";
}

struct Command
{
    std::string name;
    std::string alias;
    Func_callback fn_ptr_callback;
    Func_help fn_ptr_help;
    Func_complete fn_ptr_complete;
    
    Command(std:: string name, Func_callback fn_ptr_callback, Func_help fn_ptr_help=NULL, Func_complete fn_ptr_complete=NULL):
        name(name),
        fn_ptr_callback(fn_ptr_callback), 
        fn_ptr_help(fn_ptr_help),
        fn_ptr_complete(fn_ptr_complete)
    {}

    Command(const char name[], Func_callback fn_ptr_callback, Func_help fn_ptr_help=NULL, Func_complete fn_ptr_complete=NULL):
        name(name),
        fn_ptr_callback(fn_ptr_callback), 
        fn_ptr_help(fn_ptr_help),
        fn_ptr_complete(fn_ptr_complete)
    {}
};


class Interpreter
{
protected:
    std::string prompt = "> ";
    std::string intro = "** Interpreter **";
    std::string comment_sym = "#";
    bool enable_filename_completion = false;
    std::vector<Command> commands;
    std::vector<std::string> history;
    bool exit = false;

public:
    //////////////////////////////////////////////
    // init
    Interpreter()
    {
        add_command(Command("clear", &fn_clear, &help_clear));
    }

    Command * find_cmd(std::string c)
    {
        Command * x = nullptr;
        for(unsigned i=0; i<commands.size(); i++)
        {
            if(commands[i].name == c)
            {
                x = &commands[i];
                break;
            }
        }
        return x;
    }

    void add_command(Command cmd)
    {
        Command * cmd_ptr = this->find_cmd(cmd.name);
        if(cmd_ptr)
        {
            std::cout << "InitError: can't add command \"" << cmd.name << "\"; command already exists" << std::endl;
            return;
        }
        commands.push_back(cmd);
    }

    int help_command(Line line)
    {
        if(line.tokens.size()==1)
        {
            // print command help
            printf(" Alias Command               Description\n");
            printf("--------------------------------------------------------\n");
            printf("  %-3s  %-20s  %s\n", "?", "help", "Show command help");
            printf("  %-3s  %-20s  %s\n", "q", "quit", "Quit");
            for (auto c = commands.begin(); c!=commands.end(); c++)
            {
                std::string command_help = "---";

                if( c->fn_ptr_help)
                {
                    command_help = c->fn_ptr_help(line);

                    // crop till first newline
                    size_t found = command_help.find("\n");
                    if (found != std::string::npos)
                        command_help = command_help.substr(0, found);
                }
                printf("  %-3s  %-20s  %s\n", c->alias.c_str(), c->name.c_str(), command_help.c_str());
            }
        }
        else 
        {
            // print command specific help
            if(line.tokens[1] == "help")
                std::cout << "  Print help for specified command\n\t help [command]" << std::endl;
            else
            {
                Command * cmd = find_cmd(line.tokens[1]);
                if(cmd)
                {
                    if(cmd->fn_ptr_help)
                        std::cout << cmd->fn_ptr_help(line) << std::endl;
                    else
                        std::cout << "Help not available for command: \"" << line.tokens[1] <<"\""<< std::endl;
                }
                else
                    std::cout << "Help not available for command \"" << line.tokens[1] << "\"; command doesn't exist" << std::endl;
            }
        }
        return 0;
    }

    ///////////////////////////////////////////////
    // Runtime

    virtual void pre_loop()
    {}

    virtual void post_loop()
    {}

    virtual std::string pre_cmd(std::string s)
    {
        return s;
    }

    virtual int post_cmd(int rval, Line s)
    {
        return rval;
    }

   
    int execute_cmd(Line line)
    {
        if (line.text.length() == 0)
            return 0;

        // check if quit command
        if (line.tokens[0] == "q" || line.tokens[0] == "quit")
            return 1;

        // check if help command
        if (line.tokens[0] == "?" || line.tokens[0] == "help")
            return help_command(line);

        // find command
        Command * cmd = NULL;       
        for(auto x = commands.begin(); x!= commands.end(); x++)
        {
            if(x->name == line.tokens[0])
            {
                cmd = &(*x);
                break;
            }
        }

        if(cmd == NULL)
        {
            std::cout << "command not found: "<< line.tokens[0] << std::endl;
            return 0;
        }

        bool rv;
        try
        {
            rv = cmd->fn_ptr_callback(line);
        }
        catch(const std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << '\n';
        }
        return rv;
    }

public:
    void cmdloop()
    {
        // Print intro
        std::cout << intro << std::endl;

        // Preloop
        pre_loop();

        // setup readline library
        if (!enable_filename_completion) {
            // By default readline does filename completion. With -d, we disable this
            // by asking readline to just insert the TAB character itself.
            rl_bind_key('\t', rl_insert);
        }

        static std::string lastcmd;
        int cmd_ret_val = 0;
        char* buf;
        while ((buf = readline(prompt.c_str())) != nullptr) {
            std::string inp = buf;
            free(buf);  // clear malloc'ated buffer

            if (inp.length() > 0)
            {
                // dont add in history if same as last command
                if (inp != lastcmd)
                    add_history(inp.c_str());
            }
            lastcmd = inp;              

            // parse & execute command
            inp = pre_cmd(inp);
            Line line = Line::parse_line(inp, comment_sym);
            cmd_ret_val = execute_cmd(line);
            cmd_ret_val = post_cmd(cmd_ret_val, line);

            // check return value
            if (cmd_ret_val > 0)
                break;
        }

        post_loop();
    }
};






