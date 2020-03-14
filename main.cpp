#include "systemc.h"
#include "TestBench.h"

#include "Motor.h"
//#include "Motor_error.h"

#include <string>

int sc_main(int argc, char *argv[]){
	sc_clock clk_sig("clk", 1, SC_SEC);
	// Channel definitions:
	sc_signal<bool> sig_up_pressed;
	sc_signal<bool> sig_up_deep_pressed;
	sc_signal<bool> sig_down_pressed;
	sc_signal<bool> sig_down_deep_pressed;
	sc_signal<bool> sig_unlocked;
	sc_signal<bool,SC_MANY_WRITERS> sig_motor_error;

	
	sc_signal<TestBench::Window_State> sig_window_sensor;
	sc_signal<TestBench::Motor_Action,SC_MANY_WRITERS> sig_window_motor;
	// Connect testbench ports to channels:
	TestBench tb("TestBench", argv[1]);
	tb.btn_up_deep_pressed(sig_up_deep_pressed);
	tb.btn_up_pressed(sig_up_pressed);
	tb.car_unlocked(sig_unlocked);	
	tb.window_sensor(sig_window_sensor);
	tb.window_motor(sig_window_motor);
	tb.btn_down_deep_pressed(sig_down_deep_pressed);
	tb.btn_down_pressed(sig_down_pressed);

	tb.motor_error(sig_motor_error);
	
	// TODO connect your module(s) to channels

	//Motor
	Motor motor("Motor");
	motor.clock(clk_sig);
	motor.car_unlocked(sig_unlocked);
	motor.motor_error(sig_motor_error);

	motor.btn_up_pressed(sig_up_pressed);
	motor.btn_up_deep_pressed(sig_up_deep_pressed);
	motor.btn_down_pressed(sig_down_pressed);
	motor.btn_down_deep_pressed(sig_down_deep_pressed);

	motor.window_sensor(sig_window_sensor);
	motor.window_motor(sig_window_motor);

	// Run simulation:
	sc_start(100, SC_SEC);
	return 0;

}
