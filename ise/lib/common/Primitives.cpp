/*
Copyright (c) 2009, Martin Tofall
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The name of the author may not be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "Primitives.h"
#include <llvm/BasicBlock.h>

using namespace llvm;

/*
	the instruction's result is considered an output if it's
		- a memory write
		- used outside its basic block
		- used in the block's terminator instruction (e.g. a return statement)
*/
bool Primitives::isOutputForBasicBlock(const Instruction *inst)
{
	// memory write?
	if (isa<StoreInst>(inst))
		return true;
	// used outside of its basic block?
	const BasicBlock *bb = inst->getParent();
	if (isUsedOutsideOfBlock(inst, bb))
		return true;
	// check terminator instruction
	const TerminatorInst *ti = bb->getTerminator();
	if (ti != NULL)
	{
		if (isa<InvokeInst>(ti) || isa<ReturnInst>(ti) || isa<SwitchInst>(ti) ||
			isa<BranchInst>(ti))
		{
			for (User::const_op_iterator oi = ti->op_begin(), oe = ti->op_end(); oi != oe; ++oi)
			{
				const Value* v = oi->get();
				if (isa<Instruction>(v) && v == inst)
					return true;
			}
		}
	}
	return false;
}

/*
	like Instruction::isUsedOutsideOfBlock except for PHI node handling
*/
bool Primitives::isUsedOutsideOfBlock(const Instruction *inst, const BasicBlock *BB)
{
	for (Value::use_const_iterator UI = inst->use_begin(); UI != inst->use_end(); ++UI)
	{
		if (cast<Instruction>(*UI)->getParent() != BB)
			return true;
	}
	return false;
}
