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
    unsigned int clockRate_;        // PPC - FCM speed

    unsigned int CommNoInOverhead_;
    unsigned int CommNoOutOverhead_;
    
    unsigned int CommInBusCLK_;     // unit size_of_operand (usually 32bit)
    unsigned int CommOutBusCLK_;

    unsigned int CommInBusWidth_;          // unit no_of_input_operand
    unsigned int CommOutBusWidth_;
    
    unsigned int MaxCI_;
    unsigned int MaxInput_;         // per UDI
    unsigned int MaxOutput_;    
public:
    
    Architecture(unsigned int ClockRate = 100 * 1000000);
    virtual void setClockRate(unsigned int);
    
    virtual void setCommNoInOverhead(unsigned int);
    virtual void setCommNoOutOverhead(unsigned int);
    
    virtual void setCommInBusWidth(unsigned int);  
    virtual void setCommOutBusWidth(unsigned int);
    
    virtual void setCommInBusCLK(unsigned int);
    virtual void setCommOutBusCLK(unsigned int);
    
    virtual void setMaxCI(unsigned int);
    virtual void setMaxInput(unsigned int);
    virtual void setMaxOutput(unsigned int);
    

	// returns expected execution time of instruction in implementation-specific units
	virtual unsigned int getSwInstructionTiming(const llvm::Instruction* inst) const = 0;
	virtual unsigned int getHwInstructionTiming(const llvm::Instruction* inst) const = 0;

	// converts HW units to SW units
	virtual unsigned int convertHwToSwTiming(unsigned int hwTiming) const;
    
	// returns expected overhead for one execution in SW units
	virtual unsigned int getExecutionOverhead(unsigned int nInputs, 
                                              unsigned int nOutputs)  const;

	// returns true if instruction is supported on hardware
	virtual bool isValidInstruction(const llvm::Instruction* inst) const = 0;


    virtual unsigned int getMaxInputs() const;
    virtual unsigned int getMaxOutputs() const;
    virtual unsigned int getMaxCustomInstructions() const;
    
    
};

#endif
