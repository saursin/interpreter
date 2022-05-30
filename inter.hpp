#pragma once

#include <string>
#include <vector>
#include <iostream>

// sudo apt-get install -y libreadline-dev
#include <readline/readline.h>
#include <readline/history.h>

#define ANSI_ERASELINE "\33[2K\r"
const std::string WHITESPACE = " \n\r\t\f\v";


typedef int (*Func_callback)(std::string s);
typedef int (*Func_help)(std::string s);
typedef std::vector<std::string> (*Func_complete)(std::string s);

struct Command
{
    std::string name;
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


struct Line
{
    std::string line;
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
        l.line = line;

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
    
    //////////////////////////////////////////////
    // init
    static int fn_help(std::string s)
    {
        Command * cmd = NULL;       
        for(auto x = commands.begin(); x!= commands.end(); x++)
        {
            if(x->name == line.tokens[0])
            {
                cmd = &(*x);
                break;
            }
        }
        return 0;
    }

    static int help_help(std::string s)
    {
        std::cout <<  "print available commands" << std::endl;
        return 0;
    }

    Interpreter()
    {
        add_command(Command("help", &fn_help, &help_help));
    }

    void add_command(Command cmd)
    {
        commands.push_back(cmd);
    }



    ///////////////////////////////////////////////
    // Runtime

    // static char* completion_generator(const char* text, int state) {
    // // This function is called with state=0 the first time; subsequent calls are
    // // with a nonzero state. state=0 can be used to perform one-time
    // // initialization for this completion session.
    // static std::vector<std::string> matches;
    // static size_t match_index = 0;

    // if (state == 0) {
    //     // During initialization, compute the actual matches for 'text' and keep
    //     // them in a static vector.
    //     matches.clear();
    //     match_index = 0;

    //     // Collect a vector of matches: vocabulary words that begin with text.
    //     std::string textstr = std::string(text);
    //     for (auto word : vocabulary) {
    //     if (word.size() >= textstr.size() &&
    //         word.compare(0, textstr.size(), textstr) == 0) {
    //         matches.push_back(word);
    //     }
    //     }
    // }

    // if (match_index >= matches.size()) {
    //     // We return nullptr to notify the caller no more matches are available.
    //     return nullptr;
    // } else {
    //     // Return a malloc'd char* for the match. The caller frees it.
    //     return strdup(matches[match_index++].c_str());
    // }
    // }

    // static char** completer(const char* text, int start, int end) {
    //     // Don't do filename completion even if our generator finds no matches.
    //     rl_attempted_completion_over = 1;

    //     // Note: returning nullptr here will make readline use the default filename
    //     // completer.
    //     return rl_completion_matches(text, completion_generator);
    // }

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
        if (line.line.length() == 0)
            return 0;

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
            rv = cmd->fn_ptr_callback(line.line);
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
            line.print();
            cmd_ret_val = execute_cmd(line);
            cmd_ret_val = post_cmd(cmd_ret_val, line);

            // check return value
            if (cmd_ret_val > 1)
                break;
        }

        post_loop();
    }
};






