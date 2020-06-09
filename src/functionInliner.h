#pragma once

#include "pch.h"
#include "IR.h"
#include "IRinstruction.h"

class FunctionInliner {
public:
	void run();
private:

	void prepare();
	void non_recursive();
	void recursive();
};