
#include "dev/button-sensor.h"

const struct sensors_sensor button_sensor;
static int button_value = 0;
/*---------------------------------------------------------------------------*/
void
button_press(void)
{
  button_value++;
  sensors_changed(&button_sensor);
}
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  if(!type)
    return 0;
  else if(type == 1) {
    return button_value;
  }
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int value)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static int
status(int type)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(button_sensor, BUTTON_SENSOR,
	       value, configure, status);
