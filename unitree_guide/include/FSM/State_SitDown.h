/**********************************************************************
 Copyright (c) 2020-2023, Unitree Robotics.Co.Ltd. All rights reserved.
***********************************************************************/
#ifndef SITDOWN_H
#define SITDOWN_H

#include "FSM/FSMState.h"

class State_SitDown : public FSMState{
public:
    State_SitDown(CtrlComponents *ctrlComp);
    ~State_SitDown(){}
    void enter();
    void run();
    void exit();
    FSMStateName checkChange();

private:
    float _targetPos[12] = {0.0, 1.25, -2.7, 0.0, 1.25, -2.7, 
                            0.0, 1.25, -2.7, 0.0, 1.25, -2.7};
    float _startPos[12];
    float _duration = 1000;   //steps
    float _percent = 0;       //%
};

#endif  // FIXEDSTAND_H