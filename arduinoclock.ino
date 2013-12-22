#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>
#include <OneWire.h>

#define BRIGHTNESS 3 // min. 0.2

#define HOUR_HAND_BRIGHTNESS 80 * BRIGHTNESS
#define MINUTE_HAND_BRIGHTNESS 24 * BRIGHTNESS
#define SECOND_HAND_BRIGHTNESS 5 * BRIGHTNESS

#define ONE_WIRE_BUS 24
#define FIRST_HAND_PORT 2
#define CLOCK_CENTER_PORT 0
//                   to 13 (total 12)
#define MATRIX_LAST_ROW_PORT 52
//                   to 44 in Zweierschritten
#define MATRIX_FIRST_COL_PORT 37
//                   to 53 in Zweierschritten



static boolean NUMBERS [10][5][3] = {{
{1, 1, 1},
{1, 0, 1},
{1, 0, 1},
{1, 0, 1},
{1, 1, 1}}, {

{0, 0, 1},
{0, 1, 1},
{0, 0, 1},
{0, 0, 1},
{0, 0, 1}}, {

{1, 1, 1},
{0, 0, 1},
{1, 1, 1},
{1, 0, 0},
{1, 1, 1}}, {

{1, 1, 1},
{0, 0, 1},
{1, 1, 1},
{0, 0, 1},
{1, 1, 1}}, {

{1, 0, 1},
{1, 0, 1},
{1, 1, 1},
{0, 0, 1},
{0, 0, 1}}, {

{1, 1, 1},
{1, 0, 0},
{1, 1, 1},
{0, 0, 1},
{1, 1, 1}}, {

{1, 1, 1},
{1, 0, 0},
{1, 1, 1},
{1, 0, 1},
{1, 1, 1}}, {

{1, 1, 1},
{0, 0, 1},
{0, 0, 1},
{0, 0, 1},
{0, 0, 1}}, {

{1, 1, 1},
{1, 0, 1},
{1, 1, 1},
{1, 0, 1},
{1, 1, 1}}, {

{1, 1, 1},
{1, 0, 1},
{1, 1, 1},
{0, 0, 1},
{1, 1, 1}}};

void setup() {
    for (int i=FIRST_HAND_PORT; i<FIRST_HAND_PORT+12; i++)
    {
        pinMode(i, OUTPUT);
    }
    for (int i=MATRIX_FIRST_COL_PORT; i<MATRIX_FIRST_COL_PORT+17; i++)
    {
        pinMode(i, OUTPUT);
    }
    pinMode(CLOCK_CENTER_PORT, OUTPUT);
    digitalWrite(CLOCK_CENTER_PORT, 0);
}

void loop() {
    float current_temperature = 88.8;
    int last_update = 0;
    while (true) {
        clock();
        multiplex(current_temperature);
        if (second() > last_update + 30) {
            matrix_on();
            measure_temperature(current_temperature);
            last_update = second();
        }
    }
}

void clock() {
    tmElements_t tm;
    RTC.read(tm);
    int current_second = tm.Second;
    int current_minute = tm.Minute;
    int current_hour = tm.Hour;
    int second_hand = (current_second + 2) / 5 % 12;
    int minute_hand = (current_minute + 2) / 5 % 12;
    int hour_hand = current_hour % 12;
    for(int i=0; i<12; i++) {
        int port = i+FIRST_HAND_PORT - 1;
        if (i == 0) {
            port = i+FIRST_HAND_PORT + 11;
        }
        if (hour_hand==i) {
            analogWrite(port, HOUR_HAND_BRIGHTNESS);
        } else if (minute_hand==i) {
            analogWrite(port, MINUTE_HAND_BRIGHTNESS);
        } else if (second_hand==i) {
            analogWrite(port, SECOND_HAND_BRIGHTNESS);
        } else {
            analogWrite(port, 0);
        }
    }
    analogWrite(CLOCK_CENTER_PORT, (current_second % 2) * SECOND_HAND_BRIGHTNESS);
}

void measure_temperature(float &current_temperature) {
    // multiplex several times in this function to decrease flickering
    byte addr[8];
    byte data[12];
    byte present = 0;
    OneWire ds(ONE_WIRE_BUS);
    if (!ds.search(addr)) {
        ds.reset_search();
        current_temperature = 88.8;
        return;
    }

    //if (OneWire::crc8(addr, 7) != addr[7]) {
    //    Serial.print("CRC is not valid!\n");
    //    return;
    //}

    multiplex(current_temperature);
    ds.reset();
    multiplex(current_temperature);
    ds.select(addr);
    multiplex(current_temperature);
    //ds.write(0x44, 1);         // start conversion, with parasite power on at the end
    ds.write(0x44, 0);         // start conversion, with parasite power off at the end

    //delay(1000); // if using parasite power
    present = ds.reset();
    multiplex(current_temperature);
    ds.select(addr);
    multiplex(current_temperature);
    ds.write(0xBE);
    for (int i=0; i<9; i++) {
        data[i] = ds.read();
    }
    float temperature;
    temperature = data[0] >> 4;
    temperature += (data[1] & 0b00000111) << 4;
    int fraction = data[0] & 0b00001111;
    for (int i=0; i<4; i++) {
        multiplex(current_temperature);
        if (fraction & 1 << i) {
            switch (i) {
                case 0:
                    temperature += 0.0625;
                    break;
                case 1:
                    temperature += 0.1250;
                    break;
                case 2:
                    temperature += 0.25;
                    break;
                case 3:
                    temperature += 0.5;
                    break;
            }
        }
    }
    current_temperature = temperature;
    return;
}

void multiplex(float value) {
    int a = int(value / 10) % 10;
    int b = int(value) % 10;
    int c = int(value * 10) % 10;
    int port = -1;
    for(int col=0; col<9; col++) {
        for(int row=0; row<5; row++) {
            //int current_number;
            port = MATRIX_LAST_ROW_PORT - row * 2;
            digitalWrite(port, HIGH);
            if (col < 3) {
                digitalWrite(port, NUMBERS[a][row][col % 3]);
            } else if (col < 6) {
                digitalWrite(port, NUMBERS[b][row][col % 3]);
            } else {
                digitalWrite(port, NUMBERS[c][row][col % 3]);
            }
        }
        port = MATRIX_FIRST_COL_PORT + col * 2;
        digitalWrite(port, HIGH);
        delay(1);
        digitalWrite(port, LOW);
    }
}

void matrix_on() {
    int port;
    for(int col=0; col<9; col++) {
        for(int row=0; row<5; row++) {
            port = MATRIX_LAST_ROW_PORT - row * 2;
            digitalWrite(port, HIGH);
        }
        port = MATRIX_FIRST_COL_PORT + col * 2;
        digitalWrite(port, HIGH);
    }
}
