#ifndef MOTOR_H_
#define MOTOR_H_
#include "TestBench.h"
#include "systemc.h"

using namespace std;

SC_MODULE(Motor)
{
	//user input
	sc_in<bool> car_unlocked;
	sc_in<bool> clock;

	sc_in<bool> btn_up_pressed;
	sc_in<bool> btn_up_deep_pressed;
	sc_in<bool> btn_down_pressed;
	sc_in<bool> btn_down_deep_pressed;


	sc_in<TestBench::Window_State> window_sensor;
	sc_out<TestBench::Motor_Action> window_motor;
	sc_out<bool> motor_error;

	sc_event stop;
	sc_event lock;
	sc_event up;
	sc_event down;

	SC_CTOR(Motor) {

		window_motor.initialize(TestBench::Motor_Action::Off);
		motor_error.initialize(false);

	 //--------------- Begin ------------------------
		SC_THREAD(sensor_window);
		sensitive << window_sensor;
		SC_THREAD(panel_control);
		sensitive << btn_up_pressed;
		sensitive << btn_up_deep_pressed;
		sensitive << btn_down_pressed;
		sensitive << btn_down_deep_pressed;
		SC_THREAD(error);
		SC_THREAD(blocked_window);
		sensitive << window_sensor;	

		SC_THREAD(locked);
		sensitive << car_unlocked;
		sensitive << clock.pos();

	};

	//---------------- Aufgabe -------------------------------
	void sensor_window() {
		while (true) {
			wait();
			if (window_sensor.read() == 1) { // window closed
				window_motor.write(TestBench::Motor_Action::Off);
			}
			else if (window_sensor.read() == 2) { // window open
				window_motor.write(TestBench::Motor_Action::Off);
			}
			else if (window_sensor.read() == 0) {  //window middle
			}
		}
	}

	void locked() {
		while (true) {
			wait();
			if (car_unlocked.read()){
			}
			else if ((!car_unlocked.read()) &&(window_sensor.read() != 3)) {
				lock.notify(0, SC_NS);
				window_motor.write(TestBench::Motor_Action::Move_up);
				up.notify(0, SC_NS);		
			}
		}
	}
	
	//motor error

	void error() {
		while (true) {
			double t_start = sc_time_stamp().to_seconds();
			wait(3,SC_SEC,stop);
			double t_end = sc_time_stamp().to_seconds();
			if ((t_end - t_start) < 3) {
			}
			else {

				window_motor.write(TestBench::Motor_Action::Off);
				motor_error.write(true);
				stop.notify();
				sc_stop();
			}
		}
	}

	// hand protection
	void blocked_window() {
		while (true) {
			wait();		
			if (window_sensor.read() == 3) {
				stop.notify(1, SC_NS);
				window_motor.write(TestBench::Motor_Action::Move_down);
				down.notify(1, SC_NS);
			}
		}	
	}

	//normal control  
	void panel_control() {
		while (true) {
			wait();
			if (car_unlocked.read()) {
				// ----------------------- up --------------------------------
				if (btn_up_pressed.read()) {
					window_motor.write(TestBench::Motor_Action::Move_up);
					up.notify(1, SC_NS);	
					wait(btn_up_deep_pressed.default_event() | btn_up_pressed.default_event());
					//------------- deep up ------------------------
					if (btn_up_deep_pressed.read()) {					
						wait(btn_up_deep_pressed.default_event()); 
						double now1 = sc_time_stamp().to_seconds();
						if (!btn_up_deep_pressed.read()) {													
							wait(btn_up_pressed.default_event());
							double now2 = sc_time_stamp().to_seconds();
							double t = now2 - now1;
							if (t < 0.5) {  }
							else{
								window_motor.write(TestBench::Motor_Action::Off);
								stop.notify(1, SC_NS);
							}							
						}
					}
					
					else if (!btn_up_pressed.read()) {
						window_motor.write(TestBench::Motor_Action::Off);
						stop.notify(1, SC_NS);
					}					
				}				
				
				// --------------------------- down --------------------------------
				else if (btn_down_pressed.read()) {
					window_motor.write(TestBench::Motor_Action::Move_down);
					down.notify(1, SC_NS);
					wait(btn_down_deep_pressed.default_event() | btn_down_pressed.default_event());
					// ------------- deep down --------------------
					if (btn_down_deep_pressed.read()) {
						wait(btn_down_deep_pressed.default_event());
						double now3 = sc_time_stamp().to_seconds();
						if (!btn_down_deep_pressed.read()) {

							wait(btn_down_pressed.default_event());
							double now4 = sc_time_stamp().to_seconds();
							double t2 = now4 - now3;
							if (t2 < 0.5) {  }
							else {
								window_motor.write(TestBench::Motor_Action::Off);
								stop.notify(1, SC_NS);
							}
						}
					}
					else if (!btn_down_pressed.read()) {
						window_motor.write(TestBench::Motor_Action::Off);
						stop.notify(1, SC_NS);
					}
				}		
			}
		}
	};

};

#endif