#include "scripting.h"
LuaContext* lua;

void Recompile(ScriptData& c) {
	try {
		std::string data(c.script.begin(), c.script.end());
		lua->executeCode(data);
		auto eval = lua->readVariable<std::function<float(float)>>("evaluate");
		float result = eval(2.0);
		result = eval(2.2);
		result = eval(0.0);
		c.eval = eval;
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}
	c.recompile = false;
}

void SetupLua() {
	LuaContext l;
	lua = &l;
}

void DeleteLua() {
	try {
		lua->~LuaContext();
	}
	catch (std::exception e) {

	}
}

LuaContext* GetLua() {
	return lua;
}