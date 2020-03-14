#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include "systemc.h"
#include <string>
#include <map>

using namespace std;

SC_MODULE(TestBench)
{
    // ------------------- Outputs -------------------
    sc_out<bool> btn_up_pressed;
    sc_out<bool> btn_up_deep_pressed;

    sc_out<bool> btn_down_pressed;
    sc_out<bool> btn_down_deep_pressed;

    sc_out<bool> car_unlocked;

    enum Window_State
    {
        Intermediate = 0,
        Closed = 1,
        Open = 2,
        Blocked = 3
    };
    sc_out<Window_State> window_sensor;

    // ------------------- Inputs -------------------
    sc_in<bool> motor_error;

    enum Motor_Action
    {
        Off = 0,
        Move_up = 1,
        Move_down = 2
    };
    sc_in<Motor_Action> window_motor;

    // ------------------- Internal -------------------
    SC_HAS_PROCESS(TestBench);
    TestBench(sc_module_name name, string test_case_name);

    typedef void (TestBench::*FnPtr)(void);
    FnPtr my_test_case;
    void thread_loop();
    void set_window_state();

private:
    void testBlockedAuto();
    void testBlockedManual();
    void testBlockedDuringLocking();

    void testMotorFailManualDown();
    void testMotorFailManualUp();
    void testMotorFailAutoUp();
    void testMotorFailAutoDown();

    void testManualButtonWhenLocked();
    void testLockingWindows();

    void testAutoManualUp();
    void testAutoManualDown();
    void testManualUpRelease();
    void testManualUpSensor();
    void testManualDownRelease();
    void testManualDownSensor();
    void testAutoDownStopSensor();
    void testAutoUpStopSensor();
};

#endif