#include "asmCleanUp.h"

void ASMCleanUpPass::run()
{
	omitUnused();
}

void ASMCleanUpPass::omitUnused()
{
	for (auto &f : P->getFunctions()) {
		isUsed.clear();
		for (auto &b : f->getBlockList())
			for (auto i = b->getFront(); i != nullptr; i = i->getNextInstr())
				for (auto &r : i->getUseReg()) isUsed[r] = true;

		for (auto &b : f->getBlockList()) {
			std::vector<std::shared_ptr<RISCVinstruction> > I;
			for (auto i = b->getFront(); i != nullptr; i = i->getNextInstr()) I.push_back(i);
			for (auto i : I) {
				auto c = i->category();
				if (c != RISCVinstruction::RTYPE  && c != RISCVinstruction::ITYPE) continue;
				if (i->getDefReg() != nullptr && !isUsed[i->getDefReg()])
					removeRISCVinstruction(i);
			}
		}
	}
}
