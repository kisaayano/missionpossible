/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Copyright (c) 2011-2014, Purdue University ACM SIG BOTS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Purdue University ACM SIG BOTS nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL PURDUE UNIVERSITY ACM SIG BOTS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Purdue Robotics OS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
void operatorControl() {
	//Config variables 'n stuff
	bool autoM = false;
	bool first = true;
	bool reset = true;
	const int LFRONT=1;
	const int LBACK=2;
	const int RFRONT=3;
	const int RBACK=4;
	const int WINCH1=5;
	const int WINCH2=6;
	const int FLAGDWN=7;
	long startM;
	long endM;
	double circ;
	double MPH;
	//main loop
	while (1) {
		//motorSet(2,50);
		//auto code
		autoM = joystickGetDigital(1, 8, JOY_LEFT);
		if (autoM && first) {
			autonomous();
			first = false;
		}
		//delay(20);
		//Teleop code
		if (joystickGetDigital(1, 7, JOY_UP)) { //Drive straight
			int speed = joystickGetAnalog(1, 3);
			motorSet(LFRONT, speed);
			//delay(5);
			motorSet(LBACK, speed);
			//delay(5);
			motorSet(RFRONT, speed);
			//delay(5);
			motorSet(RBACK, speed);
			//delay(5);
		} else { //tank drive
			int LSpeed = joystickGetAnalog(1, 3);
			int RSpeed = joystickGetAnalog(1, 2);
			motorSet(LFRONT, LSpeed);
			motorSet(LBACK, LSpeed);
			motorSet(RFRONT, RSpeed);
			motorSet(RBACK, RSpeed);
		}
		//Winch 1
		if (joystickGetDigital(1, 5, JOY_UP)) {
			motorSet(WINCH1, 60);
		} else if (joystickGetDigital(1, 5, JOY_DOWN)) {
			motorSet(WINCH1, -60);
		}
		else motorSet(WINCH1, 0);
		//Winch 2
		if (joystickGetDigital(1, 6, JOY_UP)) {
			motorSet(WINCH2, 60);
		} else if (joystickGetDigital(1, 6, JOY_DOWN)) {
			motorSet(WINCH2, -60);
		}
		else motorSet(WINCH2, 0);
		//Flag
		if (joystickGetDigital(1, 8, JOY_DOWN)) {
			motorSet(FLAGDWN, 60);
		}
		else if (joystickGetDigital(1,8, JOY_UP)){
			motorSet(FLAGDWN, -60);
		}
		else motorSet(FLAGDWN, 0);
		//Anemometer
		if(reset){
			startM=micros();
			encoderReset(ameter);
			reset=false;
		}
		long reading = encoderGet(ameter);
		if(reading/360==4){
			endM=micros();
			double revPerSec = (4.0*pow(10.0, 6.0))/((double)(startM-endM));
			double revPerMin = revPerSec*60.0;
			MPH = circ/12.0/5280.0*revPerMin*60.0;
			reset=true;
		}
		//Flush Data

		delay(20);
	}
}
