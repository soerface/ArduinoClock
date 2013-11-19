/*
 * You can send the text on the next line using Serial Monitor to set the clock to noon Jan 1 2013
 T1357041600
 *
 * A Processing example sketch to automatically send the messages is inclided in the download
 * On Linux, you can use "date +T%s > /dev/ttyACM0" (UTC time zone)
 */

#include <Time.h>
#include <OneWire.h>
//#include <DallasTemperature.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

#define BRIGHTNESS 3 // min. 0.2

#define HOUR_HAND_BRIGHTNESS 80 * BRIGHTNESS
#define MINUTE_HAND_BRIGHTNESS 24 * BRIGHTNESS
#define SECOND_HAND_BRIGHTNESS 5 * BRIGHTNESS

#define ONE_WIRE_BUS 21
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

/*
boolean CELSIUS[5][3] = {
{1, 1, 1},
{1, 0, 0},
{1, 0, 0},
{1, 0, 0},
{1, 1, 1}};
*/

OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature sensors(&oneWire);

void setup() {
    Serial.begin(9600);
    for (int i=FIRST_HAND_PORT; i<FIRST_HAND_PORT+12; i++)
    {
        pinMode(i, OUTPUT);
    }
    for (int i=MATRIX_FIRST_COL_PORT; i<MATRIX_FIRST_COL_PORT+16; i++)
    {
        pinMode(i, OUTPUT);
    }
    pinMode(CLOCK_CENTER_PORT, OUTPUT);
    digitalWrite(CLOCK_CENTER_PORT, 0);
    setSyncProvider(requestSync);  // set function to call when sync required
    Serial.println("Waiting for sync message");
    //sensors.begin();
}

void loop() {
    if (Serial.available()) {
        processSyncMessage();
    }
    if (timeStatus() == timeSet) {
        clock();
        thermometer();
    }
    //delay(1000);
}

void clock() {
    int current_second = second();
    int current_minute = minute();
    int current_hour = hour();
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

void thermometer() {
    //multiplex(measure_temperature());
    multiplex(millis() / 1000.0);
}

/*
float measure_temperature() {
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}
*/

void multiplex(float value) {
    int a = int(value / 10) % 10;
    int b = int(value) % 10;
    int c = int(value * 10) % 10;
    int port = -1;
    for(int col=0; col<9; col++) {
        for(int row=0; row<5; row++) {
            int current_number;
            port = MATRIX_LAST_ROW_PORT - row * 2;
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

void processSyncMessage() {
    unsigned long pctime;
    const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

    if(Serial.find(TIME_HEADER)) {
        pctime = Serial.parseInt();
        if(pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
            setTime(pctime); // Sync Arduino clock to the time received on the serial port
        }
    }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}

