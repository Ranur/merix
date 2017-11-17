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
// Button managment

// Supports RESET and MASTER RESET 
// MASTER RESET - clean all no mater from which server the reset is arriving
// MASTER RESET - is activated only if no mesage from our server is received in 5 min from start/power on

#if defined(MODULE_IS_SERVER)

#define SERVER_BUTTON_TIMEOUT  10
uint32_t SERVER_BUTTON_LAST_EXECUTE;

// in msec
#define SERVER_BUTTON_LONG_PUSH_TIMEOUT 5000
uint32_t SERVER_BUTTON_LAST_DOWN;


inline void SERVER_BUTTON_INIT()
{
  SERVER_BUTTON_LAST_DOWN = 0;
  pinMode(SERVER_BUTTON_PIN, INPUT_PULLUP);
}

inline void SERVER_BUTTON_CLICK()
{
  LOG64_SET(F("SERVER_BUTTON : CLICK : TOGLE DISPLAY BACKLIGHT"));
  LOG64_NEW_LINE;

  SERVER_DISPLAY_TOGLE_BACKLIGHT();
}

inline void SERVER_BUTTON_LONG_PUSH()
{
  LOG64_SET(F("SERVER_BUTTON : LONG PUSH : RESET"));
  LOG64_NEW_LINE;

  SERVER_RESET();
}


inline void SERVER_BUTTON_()
{
  if (DO_EXECUTE(millis(), SERVER_BUTTON_LAST_EXECUTE, SERVER_BUTTON_TIMEOUT))
  {

    SERVER_BUTTON_LAST_EXECUTE = millis();

    if (digitalRead(SERVER_BUTTON_PIN) == LOW)
    {
      
      if (SERVER_BUTTON_LAST_DOWN == 0)
      {
        SERVER_BUTTON_LAST_DOWN = millis();
      }
    }
    else
    {
      
      if (SERVER_BUTTON_LAST_DOWN != 0)
      {
        // check if > of timeout
        if (DO_EXECUTE(millis(), SERVER_BUTTON_LAST_DOWN, SERVER_BUTTON_LONG_PUSH_TIMEOUT))
        {
          SERVER_BUTTON_LONG_PUSH();
        }
        else
        {
          SERVER_BUTTON_CLICK();
        }

        SERVER_BUTTON_LAST_DOWN = 0;
      }
    }
  }
}

#endif
