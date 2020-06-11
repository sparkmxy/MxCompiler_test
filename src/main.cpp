#include "pch.h"
#include "MxCompiler.h"
#include <fstream>

int main(int argc, char **argv) {
	MxCompiler compiler(argv[1]);
	for (int i = 0; i < argc; ++i) 
    	if (std::string(argv[i]) == "--semantic"){
			try
			{
				compiler.semantic();
			}
			catch(Error& e)
			{
				std::cerr << e.what() << '\n';
				exit(-1);
			}
			return 0;
		}
		else if(std::string(argv[i]) == "--codegen"){
			try
			{
				compiler.compile(false);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
			
		}
		else if(std::string(argv[i]) == "--opt"){
			try
			{
				compiler.compile(true);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
			
		}
		
	return 0;
}