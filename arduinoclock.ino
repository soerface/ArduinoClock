/*
 * You can send the text on the next line using Serial Monitor to set the clock to noon Jan 1 2013
 T1357041600
 *
 * A Processing example sketch to automatically send the messages is inclided in the download
 * On Linux, you can use "date +T%s > /dev/ttyACM0" (UTC time zone)
 */

#include <Time.h>

#define TIME_HEADER  "T"   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

#define FIRST_HAND 22

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

boolean CELSIUS[5][3] = {
{1, 1, 1},
{1, 0, 0},
{1, 0, 0},
{1, 0, 0},
{1, 1, 1}};

void setup() {
    Serial.begin(9600);
    for (int i=FIRST_HAND; i<FIRST_HAND+12; i++)
    {
        pinMode(i, OUTPUT);
    }
    setSyncProvider( requestSync);  //set function to call when sync required
    Serial.println("Waiting for sync message");
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
    int current_minute = minute();
    int hand = (current_minute + 2) / 5 % 12;
    Serial.println(current_minute);
    Serial.println(hand);
    for(int i=0; i<12; i++) {
        digitalWrite(i+FIRST_HAND, hand==i);
    }
    Serial.println();
}

void thermometer() {
    multiplex(4, 6);
}

void multiplex(int a, int b) {
    for(int col=0; col<9; col++) {
        for(int row=0; row<5; row++) {
            int current_number;
            if (col < 3) {
                digitalWrite(row, NUMBERS[a][row][col % 3]);
            } else if (col < 6) {
                digitalWrite(row, NUMBERS[b][row][col % 3]);
            } else {
                digitalWrite(row, CELSIUS[row][col % 3]);
            }
        }
        digitalWrite(col, HIGH);
        delay(10);
        digitalWrite(col, LOW);
    }
}

void processSyncMessage() {
    unsigned long pctime;
    const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

    if(Serial.find(TIME_HEADER)) {
        pctime = Serial.parseInt();
        if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
            setTime(pctime); // Sync Arduino clock to the time received on the serial port
        }
    }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);
  return 0; // the time will be sent later in response to serial mesg
}

