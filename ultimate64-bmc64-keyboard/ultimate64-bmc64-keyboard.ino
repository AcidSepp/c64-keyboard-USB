// Uncomment to debug using serial monitor at 115200 baud
#define DEBUG

#include <HID-Project.h>

#define KEYDELAY 30
#define KEY_COUNT 80

#define RIGHT_SHIFT_SCANCODE 64
#define LEFT_SHIFT_SCANCODE 17

#define DOWN_ARROW_SCANCODE 7
#define RIGHT_ARROW_SCANCODE 2

int debounceDelay = 5;
int RowPinMap[8] = { 9, 3, 4, 5, 6, 7, 8, 2 };
int ColPinMap[10] = { 10, 16, 14, 21, 18, 19, 20, 15, 1, 0 };

bool shifted = false;

int lastKeyState[KEY_COUNT];
long lastDebounceTime[KEY_COUNT];
KeyboardKeycode keymap[KEY_COUNT];
char directKeymap[KEY_COUNT];

void debug(String outstring) {
#ifdef DEBUG
  Serial.println(outstring);
#endif
}

void bootsetup() {

#ifdef DEBUG
  Serial.begin(115200);
#endif

  Keyboard.begin();
  Keyboard.releaseAll();

  for (int i = 0; i < KEY_COUNT; i++) {
    keymap[i] = NULL;
  }

  for (int i = 0; i < KEY_COUNT; i++) {
    directKeymap[i] = NULL;
  }

  // TOP ROW
  keymap[71] = KEY_TAB;
  keymap[70] = KEY_1;
  keymap[73] = KEY_2;
  keymap[10] = KEY_3;
  keymap[13] = KEY_4;
  keymap[20] = KEY_5;
  keymap[23] = KEY_6;
  keymap[30] = KEY_7;
  keymap[33] = KEY_8;
  keymap[40] = KEY_9;
  keymap[43] = KEY_0;
  directKeymap[50] = '+';
  directKeymap[53] = '-';
  keymap[60] = KEY_BACKSLASH;
  keymap[63] = KEY_HOME;
  keymap[0] = KEY_BACKSPACE;


  // SECOND ROW
  keymap[72] = KEY_LEFT_CTRL;
  keymap[76] = KEY_Q;
  keymap[11] = KEY_W;
  keymap[16] = KEY_E;
  keymap[21] = KEY_R;
  keymap[26] = KEY_T;
  keymap[31] = KEY_Y;
  keymap[36] = KEY_U;
  keymap[41] = KEY_I;
  keymap[46] = KEY_O;
  keymap[51] = KEY_P;
  directKeymap[56] = '@';
  directKeymap[61] = '*';
  // directKeymap[66] = KEY_UP_ARROW;
  //   TODO: RESTORE

  // THIRD ROW

  keymap[77] = KEY_ESC;
  keymap[LEFT_SHIFT_SCANCODE] = KEY_LEFT_SHIFT;
  keymap[12] = KEY_A;
  keymap[15] = KEY_S;
  keymap[22] = KEY_D;
  keymap[25] = KEY_F;
  keymap[32] = KEY_G;
  keymap[35] = KEY_H;
  keymap[42] = KEY_J;
  keymap[45] = KEY_K;
  keymap[52] = KEY_L;
  directKeymap[55] = ':';
  directKeymap[62] = ';';
  directKeymap[65] = '=';
  keymap[1] = KEY_ENTER;

  // BOTTOM ROW

  keymap[75] = KEY_LEFT_WINDOWS;
  keymap[14] = KEY_Z;
  keymap[27] = KEY_X;
  keymap[24] = KEY_C;
  keymap[37] = KEY_V;
  keymap[34] = KEY_B;
  keymap[47] = KEY_N;
  keymap[44] = KEY_M;
  directKeymap[57] = ',';
  directKeymap[54] = '.';
  keymap[67] = KEY_SLASH;
  keymap[RIGHT_SHIFT_SCANCODE] = KEY_RIGHT_SHIFT;
  // keymap[DOWN_ARROW_SCANCODE] = KEY_DOWN_ARROW;
  // keymap[RIGHT_ARROW_SCANCODE] = KEY_RIGHT_ARROW;

  // SPACE AND F1-F8

  keymap[74] = KEY_SPACE;
  keymap[4] = KEY_F1;
  keymap[5] = KEY_F3;
  keymap[6] = KEY_F5;
  keymap[3] = KEY_F7;
}


