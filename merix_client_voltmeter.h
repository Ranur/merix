/**
   USE OF THIS SOFTWARE IS GOVERNED BY THE TERMS AND CONDITIONS
   OF THE LICENSE STATEMENT AND LIMITED WARRANTY FURNISHED WITH
   THE PRODUCT.
   <p/>
   IN PARTICULAR, YOU WILL INDEMNIFY AND HOLD B2N LTD., ITS
   RELATED COMPANIES AND ITS SUPPLIERS, HARMLESS FROM AND AGAINST ANY
   CLAIMS OR LIABILITIES ARISING OUT OF THE USE, REPRODUCTION, OR
   DISTRIBUTION OF YOUR PROGRAMS, INCLUDING ANY CLAIMS OR LIABILITIES
   ARISING OUT OF OR RESULTING FROM THE USE, MODIFICATION, OR
   DISTRIBUTION OF PROGRAMS OR FILES CREATED FROM, BASED ON, AND/OR
   DERIVED FROM THIS SOURCE CODE FILE.
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Voltmeter
// ELEMENT : B25 VOLTAGE SENSOR
///////////WIRING///////////////////////////////////////////////////////////////////
/*
  B25->ARDUINO MEGA2560 / BATTERY
  =====================
  VCC ->BATTERY +
  GND ->BATTERY -
  - ->ARDUINO GND
  s ->ARDUINO ANALOG PIN
*/
////////////////////////////////////////////////////////////////////////////////////



#if defined(MODULE_IS_CLIENT)

#include <Wire.h>

// use this to simulate VOLTMETER readings
//#define CLIENT_VOLTMETER_SIMULATION

#define CLIENT_VOLTMETER_TIMEOUT  10
uint32_t CLIENT_VOLTMETER_LAST_EXECUTE;
#define CLIENT_VOLTMETER_AVERAGE_TIMEOUT  100
uint32_t CLIENT_VOLTMETER_AVERAGE_LAST_EXECUTE;

#define CLIENT_VOLTMETER_DIVIDER 40.92f
float  CLIENT_VOLTMETER_STORAGE[10];
uint8_t  CLIENT_VOLTMETER_STORAGE_COUNTER;

float CLIENT_VOLTMETER_VALUE;

float CLIENT_VOLTMETER_KALMAN_X;
float CLIENT_VOLTMETER_KALMAN_P;
float CLIENT_VOLTMETER_KALMAN_Q;
float CLIENT_VOLTMETER_KALMAN_R;

// Kalman filtering
// if x <= MERIX_NOT_AVAILABLE then assumed not intialized yet
// based on : http://interactive-matter.eu/blog/2009/12/18/filtering-sensor-data-with-a-kalman-filter/
inline void CLIENT_VOLTMETER_KALMAN(float & x, float & p, float & q, float & r, uint16_t & in_out)
{
  if (x <= MERIX_NOT_AVAILABLE)
  {
    x = in_out;
    return;
  }

  // peform Kalman
  p = p + q;
  float k = p / (p + r);
  x = x + k * (((float)in_out) - x);

  p = (1.0f - k) * p;
  in_out =  (uint16_t)x;
}

inline float  CLIENT_VOLTMETER_CALC(uint16_t from_sensor)
{
  // perform kalman
  CLIENT_VOLTMETER_KALMAN(CLIENT_VOLTMETER_KALMAN_X,
                          CLIENT_VOLTMETER_KALMAN_P,
                          CLIENT_VOLTMETER_KALMAN_Q,
                          CLIENT_VOLTMETER_KALMAN_R,
                          from_sensor);
  return ((float)from_sensor) / CLIENT_VOLTMETER_DIVIDER;
}

inline void CLIENT_VOLTMETER_INIT()
{
  CLIENT_VOLTMETER_LAST_EXECUTE = millis();
  CLIENT_VOLTMETER_AVERAGE_LAST_EXECUTE = millis();

  CLIENT_VOLTMETER_STORAGE_COUNTER = 0;

  CLIENT_VOLTMETER_VALUE = MERIX_NOT_AVAILABLE;

  CLIENT_VOLTMETER_KALMAN_X = MERIX_NOT_AVAILABLE;
  CLIENT_VOLTMETER_KALMAN_P = 0.0f;
  CLIENT_VOLTMETER_KALMAN_Q = 0.125f;
  CLIENT_VOLTMETER_KALMAN_R = 4.0f;

  LOG64_SET(F("CLIENT_VOLTMETER: INIT"));
  LOG64_NEW_LINE;
}

inline void CLIENT_VOLTMETER_()
{
#if not defined(CLIENT_VOLTMETER_SIMULATION)
  if (DO_EXECUTE(millis(), CLIENT_VOLTMETER_LAST_EXECUTE, CLIENT_VOLTMETER_TIMEOUT))
  {
    CLIENT_VOLTMETER_LAST_EXECUTE = millis();
    if (CLIENT_VOLTMETER_STORAGE_COUNTER < 10)
    {
      uint16_t value = (uint16_t)analogRead(CLIENT_VOLTMETER_PIN);
      //      LOG64_SET(F("MASTER:"));
      //      LOG64_SET(value);
      //      LOG64_NEW_LINE;
      INSERT_SORT(CLIENT_VOLTMETER_STORAGE,  CLIENT_VOLTMETER_CALC(value), CLIENT_VOLTMETER_STORAGE_COUNTER++);
    }
  }

  if (DO_EXECUTE(millis(), CLIENT_VOLTMETER_AVERAGE_LAST_EXECUTE, CLIENT_VOLTMETER_AVERAGE_TIMEOUT))
  {
    CLIENT_VOLTMETER_AVERAGE_LAST_EXECUTE = millis();

    if (CLIENT_VOLTMETER_STORAGE_COUNTER > 0)
    {
      CLIENT_VOLTMETER_VALUE = CLIENT_VOLTMETER_STORAGE[CLIENT_VOLTMETER_STORAGE_COUNTER / 2];

      CLIENT_VOLTMETER_STORAGE_COUNTER = 0;

    }
  }
#endif
}

inline float CLIENT_VOLTMETER_GET()
{
#if not defined(CLIENT_VOLTMETER_SIMULATION)
  //  LOG64_SET(F("MASTER:"));
  //  LOG64_SET(CLIENT_VOLTMETER_VALUE);
  //  LOG64_NEW_LINE;
  return CLIENT_VOLTMETER_VALUE;
#else
  return ((float)random(12, 14)) + (((float)random(33, 99)) / 100.0f);
#endif
}


#endif

