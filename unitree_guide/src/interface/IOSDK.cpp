/**********************************************************************
 Copyright (c) 2020-2023, Unitree Robotics.Co.Ltd. All rights reserved.
***********************************************************************/
#ifdef COMPILE_WITH_REAL_ROBOT

#include "interface/IOSDK.h"
#include "interface/WirelessHandle.h"
// #include "interface/KeyBoard.h"
#include <stdio.h>

#ifdef ROBOT_TYPE_Go1
IOSDK::IOSDK():_safe(UNITREE_LEGGED_SDK::LeggedType::Aliengo), _udp(UNITREE_LEGGED_SDK::LOWLEVEL, 8090, "192.168.123.10", 8007), _updateAction(false){
    std::cout << "The control interface for real robot" << std::endl;
    _udp.InitCmdData(_lowCmd);
    cmdPanel = new WirelessHandle();

// #ifdef COMPILE_WITH_MOVE_BASE
    _pub = _nh.advertise<sensor_msgs::JointState>("/realRobot/joint_states", 20);
    _imu_pub = _nh.advertise<sensor_msgs::Imu>("/realRobot/imu", 20);
    _sub = _nh.subscribe("/genesis_angles", 10, &IOSDK::getGenesisCb, this);
    _joint_state.name.resize(12);
    _joint_state.position.resize(12);
    _joint_state.velocity.resize(12);
    _joint_state.effort.resize(12);
// #endif  // COMPILE_WITH_MOVE_BASE
}
#endif

#ifdef ROBOT_TYPE_A1
IOSDK::IOSDK():_safe(UNITREE_LEGGED_SDK::LeggedType::Aliengo), _udp(UNITREE_LEGGED_SDK::LOWLEVEL){
    std::cout << "The control interface for real robot" << std::endl;
    _udp.InitCmdData(_lowCmd);
    cmdPanel = new WirelessHandle();

#ifdef COMPILE_WITH_MOVE_BASE
    _pub = _nh.advertise<sensor_msgs::JointState>("/realRobot/joint_states", 20);
    _joint_state.name.resize(12);
    _joint_state.position.resize(12);
    _joint_state.velocity.resize(12);
    _joint_state.effort.resize(12);
#endif  // COMPILE_WITH_MOVE_BASE
}
#endif


void IOSDK::sendRecv(const LowlevelCmd *cmd, LowlevelState *state){
    for(int i(0); i < 12; ++i){
        _lowCmd.motorCmd[i].mode = cmd->motorCmd[i].mode;
        _lowCmd.motorCmd[i].q    = cmd->motorCmd[i].q;
        _lowCmd.motorCmd[i].dq   = cmd->motorCmd[i].dq;
        _lowCmd.motorCmd[i].Kp   = cmd->motorCmd[i].Kp;
        _lowCmd.motorCmd[i].Kd   = cmd->motorCmd[i].Kd;
        _lowCmd.motorCmd[i].tau  = cmd->motorCmd[i].tau;
    }
    
    _udp.SetSend(_lowCmd);
    _udp.SetSend((char *)"[encryptCRC]");
    _udp.Send();

    _udp.Recv();
    _udp.GetRecv(_lowState);

    for(int i(0); i < 12; ++i){
        state->motorState[i].q = _lowState.motorState[i].q;
        state->motorState[i].dq = _lowState.motorState[i].dq;
        state->motorState[i].ddq = _lowState.motorState[i].ddq;
        state->motorState[i].tauEst = _lowState.motorState[i].tauEst;
        state->motorState[i].mode = _lowState.motorState[i].mode;
    }

    for(int i(0); i < 3; ++i){
        state->imu.quaternion[i] = _lowState.imu.quaternion[i];
        state->imu.gyroscope[i]  = _lowState.imu.gyroscope[i];
        state->imu.accelerometer[i] = _lowState.imu.accelerometer[i];
    }
    state->imu.quaternion[3] = _lowState.imu.quaternion[3];

    cmdPanel->receiveHandle(&_lowState);
    state->userCmd = cmdPanel->getUserCmd();
    state->userValue = cmdPanel->getUserValue();

// #ifdef COMPILE_WITH_MOVE_BASE
    _joint_state.header.stamp = ros::Time::now();
    _joint_state.name = {"FR_hip_joint", "FR_thigh_joint", "FR_calf_joint", 
                         "FL_hip_joint", "FL_thigh_joint", "FL_calf_joint",  
                         "RR_hip_joint", "RR_thigh_joint", "RR_calf_joint", 
                         "RL_hip_joint", "RL_thigh_joint", "RL_calf_joint"};
    for(int i(0); i<12; ++i){
        _joint_state.position[i] = state->motorState[i].q;
        _joint_state.velocity[i] = state->motorState[i].dq;
        _joint_state.effort[i]   = state->motorState[i].tauEst;
    }

    _pub.publish(_joint_state);

    _imu.header.stamp = ros::Time::now();
    _imu.orientation.x = state->imu.quaternion[0];
    _imu.orientation.y = state->imu.quaternion[1];
    _imu.orientation.z = state->imu.quaternion[2];
    _imu.orientation.w = state->imu.quaternion[3];
    _imu.angular_velocity.x = state->imu.gyroscope[0];
    _imu.angular_velocity.y = state->imu.gyroscope[1];
    _imu.angular_velocity.z = state->imu.gyroscope[2];
    _imu.linear_acceleration.x = state->imu.accelerometer[0];
    _imu.linear_acceleration.y = state->imu.accelerometer[1];
    _imu.linear_acceleration.z = state->imu.accelerometer[2];
    _imu_pub.publish(_imu);

    if(_updateAction){
        state->genesisAction = _genesisAction;
        _updateAction = false;
        state->updateAction = true;
    }

// #endif  // COMPILE_WITH_MOVE_BASE
}

void IOSDK::getGenesisCb(const std_msgs::Float32MultiArrayConstPtr& msg)
{
    _genesisAction = msg->data;
    _updateAction = true;
}
#endif  // COMPILE_WITH_REAL_ROBOT