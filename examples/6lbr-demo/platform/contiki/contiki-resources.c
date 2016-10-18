/*
 * Copyright (c) 2015, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         Sky target resources initialisation
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "contiki.h"

#include "coap-common.h"
#include "core-interface.h"

#if WITH_IPSO_APP_FW
#include "ipso-app-fw.h"
#include "sensors-batch-resource.h"
#endif

#if WITH_LWM2M
#include "lwm2m.h"
#include "lwm2m-device-object.h"
#include "ipso-so.h"
#endif

#if !IGNORE_CETIC_CONTIKI_PLATFORM
#include "contiki-resources.h"

#include "sht-temp-resource.h"
#include "sht-humidity-resource.h"
#include "battery-resource.h"
#include "radio-lqi-resource.h"
#include "radio-rssi-resource.h"
#include "button-resource.h"
#include "led-red-resource.h"
#include "led-green-resource.h"
#include "led-blue-resource.h"

#if PLATFORM_HAS_SHT11
#include "sht11-sensor.h"

#define SENSOR_INIT_SHT_TEMP() SENSORS_ACTIVATE(sht11_sensor)
#define SENSOR_INIT_SHT_HUMIDITY() SENSORS_ACTIVATE(sht11_sensor)

#if REST_RES_SHT_TEMP_RAW
#define REST_REST_SHT_TEMP_VALUE sht11_sensor.value(SHT11_SENSOR_TEMP)
#else
#define REST_REST_SHT_TEMP_VALUE (sht11_sensor.value(SHT11_SENSOR_TEMP) - 3960)
#endif

#if REST_RES_SHT_HUMIDITY_RAW
#define REST_REST_SHT_HUMIDITY_VALUE sht11_sensor.value(SHT11_SENSOR_HUMIDITY)
#else
#define REST_REST_SHT_HUMIDITY_VALUE (((uint32_t)sht11_sensor.value(SHT11_SENSOR_HUMIDITY) * 367 - 20468) / 100)
#endif

#endif /* PLATFORM_HAS_SHT11 */

#if PLATFORM_HAS_SHT21

#include "sht21.h"

#define SENSOR_INIT_SHT_TEMP()
#define SENSOR_INIT_SHT_HUMIDITY()

#if REST_RES_SHT_TEMP_RAW
#define REST_REST_SHT_TEMP_VALUE sht21_read_temp()
#else
#define REST_REST_SHT_TEMP_VALUE (((uint32_t)sht21_read_temp()) * 17572 / 65536 - 4685)
#endif

#if REST_RES_SHT_HUMIDITY_RAW
#define REST_REST_SHT_HUMIDITY_VALUE sht21_read_humidity()
#else
#define REST_REST_SHT_HUMIDITY_VALUE ((((uint32_t)sht21_read_humidity())*12500/65536)-600)
#endif

#endif /* PLATFORM_HAS_SHT21 */

#if PLATFORM_HAS_BATTERY
#include "dev/battery-sensor.h"

#define SENSOR_INIT_BATTERY() SENSORS_ACTIVATE(battery_sensor);

#if REST_RES_BATTERY_RAW
#define REST_REST_BATTERY_VALUE battery_sensor.value(0)
#else
#define REST_REST_BATTERY_VALUE ((battery_sensor.value(0) * 5 * 1000L) / 4096)
#endif
#endif

#if PLATFORM_HAS_RADIO
#include "dev/radio-sensor.h"

#if UDPCLIENT && UDP_CLIENT_STORE_RADIO_INFO
extern int udp_client_lqi;
extern int udp_client_rssi;
#endif

#define SENSOR_INIT_RADIO_LQI() SENSORS_ACTIVATE(radio_sensor);
#define SENSOR_INIT_RADIO_RSSI() SENSORS_ACTIVATE(radio_sensor);

#if UDPCLIENT && UDP_CLIENT_STORE_RADIO_INFO
#define RADIO_LQI_VALUE_SOURCE udp_client_lqi
#define RADIO_RSSI_VALUE_SOURCE udp_client_rssi
#else
#define RADIO_LQI_VALUE_SOURCE radio_sensor.value(RADIO_SENSOR_LAST_PACKET)
#define RADIO_RSSI_VALUE_SOURCE radio_sensor.value(RADIO_SENSOR_LAST_VALUE)
#endif

#if REST_RES_RADIO_LQI_RAW
#define REST_REST_RADIO_LQI_VALUE RADIO_LQI_VALUE_SOURCE
#else
#ifdef CONTIKI_TARGET_CC2538DK
#define REST_REST_RADIO_LQI_VALUE ((RADIO_LQI_VALUE_SOURCE*100)/127)
#else
#define REST_REST_RADIO_LQI_VALUE ((RADIO_LQI_VALUE_SOURCE - 50) * 100 / (110-50))
#endif
#endif

#define REST_REST_RADIO_RSSI_VALUE RADIO_RSSI_VALUE_SOURCE

#endif

