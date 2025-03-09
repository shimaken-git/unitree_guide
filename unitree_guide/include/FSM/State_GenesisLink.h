/**********************************************************************
 Copyright (c) 2020-2023, Unitree Robotics.Co.Ltd. All rights reserved.
***********************************************************************/
#ifndef GENESISLINK_H
#define GENESISLINK_H

#include "FSM/FSMState.h"

class State_GenesisLink : public FSMState{
public:
    State_GenesisLink(CtrlComponents *ctrlComp);
    ~State_GenesisLink(){}
    void enter();
    void run();
    void exit();
    FSMStateName checkChange();

private:
    float _targetPos[12] = {0.0, 0.67, -1.3, 0.0, 0.67, -1.3, 
                            0.0, 0.67, -1.3, 0.0, 0.67, -1.3};
    float _startPos[12];
    float _duration = 1000;   //steps
    float _percent = 0;       //%
};

#endif  // GENESISLINK_H