/**********************************************************************
 Copyright (c) 2020-2023, Unitree Robotics.Co.Ltd. All rights reserved.
***********************************************************************/
#include <iostream>
#include "FSM/State_GenesisLink.h"

State_GenesisLink::State_GenesisLink(CtrlComponents *ctrlComp)
                :FSMState(ctrlComp, FSMStateName::GENESISLINK, "Genesis link"){}

void State_GenesisLink::enter(){
    for(int i=0; i<4; i++){
        if(_ctrlComp->ctrlPlatform == CtrlPlatform::GAZEBO){
            _lowCmd->setSimStanceGain(i);
        }
        else if(_ctrlComp->ctrlPlatform == CtrlPlatform::REALROBOT){
            _lowCmd->setRealStanceGain(i);
        }
        _lowCmd->setZeroDq(i);
        _lowCmd->setZeroTau(i);
    }
    for(int i=0; i<12; i++){
        _lowCmd->motorCmd[i].q = _lowState->motorState[i].q;
        _startPos[i] = _lowState->motorState[i].q;
    }
    _ctrlComp->setAllStance();
}

void State_GenesisLink::run(){
    if(_lowState->updateAction){
        _lowState->updateAction = false;
        for(int j=0; j<12; j++){
            std::cout << "genesisAction[" << j << "] = " << _lowState->genesisAction[j] << std::endl;
            _lowCmd->motorCmd[j].q = _lowState->genesisAction[j];
        }
    }
}

void State_GenesisLink::exit(){
    _percent = 0;
}

FSMStateName State_GenesisLink::checkChange(){
    if(_lowState->userCmd == UserCommand::L2_B){
        return FSMStateName::PASSIVE;
    }
    else if(_lowState->userCmd == UserCommand::L2_A){
        return FSMStateName::FIXEDSTAND;
    }
#ifdef COMPILE_WITH_MOVE_BASE
    else if(_lowState->userCmd == UserCommand::L2_Y){
        return FSMStateName::MOVE_BASE;
    }
#endif  // COMPILE_WITH_MOVE_BASE
    else{
        return FSMStateName::GENESISLINK;
    }
}