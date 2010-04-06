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
#include "ArchitectureVirtual.h"
#include <llvm/Instructions.h>
#include <math.h>

/*
	"Virtual" target architecture
*/

using namespace llvm;

unsigned int ArchitectureVirtual::getSwInstructionTiming(const llvm::Instruction* inst) const
{
	if (isa<BinaryOperator>(inst))
	{
		const BinaryOperator *binop = cast<BinaryOperator>(inst);
		bool floatingPoint = false;
		for (User::const_op_iterator oi = inst->op_begin(), oe = inst->op_end();
			oi != oe; ++oi)
		{
			if (oi->get()->getType()->isFloatingPoint())
			{
				floatingPoint = true;
				break;
			}
		}
		int opcode = binop->getOpcode();
		if (!floatingPoint)
		{
			if (opcode == Instruction::UDiv || opcode == Instruction::URem ||
				opcode == Instruction::SDiv || opcode == Instruction::SRem)
				return 15;
			else if (opcode == Instruction::Mul)
				return 5;
			else
				return 1;
		}
		else
		{
			switch (opcode)
			{
			case Instruction::Add:
			case Instruction::Sub:
			case Instruction::FCmp:
				return 50;
			case Instruction::Mul:
				return 100;
			case Instruction::FPToUI:
			case Instruction::FPToSI:
			case Instruction::SIToFP:
			case Instruction::UIToFP:
			case Instruction::FPExt:
			case Instruction::FPTrunc:
				return 40;
			case Instruction::FDiv:
			case Instruction::FRem:
				return 1500;
			}
		}
	}
	else if (isa<StoreInst>(inst) || isa<LoadInst>(inst))
		return 2;
	else if (isa<TerminatorInst>(inst))
		return 3;
	else if (isa<BitCastInst>(inst) || isa<PtrToIntInst>(inst) || 
		isa<IntToPtrInst>(inst))
		return 0;
	return 1;
}

/*
	1 HW unit = 0.1 SW units
*/
unsigned int ArchitectureVirtual::getHwInstructionTiming(const llvm::Instruction* inst) const
{
	// casts are 'free'
	if (isa<BitCastInst>(inst) || isa<TruncInst>(inst) ||
		isa<ZExtInst>(inst) || isa<SExtInst>(inst) ||
		isa<PtrToIntInst>(inst) || isa<IntToPtrInst>(inst))
		return 0;
	if (isa<BinaryOperator>(inst))
	{
		const BinaryOperator *binop = cast<BinaryOperator>(inst);
		int opcode = binop->getOpcode();
		bool constantOperand = false, floatingPoint = false;
		for (User::const_op_iterator oi = inst->op_begin(), oe = inst->op_end();
			oi != oe; ++oi)
		{
			if (isa<Constant>(oi->get()))
				constantOperand = true;
			if (oi->get()->getType()->isFloatingPoint())
				floatingPoint = true;
		}
		switch (opcode)
		{
		// arithmetic operations
		case Instruction::Add:
		case Instruction::Sub:
			return floatingPoint ? 100 : (constantOperand ?  6 : 8);
		case Instruction::UDiv:
		case Instruction::SDiv:
			return constantOperand ? 45 : 75;
		case Instruction::URem:
		case Instruction::SRem:
			return constantOperand ? 100: 125;
		case Instruction::Mul:
			return floatingPoint ? 175 : (constantOperand  ? 35 : 45);
		// logical operations
		case Instruction::Xor:
		case Instruction::And:
		case Instruction::Or:
			return constantOperand ? 6 : 8;
		// shift instructions
		case Instruction::Shl:
		case Instruction::LShr:
		case Instruction::AShr:
			return constantOperand ? 7 : 9;
		// floating point operations
		case Instruction::FPToUI:
		case Instruction::FPToSI:
		case Instruction::SIToFP:
		case Instruction::UIToFP:
		case Instruction::FPExt:
		case Instruction::FPTrunc:
			return 75;
		case Instruction::FCmp:
			return 100;
		case Instruction::FDiv:
		case Instruction::FRem:
			return 400;
		}
	}
	else if (isa<CmpInst>(inst))
	{
		bool constantOperand = false;
		for (User::const_op_iterator oi = inst->op_begin(), oe = inst->op_end();
			oi != oe; ++oi)
		{
			if (isa<Constant>(oi->get()))
			{
				constantOperand = true;
				break;
			}
		}
		if (cast<CmpInst>(inst)->getPredicate() > 31)
			return (constantOperand ? 6 : 8);	// integer comparison
		else
			return 100;
	}
	return 10;
}

unsigned int ArchitectureVirtual::convertHwToSwTiming(unsigned int hwTiming) const
{
	return static_cast<unsigned int>(ceil(hwTiming / 10.0));
}

/*
	returns expected overhead for one execution in SW units
*/
unsigned int ArchitectureVirtual::getExecutionOverhead(unsigned int nInputs,
													   unsigned int nOutputs) const
{
	return static_cast<unsigned int>(ceil(nInputs / 4.0) + ceil(nOutputs / 2.0));
}

bool ArchitectureVirtual::isValidInstruction(const llvm::Instruction *inst) const
{
	if (isa<StoreInst>(inst) || isa<LoadInst>(inst) ||
		isa<TerminatorInst>(inst) || isa<AllocationInst>(inst) ||
		isa<CallInst>(inst) || isa<GetElementPtrInst>(inst) || isa<PHINode>(inst))
		return false;
	else
		return true;
}


unsigned int ArchitectureVirtual::getMaxInputs() const
{
	return 8;
}

unsigned int ArchitectureVirtual::getMaxOutputs() const
{
	return 2;
}

unsigned int ArchitectureVirtual::getMaxCustomInstructions() const
{
	return 16;
}
