// include the library code:
#include <LiquidCrystal.h>
#include "Seeed_HM330X.h"

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int backLight = 10;
typedef enum {
    OFF = 0,
    ON,
    BLINK_SLOW,
    BLINK_NORM,
    BLINK_FAST,
    BLINK_VERY_FAST
} LED_BG_STATE_t;

LED_BG_STATE_t led_bg_state = OFF;

void process_led_bg(void)
{
    static int bg = 0;
    static unsigned long ms = millis();
    switch (led_bg_state)
    {
        case OFF:
            analogWrite(backLight, 0);
            bg = 0;
        break;
        case ON:
            analogWrite(backLight, 200);
            bg = 1;
        break;
        case BLINK_SLOW:
            if (millis() - ms > 3000)
            {
                if (bg == 0) {
                    analogWrite(backLight, 200);
                    bg = 1;
                } else {
                    analogWrite(backLight, 50);
                    bg = 0;
                }
                ms = millis();
            }
        break;
        case BLINK_NORM:
            if (millis() - ms > 2000)
            {
                if (bg == 0) {
                    analogWrite(backLight, 200);
                    bg = 1;
                } else {
                    analogWrite(backLight, 50);
                    bg = 0;
                }
                ms = millis();
            }
        break;
        case BLINK_FAST:
            if (millis() - ms > 1000)
            {
                if (bg == 0) {
                    analogWrite(backLight, 200);
                    bg = 1;
                } else {
                    analogWrite(backLight, 50);
                    bg = 0;
                }
                ms = millis();
            }
        break;
        case BLINK_VERY_FAST:
            if (millis() - ms > 500)
            {
                if (bg == 0) {
                    analogWrite(backLight, 200);
                    bg = 1;
                } else {
                    analogWrite(backLight, 50);
                    bg = 0;
                }
                ms = millis();
            }
        break;
        default:
        break;
    }
}

HM330X sensor;
u8 buf[30];

u16 PM1_0_CF, PM2_5_CF, PM10_CF;
u16 PM1_0, PM2_5, PM10;
u16 sensor_num;

void process_sensor_buff(u8 *buff)
{
    uint8_t sum = 0;
    for (int i = 0; i < 28; i++) {
        sum += buff[i];
    }
    if (sum != buff[28]) {
        lcd.setCursor(0,0);
        lcd.print("wrong checkSum!!!!");
        return;
    }
    sensor_num  = (uint16_t) buff[2] << 8 | buff[3];
    PM1_0_CF    = (uint16_t) buff[4] << 8 | buff[5];
    PM2_5_CF    = (uint16_t) buff[6] << 8 | buff[7];
    PM10_CF     = (uint16_t) buff[8] << 8 | buff[9];
    PM1_0       = (uint16_t) buff[10] << 8 | buff[11];
    PM2_5       = (uint16_t) buff[12] << 8 | buff[13];
    PM10        = (uint16_t) buff[14] << 8 | buff[15];

    Serial.print(sensor_num);
    Serial.print(" ");
    Serial.print(PM1_0_CF);
    Serial.print(" ");
    Serial.print(PM2_5_CF);
    Serial.print(" ");
    Serial.print(PM10_CF);
    Serial.print(" ");
    Serial.print(PM1_0);
    Serial.print(" ");
    Serial.print(PM2_5);
    Serial.print(" ");
    Serial.println(PM10);

    // PM1.0 PM2.5 PM10
    // XXXX  XXXX  XXXX
    // lcd.noDisplay();
    lcd.clear();
    lcd.noBlink();
    lcd.setCursor(0, 0);
    lcd.print("PM1.0 PM2.5 PM10");
    lcd.setCursor(0, 1);
    lcd.print(PM1_0, DEC);
    lcd.setCursor(6, 1);
    lcd.print(PM2_5, DEC);
    lcd.setCursor(12, 1);
    lcd.print(PM10, DEC);
    // lcd.display();

    PM_check();
}

void PM_check(void)
{
    #define PM2_5_GOOD           12
    #define PM2_5_MEDIUM         35
    #define PM2_5_SENSITY        55
    #define PM2_5_UNHEALTHY      150
    #define PM2_5_VERY_UNHEALTHY 250
    #define PM2_5_HAZARD         500

    #define PM10_GOOD           54
    #define PM10_MEDIUM         154
    #define PM10_SENSITY        254
    #define PM10_UNHEALTHY      354
    #define PM10_VERY_UNHEALTHY 424
    #define PM10_HAZARD         604

    // PM2.5
    if (PM2_5 < PM2_5_GOOD) {
        led_bg_state = OFF;
    } else if (PM2_5 < PM2_5_MEDIUM) {
        lcd.setCursor(6, 1);
        lcd.blink();
        led_bg_state = BLINK_SLOW;
    } else if (PM2_5 < PM2_5_SENSITY) {
        lcd.setCursor(6, 1);
        lcd.blink();
        led_bg_state = BLINK_NORM;
    } else if (PM2_5 < PM2_5_UNHEALTHY) {
        lcd.setCursor(6, 1);
        lcd.blink();
        led_bg_state = BLINK_FAST;
    } else if (PM2_5 < PM2_5_VERY_UNHEALTHY) {
        lcd.setCursor(6, 1);
        lcd.blink();
        led_bg_state = BLINK_FAST;
    } else if (PM2_5 < PM2_5_HAZARD) {
        lcd.setCursor(6, 1);
        lcd.blink();
        led_bg_state = BLINK_VERY_FAST;
    } else {
        lcd.setCursor(6, 1);
        lcd.blink();
        led_bg_state = BLINK_VERY_FAST;
    }

    // PM10
    if (PM10 < PM10_GOOD) {
        led_bg_state = OFF;
    } else if (PM10 < PM10_MEDIUM) {
        lcd.setCursor(12, 1);
        lcd.blink();
        led_bg_state = BLINK_SLOW;
    } else if (PM10 < PM10_SENSITY) {
        lcd.setCursor(12, 1);
        lcd.blink();
        led_bg_state = BLINK_NORM;
    } else if (PM10 < PM10_UNHEALTHY) {
        lcd.setCursor(12, 1);
        lcd.blink();
        led_bg_state = BLINK_FAST;
    } else if (PM10 < PM10_VERY_UNHEALTHY) {
        lcd.setCursor(12, 1);
        lcd.blink();
        led_bg_state = BLINK_FAST;
    } else if (PM10 < PM10_HAZARD) {
        lcd.setCursor(12, 1);
        lcd.blink();
        led_bg_state = BLINK_VERY_FAST;
    } else {
        lcd.setCursor(12, 1);
        lcd.blink();
        led_bg_state = BLINK_VERY_FAST;
    }
}

unsigned long s_read_time_ms = 0;
void setup()
{
    // set up the LCD's number of columns and rows:
    lcd.begin(16, 2);
    lcd.clear();

    pinMode(backLight, OUTPUT);
    led_bg_state = OFF;

    if (sensor.init())
    {
        lcd.setCursor(0, 0);
        lcd.print("HM330X failed!!!");
        lcd.setCursor(1, 0);
        lcd.blink();
        while (1)
            ;
    }

    Serial.begin(115200);
}

// TODO: night and day check
void loop()
{
    // main code
    process_led_bg();

    if (millis() - s_read_time_ms > 1000)
    {
        if (sensor.read_sensor_value(buf, 29))
        {
            Serial.println("HM330X read result failed!!!");
        }

        process_sensor_buff(buf);

        s_read_time_ms = millis();
    }
}