void setup() {

  bootsetup();
  for (int i = 0; i < 80; i++) lastKeyState[i] = false;
  for (int Row = 0; Row < 8; Row++) pinMode(RowPinMap[Row], INPUT_PULLUP);
  for (int Col = 0; Col < 10; Col++) pinMode(ColPinMap[Col], INPUT_PULLUP);
}

void specialKeyPress() {
}

void specialKeyRelease() {
}

void loop() {

  for (int Row = 0; Row < 8; Row++) {

    int RowPin = RowPinMap[Row];
    pinMode(RowPin, OUTPUT);
    digitalWrite(RowPin, LOW);

    for (int Col = 0; Col < 10; Col++) {

      int thisKey = Col + (Row * 10);
      bool isKeyDown = !digitalRead(ColPinMap[Col]);

      // Non-blocking delay
      if (millis() < lastDebounceTime[thisKey] + debounceDelay) continue;

      // Is the key currently down and wasn't before?
      if (isKeyDown && !lastKeyState[thisKey]) {

        // This is a "normal" key. The scancode can be passed on directly.
        if (keymap[thisKey] != NULL) {
          Keyboard.press(keymap[thisKey]);
        }

        // This is a C64 specific key, we need to map it to a char manually.
        if (directKeymap[thisKey] != NULL) {
          Keyboard.press(directKeymap[thisKey]);
        }

        if (thisKey == LEFT_SHIFT_SCANCODE || thisKey == RIGHT_SHIFT_SCANCODE) {
          shifted = true;
        }

        if (thisKey == DOWN_ARROW_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_UP_ARROW);
            debug("Press UP");
          } else {
            Keyboard.press(KEY_DOWN_ARROW);
            debug("Press DOWN");
          }
        }
        if (thisKey == RIGHT_ARROW_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_LEFT_ARROW);
            debug("Press LEFT");
          } else {
            Keyboard.press(KEY_RIGHT_ARROW);
            debug("Press RIGHT");
          }
        }

        lastKeyState[thisKey] = true;
        debug("Press: " + String(thisKey));
      }

      // The key is NOT down but WAS before
      if (!isKeyDown && lastKeyState[thisKey]) {

        // This is a "normal" key. The scancode can be passed on directly.
        if (keymap[thisKey] != NULL) {
          Keyboard.release(keymap[thisKey]);
        }

        // This is a C64 specific key, we need to map it to a char manually.
        if (directKeymap[thisKey] != NULL) {
          Keyboard.release(directKeymap[thisKey]);
        }

        if (thisKey == LEFT_SHIFT_SCANCODE || thisKey == RIGHT_SHIFT_SCANCODE) {
          shifted = false;
        }

        if (thisKey == DOWN_ARROW_SCANCODE) {
          Keyboard.release(KEY_UP_ARROW);
          Keyboard.release(KEY_DOWN_ARROW);
        }

        if (thisKey == RIGHT_ARROW_SCANCODE) {
          Keyboard.release(KEY_RIGHT_ARROW);
          Keyboard.release(KEY_LEFT_ARROW);
        }

        lastKeyState[thisKey] = false;
        debug("Release: " + String(thisKey));
      }

      lastDebounceTime[thisKey] = millis();
    }

    digitalWrite(RowPin, HIGH);
    delay(1);
    pinMode(RowPin, INPUT_PULLUP);
  }
}
