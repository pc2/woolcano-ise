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
    const unsigned int mhz = 10000000;	
    if (unsigned int c = a*mhz && c != clockRate_) {
        std::cerr << "- changing value of clockRate: " << c << " MHz\n";
        clockRate_ = c;       
    }
}

void Architecture::setCommNoInOverhead(unsigned int a) {
    std::cerr << "- changing value of CommNoInOverhead_: " << a << "\n";
    CommNoInOverhead_ = a;
}

void Architecture::setCommNoOutOverhead(unsigned int a) {
    std::cerr << "- changing value of CommNoOutOverhead_: " << a << "\n";
    CommNoOutOverhead_ = a;
}

void Architecture::setCommInBusWidth(float a) {
    if (a != CommInBusWidth_) {
        std::cerr << "- changing value of CommInBusWidth: " << a << "\n";
        CommInBusWidth_ = a;       
    }
}  
void Architecture::setCommOutBusWidth(float a) {
    if (a != CommOutBusWidth_) {
        std::cerr << "- changing value of CommOutBusWidth: " << a << "\n";
        CommOutBusWidth_ = a;       
    }
}

void Architecture::setCommInBusCLK(unsigned int a) {
    if (a != CommInBusCLK_) {
        std::cerr << "- changing value of CommInBusCLK_: " << a << "\n";
        CommInBusCLK_ = a;       
    }
}
void Architecture::setCommOutBusCLK(unsigned int a) {
    if (a != CommOutBusCLK_) {
        std::cerr << "- changing value of CommOutBusCLK_: " << a << "\n";
        CommOutBusCLK_ = a;       
    }
}


void Architecture::setMaxCI(unsigned int a) {
    if (a != MaxCI_) {
        std::cerr << "- changing value of MaxCI_: " << a << "\n";
        MaxCI_ = a;       
    }
}

void Architecture::setMaxInput(unsigned int a) {
    if (a != MaxInput_) {
        std::cerr << "- changing value of MaxInput_: " << a << "\n";
        MaxInput_ = a;       
    }
}

void Architecture::setMaxOutput(unsigned int a) {
    if (a != MaxOutput_) {
        std::cerr << "- changing value of MaxOutput_: " << a << "\n";
        MaxOutput_ = a;       
    }
}

// converts HW units to SW units
unsigned int Architecture::convertHwToSwTiming(unsigned int hwTiming) const {
    const float ns = 0.0000000001;
    return   ceil((static_cast<double>(hwTiming) * ns) / 
             (1.0 / static_cast<double>(clockRate_)));
}

// returns expected overhead for one execution in SW units
unsigned int Architecture::getExecutionOverhead(unsigned int nInputs, 
                                                unsigned int nOutputs)  const
{   
/*
    std::cerr << "getExecutionOverhead: in = " << nInputs << " out = " << nOutputs << "\n"
    << " input bus = " << CommInBusWidth_ << " operands"
    << " / speed = " << CommInBusCLK_ << " clks\n" 
    << " output bus = " << CommOutBusWidth_ << " operands"
    << " / speed = " << CommOutBusCLK_ << " clks\n"; 
*/    
    unsigned int clks = 0;
    
    // do not count first CommNoInOverhead transfers
    int nInToCalc = nInputs - (CommInBusWidth_ * CommNoInOverhead_);
    //count rest 
    if (nInToCalc > 0) {
        clks = ceil(nInToCalc / CommInBusWidth_) * CommInBusCLK_;
//        std::cerr << "Free transfers = " << CommNoInOverhead_ << " ; left = " 
//            << nInToCalc << " operands to transfer -> " << clks << " clks\n"; 
    }
    
    // do not count first CommNoOutOverhead transfers
    int nOutToCalc = nOutputs - (CommOutBusWidth_ * CommNoOutOverhead_);
    
    // count rest
    if (nOutToCalc > 0 ) {
        clks += ceil(nOutToCalc / CommOutBusWidth_) * CommOutBusCLK_;
    }
//    std::cerr << "getExecutionOverhead = " << clks << " clks\n\n";
    return clks;
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
    return MaxCI_;
}
