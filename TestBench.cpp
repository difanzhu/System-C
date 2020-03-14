#include "TestBench.h"
#include "TestHelper.h"
#include <map>
#include <string>

TestBench::TestBench(sc_module_name name, string test_case_name) : sc_module(name)
{
    // Init signals:
    btn_up_pressed.initialize(false);
    btn_up_deep_pressed.initialize(false);
    btn_down_pressed.initialize(false);
    btn_down_deep_pressed.initialize(false);

    car_unlocked.initialize(false);
    window_sensor.initialize(Window_State::Intermediate);

    // Init test list:
    map<std::string, FnPtr> test_cases;
    test_cases["testBlockedAuto"] = &TestBench::testBlockedAuto;
    test_cases["testBlockedManual"] = &TestBench::testBlockedManual;
    test_cases["testBlockedDuringLocking"] = &TestBench::testBlockedDuringLocking;

    test_cases["testMotorFailManualDown"] = &TestBench::testMotorFailManualDown;
    test_cases["testMotorFailManualUp"] = &TestBench::testMotorFailManualUp;
    test_cases["testMotorFailAutoUp"] = &TestBench::testMotorFailAutoUp;
    test_cases["testMotorFailAutoDown"] = &TestBench::testMotorFailAutoDown;

    test_cases["testManualButtonWhenLocked"] = &TestBench::testManualButtonWhenLocked;
    test_cases["testLockingWindows"] = &TestBench::testLockingWindows;

    test_cases["testAutoManualUp"] = &TestBench::testAutoManualUp;
    test_cases["testAutoManualDown"] = &TestBench::testAutoManualDown;
    test_cases["testManualUpRelease"] = &TestBench::testManualUpRelease;
    test_cases["testManualUpSensor"] = &TestBench::testManualUpSensor;
    test_cases["testManualDownRelease"] = &TestBench::testManualDownRelease;
    test_cases["testManualDownSensor"] = &TestBench::testManualDownSensor;
    test_cases["testAutoDownStopSensor"] = &TestBench::testAutoDownStopSensor;
    test_cases["testAutoUpStopSensor"] = &TestBench::testAutoUpStopSensor;

    // Select test function:
    if (test_cases.count(test_case_name) == 0)
    {
        cout << "Unknown test function '" << test_case_name << "'" << endl;
        exit(2);
    }
    my_test_case = test_cases[test_case_name];
    SC_THREAD(thread_loop);
    SC_METHOD(set_window_state);
}

void TestBench::set_window_state()
{    
    if (window_motor.read() == Motor_Action::Off)
    {
        return;
    }
    window_sensor.write(Window_State::Intermediate);
}

void TestBench::thread_loop()
{
    (this->*my_test_case)();
}

// Test cases

// =================== Einklemmschutz ===================
void TestBench::testBlockedAuto()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_up_pressed, "btn_up_pressed", true);
    send_signal(&btn_up_deep_pressed, "btn_up_deep_pressed", true);
    send_signal(&btn_up_deep_pressed, "btn_up_deep_pressed", false);
    send_signal(&btn_up_pressed, "btn_up_pressed", false);

    wait(1, SC_SEC);

    send_signal(&window_sensor, "window_sensor", Window_State::Blocked);
    assert_now(&window_motor, "window_motor", Motor_Action::Move_down, "testBlockedAuto failed");

    send_signal(&window_sensor, "window_sensor", Window_State::Open);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testBlockedAuto failed");
}

void TestBench::testBlockedManual()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_up_pressed, "btn_up_pressed", true);
    wait(1, SC_SEC);

    send_signal(&window_sensor, "window_sensor", Window_State::Blocked);
    assert_now(&window_motor, "window_motor", Motor_Action::Move_down, "testBlockedManual failed");

    send_signal(&window_sensor, "window_sensor", Window_State::Open);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testBlockedManual failed");
}

void TestBench::testBlockedDuringLocking()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&car_unlocked, "car_unlocked", false);
    assert_now(&window_motor, "window_motor", Motor_Action::Move_up, "testBlockedDuringLocking failed");

    send_signal(&window_sensor, "window_sensor", Window_State::Blocked);
    assert_now(&window_motor, "window_motor", Motor_Action::Move_down, "testBlockedDuringLocking failed");

    send_signal(&window_sensor, "window_sensor", Window_State::Open);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testBlockedDuringLocking failed");
}

// =================== Motorfehler ===================

void TestBench::testMotorFailManualDown()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    assert_now(&motor_error, "motor_error", false, "testMotorFailManualDown failed");
    send_signal(&btn_down_pressed, "btn_down_pressed", true);

    wait(4, SC_SEC);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testMotorFailManualDown failed");
    assert_now(&motor_error, "motor_error", true, "testMotorFailManualDown failed");
}

void TestBench::testMotorFailManualUp()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    assert_now(&motor_error, "motor_error", false, "testMotorFailManualUp failed");
    send_signal(&btn_up_pressed, "btn_up_pressed", true);

    wait(4, SC_SEC);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testMotorFailManualUp failed");
    assert_now(&motor_error, "motor_error", true, "testMotorFailManualUp failed");
}

void TestBench::testMotorFailAutoUp()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_up_pressed, "btn_up_pressed", true);
    send_signal(&btn_up_deep_pressed, "btn_up_deep_pressed", true);
    send_signal(&btn_up_deep_pressed, "btn_up_deep_pressed", false);
    send_signal(&btn_up_pressed, "btn_up_pressed", false);

    assert_now(&motor_error, "motor_error", false, "testMotorFailAutoUp failed");
    wait(4, SC_SEC);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testMotorFailAutoUp failed");
    assert_now(&motor_error, "motor_error", true, "testMotorFailAutoUp failed");
}

