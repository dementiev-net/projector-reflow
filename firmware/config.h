#ifndef CONFIG_H
#define CONFIG_H

const char * ver = "3.0";

// hardware
static const uint8_t PIN_TC_CS  = A0;
static const uint8_t PIN_TC_DO  = A1;
static const uint8_t PIN_TC_CLK = A2;
static const uint8_t PIN_HEATER = 3;
static const uint8_t PIN_FAN    = 1;
static const uint8_t PIN_BEEPER = 10;
static const uint16_t BEEP_FREQ = 1976;  // B6 note
static const uint8_t PIN_ZX = 2; // pin for zero crossing detector
static const uint8_t INT_ZX = digitalPinToInterrupt(PIN_ZX); // interrupt for zero crossing detector
static const uint8_t DEFAULT_LOOP_DELAY = 89;  // should be about 16% less for 60Hz mains
static const uint8_t TICKS_PER_SEC      = 100; // for 50Hz mains:  2*50Hz = 100 ticks per second
static const uint8_t TICKS_PER_UPDATE   = 25;
static const uint8_t TICKS_TO_REDRAW    = 50;

// encoder
static const uint8_t PIN_ENC_A   = A3;
static const uint8_t PIN_ENC_B   = A4;
static const uint8_t PIN_ENC_BTN = A5;
static const uint8_t ENC_STEPS_PER_NOTCH = 4;
static const boolean IS_ENC_ACTIVE = false; // encoder module actively fed with VCC ( seems to works bad if set to true )

// temperature
static const uint8_t NUM_TEMP_READINGS  = 5;
static const uint8_t TC_ERROR_TOLERANCE = 5;    // allow for n consecutive errors due to noisy power supply before bailing out
static const float   TEMP_COMPENSATION  = 1.06; // correction factor to match temperature measured with other device

// profiles
static const uint8_t DEFAULT_SOAK_TEPM_A      = 150;
static const uint8_t DEFAULT_SOAK_TEPM_B      = 180;
static const uint8_t DEFAULT_SOAK_DURATION    = 180;
static const uint8_t DEFAULT_PEAK_TEPM        = 250;
static const uint8_t DEFAULT_PEAK_DURATION    = 60;
static const float DEFAULT_RAMP_UP_RATE       = 1.2; // degrees / second (keep it about 1/2 of maximum to prevent PID overshooting)
static const float DEFAULT_RAMP_DOWN_RATE     = 2.0; // degrees / second
static const uint8_t FACTORY_FAN_ASSIST_SPEED = 33;

// PID
// Kp: Determines how aggressively the PID reacts to the current amount of error (Proportional) (double >=0)
// Ki: Determines how aggressively the PID reacts to error over time (Integral) (double>=0)
// Kd: Determines how aggressively the PID reacts to the change in error (Derivative) (double>=0)
//
// Experimental method to tune PID:
//
// > Set all gains to 0.
// > Increase Kd until the system oscillates.
// > Reduce Kd by a factor of 2-4.
// > Set Kp to about 1% of Kd.
// > Increase Kp until oscillations start.
// > Decrease Kp by a factor of 2-4.
// > Set Ki to about 1% of Kp.
// > Increase Ki until oscillations start.
// > Decrease Ki by a factor of 2-4.
#define PID_SAMPLE_TIME 200
#define FACTORY_KP  1.75
#define FACTORY_KI 0.03
#define FACTORY_KD 3.0

double temperature;
uint8_t tcStat = 0;
double Setpoint;
double Input;
double Output;
uint8_t fanValue;
uint8_t heaterValue;
double rampRate = 0;
typedef struct {
  double Kp;
  double Ki;
  double Kd;
} PID_t;
PID_t heaterPID = { FACTORY_KP, FACTORY_KI,  FACTORY_KD };
PID_t fanPID    = { 1.00, 0.00, 0.00 };
int idleTemp    = 50;                  // the temperature at which to consider the oven safe to leave to cool naturally
uint32_t startCycleZeroCrossTicks;
volatile uint32_t zeroCrossTicks = 0;
char buf[20];
int fanAssistSpeed = 33;               // default fan speed

// state machine
typedef enum {
  None     = 0,
  Idle     = 1,
  Settings = 2,
  Edit     = 3,
  UIMenuEnd = 9,
  RampToSoak = 10,
  Soak,
  RampUp,
  Peak,
  RampDown,
  CoolDown,
  Complete = 20,
  Tune = 30
} State;

State currentState  = Idle;

// data type for the values used in the reflow profile
typedef struct profileValues_s {
  int16_t soakTempA;
  int16_t soakTempB;
  int16_t soakDuration;
  int16_t peakTemp;
  int16_t peakDuration;
  double  rampUpRate;
  double  rampDownRate;
  uint8_t checksum;
} Profile_t;

Profile_t activeProfile; // the one and only instance
int activeProfileId = 0;

const uint8_t maxProfiles = 30;

/**
   makeDefaultProfile
*/
void makeDefaultProfile() {
  activeProfile.soakTempA    = DEFAULT_SOAK_TEPM_A;
  activeProfile.soakTempB    = DEFAULT_SOAK_TEPM_B;
  activeProfile.soakDuration = DEFAULT_SOAK_DURATION;
  activeProfile.peakTemp     = DEFAULT_PEAK_TEPM;
  activeProfile.peakDuration =  DEFAULT_PEAK_DURATION;
  activeProfile.rampUpRate   =   DEFAULT_RAMP_UP_RATE;
  activeProfile.rampDownRate =   DEFAULT_RAMP_DOWN_RATE;
}

#endif // CONFIG_H
