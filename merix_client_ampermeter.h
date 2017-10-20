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
// Ampermeter
#if defined(MODULE_IS_CLIENT)

// use this to simulate ampremeter readings
//#define CLIENT_AMPERMETER_SIMULATION

#define CLIENT_AMPERMETER_TIMEOUT  10
uint32_t CLIENT_AMPERMETER_LAST_EXECUTE;

#define CLIENT_AMPERMETER_AVERAGE_TIMEOUT  100
uint32_t CLIENT_AMPERMETER_AVERAGE_LAST_EXECUTE;

float  CLIENT_AMPERMETER_STORAGE[10];
uint8_t  CLIENT_AMPERMETER_STORAGE_COUNTER;

float CLIENT_AMPERMETER_VALUE;


float CLIENT_AMPERMETER_KALMAN_X;
float CLIENT_AMPERMETER_KALMAN_P;
float CLIENT_AMPERMETER_KALMAN_Q;
float CLIENT_AMPERMETER_KALMAN_R;

// Kalman filtering
// if x <= MERIX_NOT_AVAILABLE then assumed not intialized yet
// based on : http://interactive-matter.eu/blog/2009/12/18/filtering-sensor-data-with-a-kalman-filter/
inline void CLIENT_AMPERMETER_KALMAN(float & x, float & p, float & q, float & r, uint16_t & in_out)
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
  in_out = (uint16_t)x;
}


#define CLIENT_AMPERMETER_COEF 0.07424f //=0.0049/0.066V/A - 66mV/A as per datasheet 
#define CLIENT_AMPERMETER_SUB 37.8787f  //=2.5/0.066V/A - 66mV/A as per datasheet 
// (coef * a) - sub
//  5 / 1023 / 0.66 * a  - 2.5 / 0.066
// 0.66  * ( 5 / 1023 * a - 2.5)
inline float  CLIENT_AMPERMETER_CALC(uint16_t from_sensor)
{

  // perform kalman
  CLIENT_AMPERMETER_KALMAN(CLIENT_AMPERMETER_KALMAN_X,
                           CLIENT_AMPERMETER_KALMAN_P,
                           CLIENT_AMPERMETER_KALMAN_Q,
                           CLIENT_AMPERMETER_KALMAN_R,
                           from_sensor);

  return ( CLIENT_AMPERMETER_COEF * ((float)from_sensor) ) - CLIENT_AMPERMETER_SUB;
}


inline void CLIENT_AMPERMETER_INIT()
{
#if not defined(CLIENT_AMPERMETER_SIMULATION)
  CLIENT_AMPERMETER_LAST_EXECUTE = millis();
  CLIENT_AMPERMETER_AVERAGE_LAST_EXECUTE = millis();

  CLIENT_AMPERMETER_STORAGE_COUNTER = 0;

  CLIENT_AMPERMETER_VALUE = MERIX_NOT_AVAILABLE;

  CLIENT_AMPERMETER_KALMAN_X = MERIX_NOT_AVAILABLE;
  CLIENT_AMPERMETER_KALMAN_P = 0.0f;
  CLIENT_AMPERMETER_KALMAN_Q = 0.125f;
  CLIENT_AMPERMETER_KALMAN_R = 4.0f;

#endif

  LOG64_SET(F("CLIENT_AMPERMETER: INIT"));
  LOG64_NEW_LINE;
}


inline void CLIENT_AMPERMETER_()
{
#if not defined(CLIENT_AMPERMETER_SIMULATION)
  if (DO_EXECUTE(millis(), CLIENT_AMPERMETER_LAST_EXECUTE, CLIENT_AMPERMETER_TIMEOUT))
  {
    CLIENT_AMPERMETER_LAST_EXECUTE = millis();

    if (CLIENT_AMPERMETER_STORAGE_COUNTER < 10)
    {
      uint16_t value = (uint16_t)analogRead(CLIENT_AMPERMETER_PIN);

      INSERT_SORT(CLIENT_AMPERMETER_STORAGE, CLIENT_AMPERMETER_CALC(value) , CLIENT_AMPERMETER_STORAGE_COUNTER++);
    }
  }

  if (DO_EXECUTE(millis(), CLIENT_AMPERMETER_AVERAGE_LAST_EXECUTE, CLIENT_AMPERMETER_AVERAGE_TIMEOUT))
  {
    CLIENT_AMPERMETER_AVERAGE_LAST_EXECUTE = millis();

    if (CLIENT_AMPERMETER_STORAGE_COUNTER > 0)
    {
      float temp_amps = 0.0f;
      for (uint8_t i = 0; i < CLIENT_AMPERMETER_STORAGE_COUNTER; i++)
      {
        temp_amps += CLIENT_AMPERMETER_STORAGE[i];
      }
      temp_amps /= ((float)CLIENT_AMPERMETER_STORAGE_COUNTER);

      CLIENT_AMPERMETER_VALUE = temp_amps;

      //CLIENT_AMPERMETER_VALUE = CLIENT_AMPERMETER_STORAGE[CLIENT_AMPERMETER_STORAGE_COUNTER / 2];

      CLIENT_AMPERMETER_STORAGE_COUNTER = 0;
    }
  }
#endif
}

inline float CLIENT_AMPERMETER_GET()
{
#if not defined(CLIENT_AMPERMETER_SIMULATION)
  return CLIENT_AMPERMETER_VALUE;
#else
  return (((float)random(0, 30)) + (((float)random(0, 99)) / 100.0f)) - 30.0f;
#endif
}

#endif

