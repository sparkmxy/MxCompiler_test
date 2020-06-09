#pragma once

#include "pch.h"
#include "RISCVinstruction.h"
#include "RISCVassembly.h"

class ASMCleanUpPass {
public:
	ASMCleanUpPass(std::shared_ptr<RISCVProgram> p) :P(p) {}

	void run();

private:
	std::shared_ptr<RISCVProgram> P;

	void omitUnused();

	std::map<std::shared_ptr<Register>, bool> isUsed;

};