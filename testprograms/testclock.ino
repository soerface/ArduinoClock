#define FIRST_HAND_PORT 2
#define MATRIX_LAST_ROW_PORT 52
//                   to 44 in Zweierschritten
#define MATRIX_FIRST_COL_PORT 37

void setup() {
    int port;
    for(int i=0; i<12; i++) {
        port = i + FIRST_HAND_PORT;
        pinMode(port, OUTPUT);
    }
    for(int i=44; i<53; i++) {
        port = i;
        pinMode(port, OUTPUT);
    }
}

void loop() {
    //cycle(200, 10);
    matrixtest();
}

void fade(int port, int from, int to=0, int idletime=1) {
    int i=from;
    while (i != to) {
        analogWrite(port, i);
        if (i < to) {
            i++;
        } else if (i > to) {
            i--;
        }
        delay(idletime);
    }
}

void cycle(int idletime, int brightness) {
    int port;
    for (int i=0; i<12; i++) {
        port = i + FIRST_HAND_PORT;
        analogWrite(port, brightness);
        delay(idletime);
    }
    for (int i=0; i<12; i++) {
        port = i + FIRST_HAND_PORT;
        analogWrite(port, 0);
        delay(idletime);
    }
}

void matrixtest() {
    int port;
    for(int col=0; col<9; col++) {
        port = MATRIX_FIRST_COL_PORT + col * 2;
        digitalWrite(port, HIGH);
        for(int row=0; row<5; row++) {
            int current_number;
            port = MATRIX_LAST_ROW_PORT - row * 2;
            digitalWrite(port, HIGH);
            delay(100);
        }
        delay(500);
        port = MATRIX_FIRST_COL_PORT + col * 2;
        digitalWrite(port, LOW);
    }
}