void TestBench::testMotorFailAutoDown()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_down_pressed, "btn_down_pressed", true);
    send_signal(&btn_down_deep_pressed, "btn_down_deep_pressed", true);
    send_signal(&btn_down_deep_pressed, "btn_down_deep_pressed", false);
    send_signal(&btn_down_pressed, "btn_down_pressed", false);

    assert_now(&motor_error, "motor_error", false, "testMotorFailAutoDown failed");
    wait(4, SC_SEC);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testMotorFailAutoDown failed");
    assert_now(&motor_error, "motor_error", true, "testMotorFailAutoDown failed");
}

// =================== Verriegelung ===================
void TestBench::testManualButtonWhenLocked()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_down_pressed, "btn_down_pressed", true);
    send_signal(&btn_down_pressed, "btn_down_pressed", false);

    send_signal(&car_unlocked, "car_unlocked", false);
    send_signal(&window_sensor, "window_sensor", Window_State::Closed);

    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testManualButtonWhenLocked failed");
    send_signal(&btn_down_pressed, "btn_down_pressed", true);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testManualButtonWhenLocked failed");
}

void TestBench::testLockingWindows()
{
    send_signal(&car_unlocked, "car_unlocked", true);
    send_signal(&car_unlocked, "car_unlocked", false);
    assert_now(&window_motor, "window_motor", Motor_Action::Move_up, "testLockingWindows failed");

    send_signal(&window_sensor, "window_sensor", Window_State::Closed);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testLockingWindows failed");
}

// =================== Normalbetrieb ===================
void TestBench::testAutoManualUp()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_up_pressed, "btn_up_pressed", true);
    send_signal(&btn_up_deep_pressed, "btn_up_deep_pressed", true);
    send_signal(&btn_up_deep_pressed, "btn_up_deep_pressed", false);
    wait(1, SC_SEC);

    assert_now(&window_motor, "window_motor", Motor_Action::Move_up, "testAutoManualUp failed");
    send_signal(&btn_up_pressed, "btn_up_pressed", false);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testAutoManualUp failed");
}

void TestBench::testAutoManualDown()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_down_pressed, "btn_down_pressed", true);
    send_signal(&btn_down_deep_pressed, "btn_down_deep_pressed", true);
    send_signal(&btn_down_deep_pressed, "btn_down_deep_pressed", false);
    wait(1, SC_SEC);

    assert_now(&window_motor, "window_motor", Motor_Action::Move_down, "testAutoManualDown failed");
    send_signal(&btn_down_pressed, "btn_down_pressed", false);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testAutoManualDown failed");
}

void TestBench::testManualUpRelease()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_up_pressed, "btn_up_pressed", true);
    wait(1, SC_SEC);

    assert_now(&window_motor, "window_motor", Motor_Action::Move_up, "testManualUpRelease failed");
    send_signal(&btn_up_pressed, "btn_up_pressed", false);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testManualUpRelease failed");
}

void TestBench::testManualUpSensor()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_up_pressed, "btn_up_pressed", true);
    wait(1, SC_SEC);

    assert_now(&window_motor, "window_motor", Motor_Action::Move_up, "testManualUpSensor failed");
    send_signal(&window_sensor, "window_sensor", Window_State::Closed);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testManualUpSensor failed");
}

void TestBench::testManualDownRelease()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_down_pressed, "btn_down_pressed", true);
    wait(1, SC_SEC);

    assert_now(&window_motor, "window_motor", Motor_Action::Move_down, "testManualDownRelease failed");
    send_signal(&btn_down_pressed, "btn_down_pressed", false);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testManualDownRelease failed");
}

void TestBench::testManualDownSensor()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_down_pressed, "btn_down_pressed", true);
    wait(1, SC_SEC);

    assert_now(&window_motor, "window_motor", Motor_Action::Move_down, "testManualDownSensor failed");
    send_signal(&window_sensor, "window_sensor", Window_State::Open);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testManualDownSensor failed");
}

void TestBench::testAutoDownStopSensor()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_down_pressed, "btn_down_pressed", true);
    send_signal(&btn_down_deep_pressed, "btn_down_deep_pressed", true);
    send_signal(&btn_down_deep_pressed, "btn_down_deep_pressed", false);
    send_signal(&btn_down_pressed, "btn_down_pressed", false);

    assert_now(&window_motor, "window_motor", Motor_Action::Move_down, "testAutoDownStopSensor failed");
    send_signal(&window_sensor, "window_sensor", Window_State::Open);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testAutoDownStopSensor failed");
}

void TestBench::testAutoUpStopSensor()
{
    send_signal(&car_unlocked, "car_unlocked", true);

    send_signal(&btn_up_pressed, "btn_up_pressed", true);
    send_signal(&btn_up_deep_pressed, "btn_up_deep_pressed", true);
    send_signal(&btn_up_deep_pressed, "btn_up_deep_pressed", false);
    wait(1, SC_SEC);

    assert_now(&window_motor, "window_motor", Motor_Action::Move_up, "testAutoUpStopSensor failed");
    send_signal(&window_sensor, "window_sensor", Window_State::Closed);
    assert_now(&window_motor, "window_motor", Motor_Action::Off, "testAutoUpStopSensor failed");
}