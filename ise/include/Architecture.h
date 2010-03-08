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
#ifndef _ARCHITECTURE_H_
#define _ARCHITECTURE_H_

#include "llvm/Instructions.h"
#include <iostream>


class Architecture
{
protected:
    static const unsigned int clockRate = 100 * 1000000;	// PPC running at 100 MHz
    bool CommDisableOverhead_;
    unsigned int CommClkPerInput_;
    float CommInputBusWidth_;   // unit = Input
    unsigned int CommClkPerOutput_;          // 
    float CommOutputBusWidth_;
    unsigned int MaxUDI_;
    unsigned int MaxInput_;   // per UDI
    unsigned int MaxOutput_;  // per UDI    
public:
    
    void setCommDisableOverhead(bool a) { CommDisableOverhead_ = a ; }
    
	// returns expected execution time of instruction in implementation-specific units
	virtual unsigned int getSwInstructionTiming(const llvm::Instruction* inst) const = 0;
	virtual unsigned int getHwInstructionTiming(const llvm::Instruction* inst) const = 0;

	// converts HW units to SW units
	virtual unsigned int convertHwToSwTiming(unsigned int hwTiming) const {
        return ceil((static_cast<double>(hwTiming) * 0.0000000001) / (1.0 / static_cast<double>(clockRate)));
    }

	// returns expected overhead for one execution in SW units
	virtual unsigned int getExecutionOverhead(unsigned int nInputs, 
                                      unsigned int nOutputs)  const {
        
        if (CommDisableOverhead_) 
            return 0;
        
        return ceil(nInputs / CommInputBusWidth_) * CommClkPerInput_ + 
        ceil(nOutputs / CommOutputBusWidth_) * CommClkPerOutput_;
    } 

	// returns true if instruction is supported on hardware
	virtual bool isValidInstruction(const llvm::Instruction* inst) const = 0;


    virtual unsigned int getMaxInputs() const
    {
        return MaxInput_;
    }
    
    virtual unsigned int getMaxOutputs() const
    {
        return MaxOutput_;
    }
    
    virtual unsigned int getMaxCustomInstructions() const
    {
        return MaxUDI_;
    }

    
};

#endif
