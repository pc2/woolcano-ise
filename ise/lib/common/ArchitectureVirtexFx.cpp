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
/*
	PPC405
	"Assuming cache hits, all instructions execute in one cycle except the following:
	- Divide instructions execute in 35 clock cycles.
	- Branches execute in one to three clock cycles as described in Branches below.
	- Multiply-accumulate and multiply instructions execute in one to five cycles as 
	described in Multiplies below.
	- Aligned load/store instructions that hit in the data cache execute in one clock 
	cycle. See Alignment above for information on the access penalty associated with 
	unaligned load/stores." -PowerPC Processor Reference Guide UG011 (v1.2) January 19, 2007

	
	This code may break on LLVM 2.6 due to the mul add and sub opcodes having been split
*/

#include "ArchitectureVirtexFx.h"
#include <llvm/Instructions.h>
#include <math.h>

using namespace llvm;
ArchitectureVirtexFx::ArchitectureVirtexFx(bool CommDisableOverhead,     
                                           unsigned int CommClkPerInput,
                                           float CommInputBusWidth,
                                           unsigned int CommClkPerOutput,
                                           float CommOutputBusWidth,
                                           unsigned int MaxUDI,
                                           unsigned int MaxInput, 
                                           unsigned int MaxOutput)
{
    CommDisableOverhead_ = CommDisableOverhead;
    CommClkPerInput_ = CommClkPerInput;
    CommInputBusWidth_ = CommInputBusWidth;
    CommClkPerOutput_ =  CommClkPerOutput;
    CommOutputBusWidth_ = CommOutputBusWidth;
    MaxUDI_ =  MaxUDI;
    MaxInput_ =  MaxInput;
    MaxOutput_ = MaxOutput;
    
    std::cout << "Selected architecture: " <<  typeid(this).name() << 
    "\n * CommDisableOverhead: " << CommDisableOverhead_ << 
    "\n * CommClkPerInput: " << CommClkPerInput_ << 
    "\n * CommInputBusWidth: " << CommInputBusWidth_ << 
    "\n * CommClkPerOutput: " << CommClkPerOutput_ <<
    "\n * CommOutputBusWidth: " << CommOutputBusWidth_ <<
    "\n * MaxUDI: " << MaxUDI_ <<
    "\n * MaxInput: " << MaxInput_ <<
    "\n * MaxOutput: " << MaxOutput_ << "\n";
}


/*
	SW units: clock cycles
*/
unsigned int ArchitectureVirtexFx::getSwInstructionTiming(const llvm::Instruction* inst) const
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
				return 35;
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
				return 61;
			case Instruction::Sub:
				return 55;
			case Instruction::Mul:
				return 102;
			case Instruction::FPToUI:
			case Instruction::FPToSI:
				return 22;
			case Instruction::SIToFP:
			case Instruction::UIToFP:
				return 34;
			case Instruction::FCmp:
				return 58;
			case Instruction::FPExt:
			case Instruction::FPTrunc:
				return 40;		// guess
			case Instruction::FDiv:
			case Instruction::FRem:
				return 1581;
			}
		}
	}
	else if (isa<StoreInst>(inst) || isa<LoadInst>(inst))
		return 2;	// very optimistic...
	else if (isa<TerminatorInst>(inst))
		return 3;
	else if (isa<BitCastInst>(inst) || isa<PtrToIntInst>(inst) || 
		isa<IntToPtrInst>(inst))
		return 0;
	return 1;		// very optimistic as well...
}

/*
	HW units: 1/10th of a nanosecond
*/
unsigned int ArchitectureVirtexFx::getHwInstructionTiming(const llvm::Instruction* inst) const
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
			return floatingPoint ? 75 : (constantOperand ? 58 : 64);
		case Instruction::Sub:
			return floatingPoint ? 75 : (constantOperand ? 56 : 64);
		case Instruction::UDiv:
		case Instruction::SDiv:
			return constantOperand ? 115 : 350;	// 350 = guess
		case Instruction::URem:
		case Instruction::SRem:
			return constantOperand ? 300: 350;	// 300/350 = guess
		case Instruction::Mul:
			return floatingPoint ? 125 : 115;
		// logical operations
		case Instruction::Xor:
			return constantOperand ? 19 : 34;
		case Instruction::And:
			return constantOperand ? 33 : 33;	// TODO?
		case Instruction::Or:
			return constantOperand ? 37 : 37;	// TODO?
		// shift instructions
		case Instruction::Shl:
			return constantOperand ? 43 : 79;
		case Instruction::LShr:
		case Instruction::AShr:
			return constantOperand ? 44 : 94;
		// floating point operations
		case Instruction::FPToUI:
		case Instruction::FPToSI:
		case Instruction::SIToFP:
		case Instruction::UIToFP:
			return 35;
		case Instruction::FCmp:
			return 10;
		case Instruction::FPExt:
		case Instruction::FPTrunc:
			return 15;
		case Instruction::FDiv:
		case Instruction::FRem:
			return 340;
		}
	}
	else if (isa<CmpInst>(inst))
	{
		const CmpInst *cmp = cast<CmpInst>(inst);
		if (cmp->getPredicate() > 31)
		{
			// integer comparison
			return 37;							// avg.
		}
		else
		{
			return 10;
		}
	}
	return 10;
}



bool ArchitectureVirtexFx::isValidInstruction(const llvm::Instruction *inst) const
{
//    isa<AllocationInst>(inst) || isa<GetElementPtrInst>(inst) || isa<PHINode>(inst))
	if (isa<StoreInst>(inst) || isa<LoadInst>(inst) ||
		isa<TerminatorInst>(inst) || isa<CallInst>(inst))
		return false;
	else
		return true;
}