#if PLATFORM_HAS_BUTTON
#include "dev/button-sensor.h"
#define SENSOR_INIT_BUTTON() SENSORS_ACTIVATE(button_sensor);

#define REST_REST_BUTTON_VALUE button_sensor.value(0)

#define REST_CONF_RES_RESOURCEID_EVENT_HANDLER(ev, data) \
    if (ev == sensors_event && data == &button_sensor) { \
      resource_button.trigger(); \
      coap_push_update_binding(&resource_button, button_sensor.value(1)); \
    }

#endif

#if PLATFORM_HAS_LEDS
#include "dev/leds.h"

#if REST_RES_LED_RED
inline int led_r_value(void) {
  return ((leds_get() & LEDS_RED) != 0);
}

static int led_r_set(uint32_t value, uint32_t len) {
  if (value) {
    leds_on(LEDS_RED);
  } else {
    leds_off(LEDS_RED);
  }
  return 1;
}

#define SENSOR_INIT_LED_RED()
#define REST_REST_LED_RED_VALUE led_r_value()
#define REST_REST_LED_RED_ACTUATOR led_r_set
#endif

#if REST_RES_LED_GREEN
inline int led_g_value(void) {
  return ((leds_get() & LEDS_GREEN) != 0);
}

static int led_g_set(uint32_t value, uint32_t len) {
  if (value) {
    leds_on(LEDS_GREEN);
  } else {
    leds_off(LEDS_GREEN);
  }
  return 1;
}

#define SENSOR_INIT_LED_GREEN()
#define REST_REST_LED_GREEN_VALUE led_g_value()
#define REST_REST_LED_GREEN_ACTUATOR led_g_set
#endif

#if REST_RES_LED_BLUE
inline int led_b_value(void) {
  return ((leds_get() & LEDS_BLUE) != 0);
}

static int led_b_set(uint32_t value, uint32_t len) {
  if (value) {
    leds_on(LEDS_BLUE);
  } else {
    leds_off(LEDS_BLUE);
  }
  return 1;
}

#define SENSOR_INIT_LED_BLUE()
#define REST_REST_LED_BLUE_VALUE led_b_value()
#define REST_REST_LED_BLUE_ACTUATOR led_b_set
#endif

#endif

REST_RES_SHT_TEMP_DECLARE();
REST_RES_SHT_HUMIDITY_DECLARE();
REST_RES_BATTERY_DECLARE();
REST_RES_RADIO_LQI_DECLARE();
REST_RES_RADIO_RSSI_DECLARE();
REST_RES_BUTTON_DECLARE();
REST_RES_LED_RED_DECLARE();
REST_RES_LED_GREEN_DECLARE();
REST_RES_LED_BLUE_DECLARE();

REST_RES_SHT_TEMP_DEFINE();
REST_RES_SHT_HUMIDITY_DEFINE();
REST_RES_BATTERY_DEFINE();
REST_RES_RADIO_LQI_DEFINE();
REST_RES_RADIO_RSSI_DEFINE();
REST_RES_BUTTON_DEFINE();
REST_RES_LED_RED_DEFINE();
REST_RES_LED_GREEN_DEFINE();
REST_RES_LED_BLUE_DEFINE();

#if WITH_LWM2M
REST_RES_SHT_TEMP_SO_INSTANCE_DEFINE("ucum:Celcius", "Temperature");
REST_RES_SHT_HUMIDITY_SO_INSTANCE_DEFINE("ucum:Celcius", "Humidity");
/*
char const * lwm2m_objects_link = ""
    LWM2M_DEVICE_OBJECT_LINK
    REST_RES_SHT_TEMP_SO_INSTANCE_LINK
    REST_RES_SHT_HUMIDITY_SO_INSTANCE_LINK
  ;
  */
#endif

void
contiki_platform_resources_init(void)
{
  REST_RES_SHT_TEMP_INIT();
  REST_RES_SHT_HUMIDITY_INIT();
  REST_RES_RADIO_LQI_INIT();
  REST_RES_RADIO_RSSI_INIT();
  REST_RES_BUTTON_INIT();
  REST_RES_LED_RED_INIT();
  REST_RES_LED_GREEN_INIT();

  REST_RES_LED_BLUE_INIT();
#if WITH_LWM2M
  REST_RES_BATTERY_INIT_WITH_PATH(LWM2M_DEVICE_PATH(LWM2M_DEVICE_POWER_VOLTAGE_RESOURCE_ID));
#endif
#if WITH_IPSO_APP_FW
  REST_RES_BATTERY_INIT_WITH_PATH(DEVICE_POWER_SUPPLY_ENUM_LINE DEVICE_POWER_SUPPLY_VOLTAGE_RES);
#endif

#if WITH_LWM2M
  REST_RES_SHT_TEMP_SO_INSTANCE_INIT();
  REST_RES_SHT_HUMIDITY_SO_INSTANCE_INIT();
#endif
}
#endif /* !IGNORE_CETIC_CONTIKI_PLATFORM */
