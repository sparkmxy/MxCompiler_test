#include "pch.h"
#include "MxCompiler.h"

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
	
	return 0;
}