#include "globalvarResolver.h"

void GlobalVarResolver::run()
{
	prepare();

	loadWhenEntering();
	storeWhenExiting();
	resolveCallInstr();
}

void GlobalVarResolver::prepare()
{
	computeRecursiveCalleeSet();

	for (auto &f : ir->getFunctions()) {
		for (auto &b : f->getBlockList()) {
			for (auto i = b->getFront(); i != nullptr; i = i->getNextInstr()) {
				auto uses = i->getUseRegs();
				for (auto reg : uses)
					if (reg->isGlobal() && Operand::isRegister(reg->category()))
						i->replaceUseReg(reg, getTempReg(std::static_pointer_cast<VirtualReg>(reg), f));

				auto def = i->getDefReg();
				if (def != nullptr && def->isGlobal() && Operand::isRegister(def->category())) {
					i->setDefReg(getTempReg(std::static_pointer_cast<VirtualReg>(def), f));
					varDef[f].insert(std::static_pointer_cast<VirtualReg>(def));
				}
			}
		}

		for (auto it : func2varMap[f]) varUsedRecursively[f].insert(it.first);
		for (auto var : varDef[f]) varDefRecursively[f].insert(var);
		for (auto callee : recursiveCalleeSet[f]) {
			for (auto it : func2varMap[callee]) varUsedRecursively[f].insert(it.first);
			for (auto var : varDef[f]) varDefRecursively[f].insert(var);
		}
	}
}

void GlobalVarResolver::loadWhenEntering()
{
	for (auto &f : ir->getFunctions())
			for (auto it : func2varMap[f])
				f->getEntry()->append_front(std::make_shared<Quadruple>(
					f->getEntry(), Quadruple::LOAD, it.second, it.first));
}

void GlobalVarResolver::storeWhenExiting()
{
	for(auto &f : ir->getFunctions())
		if (f->getName() != "__bootstrap") {
			auto ret = f->getReturnIntrs()[0];
			for (auto var : varDef[f])
				appendInstrBefore(ret, std::make_shared<Quadruple>(
					ret->getBlock(), Quadruple::STORE, var, getTempReg(var, f)));
		}
}

void GlobalVarResolver::resolveCallInstr()
{
	for (auto &f : ir->getFunctions()) {
		std::set<std::shared_ptr<VirtualReg> > varUsed;
		for (auto it : func2varMap[f]) varUsed.insert(it.first);

		if (!varUsed.empty()) {
			for(auto &b : f->getBlockList())
				for (auto i = b->getFront(); i != nullptr; i = i->getNextInstr()) {
					if (i->getTag() != IRInstruction::CALL) continue;

					auto callee = std::static_pointer_cast<Call>(i)->getFunction();

					for (auto def : varDef[f])   
						if (varUsedRecursively[callee].find(def) != varUsedRecursively[callee].end())  // store before call
							appendInstrBefore(i, std::make_shared<Quadruple>(
								b, Quadruple::STORE, def, getTempReg(def, f)));

					if (f->getName() != "__bootstrap" && !varDefRecursively[callee].empty()) {  // reload after call
						for (auto var : varDefRecursively[callee])
							if (varUsed.find(var) == varUsed.end())
								appendInstrAfter(i, std::make_shared<Quadruple>(
									b, Quadruple::LOAD, getTempReg(var, f), var));
					}

				}
		}
	}
}

void GlobalVarResolver::computeRecursiveCalleeSet()
{
	for (auto &f : ir->getFunctions())
		for (auto &b : f->getBlockList())
			for (auto i = b->getFront(); i != nullptr; i = i->getNextInstr())
				if (i->getTag() == IRInstruction::CALL)
					calleeSet[f].insert(std::static_pointer_cast<Call>(i)->getFunction());

	bool changed;
	do
	{
		changed = false;
		for (auto &f : ir->getFunctions()) {
			auto new_calleeSet = calleeSet[f];
			for (auto callee : calleeSet[f])
				for (auto ff : recursiveCalleeSet[callee]) new_calleeSet.insert(ff);
			
			if (new_calleeSet != recursiveCalleeSet[f]) {
				recursiveCalleeSet[f] = new_calleeSet;
				changed = true;
			}
		}
	} while (changed);
}

std::shared_ptr<VirtualReg> GlobalVarResolver::getTempReg(std::shared_ptr<VirtualReg> reg, std::shared_ptr<Function> f)
{
	auto it = func2varMap[f].find(reg);
	if (it == func2varMap[f].end())   // create a new one
		func2varMap[f][reg] = std::make_shared<VirtualReg>();
	return func2varMap[f][reg];
}
