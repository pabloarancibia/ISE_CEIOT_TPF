//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "fire_alarm.h"

#include "siren.h"
#include "strobe_light.h"
#include "user_interface.h"
#include "code.h"
#include "date_and_time.h"
#include "temperature_sensor.h"
#include "gas_sensor.h"
#include "matrix_keypad.h"

#include "countdown.h"
#include "motor.h"

//=====[Declaration of private defines]======================================

#define MAX_TIME_ALONE_SEC     15.0


#define TEMPERATURE_C_LIMIT_ALARM     100.0
#define STROBE_TIME_GAS               1000
#define STROBE_TIME_OVER_TEMP          500
#define STROBE_TIME_GAS_AND_OVER_TEMP  100

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalIn alarmTestButton(BUTTON1);

//=====[Declaration and initialization of private global variables]============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool maxTimeCountdownExceeds      = OFF;

static bool gasDetected                  = OFF;
static bool overTemperatureDetected      = OFF;
static bool gasDetectorState             = OFF;
static bool overTemperatureDetectorState = OFF;

//=====[Declarations (prototypes) of private functions]========================

static void fireAlarmActivationUpdate();
static void fireAlarmDeactivationUpdate();
static void fireAlarmDeactivate();
static int fireAlarmStrobeTime();

//=====[Implementations of public functions]===================================

void fireAlarmInit()
{
    countdownInit();

    temperatureSensorInit();
    gasSensorInit();
    sirenInit();
    strobeLightInit();

    motorControlInit();
	
	alarmTestButton.mode(PullDown); 
}

void fireAlarmUpdate()
{
    fireAlarmActivationUpdate();
    fireAlarmDeactivationUpdate();
    sirenUpdate( fireAlarmStrobeTime() );
    strobeLightUpdate( fireAlarmStrobeTime() );	

    motorControlUpdate();
}

bool gasDetectorStateRead()
{
    return gasDetectorState;
}

bool overTemperatureDetectorStateRead()
{
    return overTemperatureDetectorState;
}

bool gasDetectedRead()
{
    return gasDetected;
}

bool overTemperatureDetectedRead()
{
    return overTemperatureDetected;
}

//=====[Implementations of private functions]==================================

static void fireAlarmActivationUpdate()
{
    countdownUpdate();
    temperatureSensorUpdate();
    gasSensorUpdate();

    maxTimeCountdownExceeds = countdownRead() > MAX_TIME_ALONE_SEC;

    overTemperatureDetectorState = temperatureSensorReadCelsius() > 
                                   TEMPERATURE_C_LIMIT_ALARM;

    if (maxTimeCountdownExceeds && overTemperatureDetectorState ) {
        overTemperatureDetected = ON;
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
        motorDirectionWrite(DIRECTION_1);

    }

    gasDetectorState = !gasSensorRead();

    if (maxTimeCountdownExceeds &&  gasDetectorState ) {
        gasDetected = ON;
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
        motorDirectionWrite(DIRECTION_1);
    }
	
    if( alarmTestButton ) {             
        overTemperatureDetected = ON;
        gasDetected = ON;
        sirenStateWrite(ON);
        strobeLightStateWrite(ON);
        motorDirectionWrite(DIRECTION_1);

    }
}

static void fireAlarmDeactivationUpdate()
{
    if ( sirenStateRead() ) {
        if ( codeMatchFrom(CODE_KEYPAD) ||
             codeMatchFrom(CODE_PC_SERIAL) ) {
            fireAlarmDeactivate();
        }
    }
}

static void fireAlarmDeactivate()
{
    sirenStateWrite(OFF);
	strobeLightStateWrite(OFF);
    motorDirectionWrite(STOPPED);
    overTemperatureDetected = OFF;
    gasDetected             = OFF; 
    maxTimeCountdownExceeds = OFF;
    

}

static int fireAlarmStrobeTime()
{
    if( gasDetectedRead() && overTemperatureDetectedRead() ) {
        return STROBE_TIME_GAS_AND_OVER_TEMP;
    } else if ( gasDetectedRead() ) {
        return STROBE_TIME_GAS;
    } else if ( overTemperatureDetectedRead() ) {
        return STROBE_TIME_OVER_TEMP;
    } else {
        return 0;
    }
}
