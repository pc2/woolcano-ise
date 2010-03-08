/*
 *  Architecture.cpp
 *  ise
 *
 *  Created by mgrad on 3/8/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */


#include "Architecture.h"


Architecture::Architecture(unsigned int clockRate) {
    clockRate_ = clockRate;
}

void Architecture::setClockRate(unsigned int a) {
    clockRate_ = a;
}

void Architecture::setCommDisableOverhead(bool a) {
    CommDisableOverhead_ = a;
}

void Architecture::setCommClkPerInput(unsigned int a) {
    CommClkPerInput_ = a;
}

void Architecture::setCommInputBusWidth(float a) {
    CommInputBusWidth_ = a;
}  

void Architecture::setCommClkPerOutput(unsigned int a) {
    CommClkPerOutput_ = a;
}

void Architecture::setCommOutputBusWidth(float a) {
    CommOutputBusWidth_ = a;
}

void Architecture::setMaxUDI(unsigned int a) {
    MaxUDI_ = a;
}

void Architecture::setMaxInput(unsigned int a) {
    MaxInput_ = a;
}

void Architecture::setMaxOutput(unsigned int a) {
    MaxOutput_ = a; 
}

// converts HW units to SW units
unsigned int Architecture::convertHwToSwTiming(unsigned int hwTiming) const {
    return   ceil((static_cast<double>(hwTiming) * 0.0000000001) / 
             (1.0 / static_cast<double>(clockRate_)));
}

// returns expected overhead for one execution in SW units
unsigned int Architecture::getExecutionOverhead(unsigned int nInputs, 
                                                unsigned int nOutputs)  const
{
    if (CommDisableOverhead_) 
        return 0;
    
    return ceil(nInputs / CommInputBusWidth_) * CommClkPerInput_ + 
    ceil(nOutputs / CommOutputBusWidth_) * CommClkPerOutput_;
} 


unsigned int Architecture::getMaxInputs() const
{
    return MaxInput_;
}

unsigned int Architecture::getMaxOutputs() const
{
    return MaxOutput_;
}

unsigned int Architecture::getMaxCustomInstructions() const
{
    return MaxUDI_;
}
