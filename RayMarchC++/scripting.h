#include <LuaContext.hpp>
#include <boost/array.hpp>
#include "constants.h"
#include <iostream>

struct ScriptData {
    boost::array<char, SCRIPT_SIZE> script = boost::array<char, SCRIPT_SIZE>();
    bool recompile;
    std::string name;
    std::function<float(float)> eval;

    ScriptData(std::string name) {
        std::string default_val = "evaluate = function(t)\n  return t\nend";
        for (int i = 0; i < default_val.size(); i++) {
            script[i] = default_val[i];
        }
        this->name = name;
    }

    void rename(std::string name) {

    }

    void compile() {
        recompile = true;
    }

    float evaluate(float t) {
        try {
            //auto start = std::chrono::high_resolution_clock::now();
            //float result = eval(t);
            //auto finish = std::chrono::high_resolution_clock::now();

            //std::cout << result << std::endl;
            //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << "ns\n";
            return eval(t);
        }
        catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
};

void Recompile(LuaContext& lua, ScriptData& c);
void DeleteLua();
void SetupLua();