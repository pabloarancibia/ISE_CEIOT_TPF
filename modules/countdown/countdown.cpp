//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include <iostream>
#include <chrono>


#include "countdown.h"
#include "intruder_alarm.h"

#include "mbed_power_mgmt.h"

//=====[Declaration of private defines]======================================


//=====[Declaration and initialization of private global variables]============

static float countdownTime = OFF;
static bool presencia = OFF;

static clock_t begin_time = clock();

//auto t_start = std::chrono::high_resolution_clock::now();
// the work...


//=====[Implementations of public functions]===================================


void countdownInit(){
    countdownTime = OFF;
    //t_start = std::chrono::high_resolution_clock::now();
    //clock_t begin_time = clock();
    // do something
    //std::cout << float( clock () - begin_time ); //  CLOCKS_PER_SEC;
}
void countdownUpdate(){
    presencia = intruderDetectedRead();
    if (presencia){
        countdownDeactivate();
    }else{
        if (!countdownTime){
            countdownActivate();
        }else{
            
            countdownTime = float( clock () - begin_time );;
            //auto t_end = std::chrono::high_resolution_clock::now();
            //countdownTime = std::chrono::duration<double, std::milli>(t_end-t_start).count();
        }
    }
}

void countdownActivate(){
    countdownTime = ON;
    begin_time = clock();
    //t_start = std::chrono::high_resolution_clock::now();
}

void countdownDeactivate(){
    countdownTime = OFF;
    //begin_time = 0;
}

float countdownRead(){
    return countdownTime / 100;
}