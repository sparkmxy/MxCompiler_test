#include "Optimizer.h"
#include "ir_printer.h"
#include <fstream>

void Optimizer::optimize()
{

	std::make_shared<SSAConstructor>(ir)->run();
	auto CEE = ConstantExpressionEvaluation(ir);
	auto cfgClearUp = CFGCleanUpPass(ir);
	bool changed;
	do
	{
		changed = false;
		changed |= CEE.run();
		changed |= cfgClearUp.run();
	} while (changed);
	
	//std::make_shared<IR_Printer>(ir, std::cerr)->print();
	std::make_shared<SSADestructor>(ir)->run();
	cfgClearUp.run();
}
