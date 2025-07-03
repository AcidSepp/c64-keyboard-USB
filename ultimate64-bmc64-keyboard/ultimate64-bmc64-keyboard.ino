// Uncomment to debug using serial monitor at 115200 baud
#define DEBUG

#include <HID-Project.h>

#define KEYDELAY 30
#define KEY_COUNT 80

// TOP ROW
#define TAB_SCANCODE 71
#define HOME_SCANCODE 63
#define BACKSPACE_SCANCODE 0

// SECOND ROW
#define LEFT_CTRL_SCANCODE 72

// THIRD ROW
#define RUN_STOP_SCANCODE 77
#define RETURN_SCANCODE 1

// FOURTH ROW
#define C64_SCANCODE 75
#define LEFT_SHIFT_SCANCODE 17
#define SPACE_SCANCODE 74
#define RIGHT_SHIFT_SCANCODE 64
#define DOWN_ARROW_SCANCODE 7
#define RIGHT_ARROW_SCANCODE 2

// F-Keys
#define F1_SCANCODE 4
#define F3_SCANCODE 5
#define F5_SCANCODE 6
#define F7_SCANCODE 3

int debounceDelay = 5;
int RowPinMap[8] = { 9, 3, 4, 5, 6, 7, 8, 2 };
int ColPinMap[10] = { 10, 16, 14, 21, 18, 19, 20, 15, 1, 0 };

bool shifted = false;

int lastKeyState[KEY_COUNT];
long lastDebounceTime[KEY_COUNT];
char keymap[KEY_COUNT];
char keymapShifted[KEY_COUNT];

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
    keymapShifted[i] = NULL;
  }

  // TOP ROW
  keymap[70] = '1';
  keymapShifted[70] = '!';
  keymap[73] = '2';
  keymapShifted[73] = '"';
  keymap[10] = '3';
  keymapShifted[10] = '#';
  keymap[13] = '4';
  keymapShifted[13] = '$';
  keymap[20] = '5';
  keymapShifted[20] = '%';
  keymap[23] = '6';
  keymapShifted[23] = '&';
  keymap[30] = '7';
  keymapShifted[30] = '\'';
  keymap[33] = '8';
  keymapShifted[33] = '(';
  keymap[40] = '9';
  keymapShifted[40] = ')';
  keymap[43] = '0';
  keymapShifted[43] = '0';
  keymap[50] = '+';
  keymapShifted[50] = '+';
  keymap[53] = '-';
  keymapShifted[53] = '-';
  keymap[60] = '~';
  keymapShifted[60] = '~';


  // SECOND ROW
  keymap[76] = 'q';
  keymapShifted[76] = 'Q';
  keymap[11] = 'w';
  keymapShifted[11] = 'W';
  keymap[16] = 'e';
  keymapShifted[16] = 'E';
  keymap[21] = 'r';
  keymapShifted[21] = 'R';
  keymap[26] = 't';
  keymapShifted[26] = 'T';
  keymap[31] = 'y';
  keymapShifted[31] = 'Y';
  keymap[36] = 'u';
  keymapShifted[36] = 'U';
  keymap[41] = 'i';
  keymapShifted[41] = 'I';
  keymap[46] = 'o';
  keymapShifted[46] = 'O';
  keymap[51] = 'p';
  keymapShifted[51] = 'P';
  keymap[56] = '@';
  keymap[61] = '*';

  // THIRD ROW
  keymap[12] = 'a';
  keymapShifted[12] = 'A';
  keymap[15] = 's';
  keymapShifted[15] = 'S';
  keymap[22] = 'd';
  keymapShifted[22] = 'D';
  keymap[25] = 'f';
  keymapShifted[25] = 'F';
  keymap[32] = 'g';
  keymapShifted[32] = 'G';
  keymap[35] = 'h';
  keymapShifted[35] = 'H';
  keymap[42] = 'j';
  keymapShifted[42] = 'J';
  keymap[45] = 'k';
  keymapShifted[45] = 'K';
  keymap[52] = 'l';
  keymapShifted[52] = 'L';
  keymap[55] = ':';
  keymapShifted[55] = '(';
  keymap[62] = ';';
  keymapShifted[62] = ')';
  keymap[65] = '=';

  // BOTTOM ROW
  keymap[14] = 'z';
  keymapShifted[14] = 'Z';
  keymap[27] = 'x';
  keymapShifted[27] = 'X';
  keymap[24] = 'c';
  keymapShifted[24] = 'C';
  keymap[37] = 'v';
  keymapShifted[37] = 'V';
  keymap[34] = 'b';
  keymapShifted[34] = 'B';
  keymap[47] = 'n';
  keymapShifted[47] = 'N';
  keymap[44] = 'm';
  keymapShifted[44] = 'M';
  keymap[57] = ',';
  keymapShifted[57] = '<';
  keymap[54] = '.';
  keymapShifted[54] = '>';
  keymap[67] = '/';
  keymapShifted[67] = '?';
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

        // This is a C64 specific key, we need to map it to a char manually.
        if (shifted) {
          if (keymapShifted[thisKey] != NULL) {
            Keyboard.press(keymapShifted[thisKey]);
          }
        } else {
          if (keymap[thisKey] != NULL) {
            Keyboard.press(keymap[thisKey]);
          }
        }

        if (thisKey == LEFT_SHIFT_SCANCODE || thisKey == RIGHT_SHIFT_SCANCODE) {
          shifted = true;
        }

        // FIRST ROW
        if (thisKey == TAB_SCANCODE) {
          Keyboard.press(KEY_TAB);
          debug("<-: Press Tab");
        }
        if (thisKey == HOME_SCANCODE) {
          Keyboard.press(KEY_HOME);
          debug("CRL HOME: Press Home");
        }
        if (thisKey == BACKSPACE_SCANCODE) {
          Keyboard.press(KEY_BACKSPACE);
          debug("INST DEL: Press Backspace");
        }
        if (thisKey == LEFT_CTRL_SCANCODE) {
          Keyboard.press(KEY_LEFT_CTRL);
          debug("left CTRL: Press Left CRTL");
        }

        // THIRD ROW
        if (thisKey == RUN_STOP_SCANCODE) {
          Keyboard.press(KEY_ESC);
          debug("RUN STOP: Press ESC");
        }
        if (thisKey == RETURN_SCANCODE) {
          Keyboard.press(KEY_RETURN);
          debug("RETURN: Press RETURN");
        }


        // FOURTH ROW
        if (thisKey == C64_SCANCODE) {
          Keyboard.press(KEY_LEFT_WINDOWS);
          debug("C64: Press Left Windows");
        }
        if (thisKey == SPACE_SCANCODE) {
          Keyboard.press(KEY_SPACE);
          debug("Press SPACE");
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

        // F-KEYS
        if (thisKey == F1_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_F2);
            debug("Press F2");
          } else {
            Keyboard.press(KEY_F1);
            debug("Press F1");
          }
        }
        if (thisKey == F3_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_F4);
            debug("Press F4");
          } else {
            Keyboard.press(KEY_F3);
            debug("Press F3");
          }
        }
        if (thisKey == F5_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_F6);
            debug("Press F6");
          } else {
            Keyboard.press(KEY_F5);
            debug("Press F5");
          }
        }
        if (thisKey == F7_SCANCODE) {
          if (shifted) {
            Keyboard.press(KEY_F8);
            debug("Press F8");
          } else {
            Keyboard.press(KEY_F7);
            debug("Press F7");
          }
        }

        lastKeyState[thisKey] = true;
        debug("Press: " + String(thisKey));
      }

      // The key is NOT down but WAS before
      if (!isKeyDown && lastKeyState[thisKey]) {

        // This is a C64 specific key, we need to map it to a char manually.
        if (keymap[thisKey] != NULL) {
          Keyboard.release(keymap[thisKey]);
          Keyboard.release(keymapShifted[thisKey]);
        }

        if (thisKey == LEFT_SHIFT_SCANCODE || thisKey == RIGHT_SHIFT_SCANCODE) {
          shifted = false;
        }

        // FIRST ROW
        if (thisKey == TAB_SCANCODE) {
          Keyboard.release(KEY_TAB);
          debug("<-: Release Tab");
        }
        if (thisKey == HOME_SCANCODE) {
          Keyboard.release(KEY_HOME);
          debug("CRL HOME: Release Home");
        }
        if (thisKey == BACKSPACE_SCANCODE) {
          Keyboard.release(KEY_BACKSPACE);
          debug("INST DEL: Release Backspace");
        }
        if (thisKey == LEFT_CTRL_SCANCODE) {
          Keyboard.release(KEY_LEFT_CTRL);
          debug("left CTRL: Release Left CRTL");
        }

        // THIRD ROW
        if (thisKey == RUN_STOP_SCANCODE) {
          Keyboard.release(KEY_ESC);
          debug("RUN STOP: Release ESC");
        }
        if (thisKey == RETURN_SCANCODE) {
          Keyboard.release(KEY_RETURN);
          debug("RETURN: Release Return");
        }


        // FOURTH ROW
        if (thisKey == C64_SCANCODE) {
          Keyboard.release(KEY_LEFT_WINDOWS);
          debug("C64: Release Left Windows");
        }
        if (thisKey == SPACE_SCANCODE) {
          Keyboard.release(KEY_SPACE);
          debug("Release SPACE");
        }
        if (thisKey == DOWN_ARROW_SCANCODE) {
            Keyboard.release(KEY_UP_ARROW);
            debug("Release UP");
            Keyboard.release(KEY_DOWN_ARROW);
            debug("Release DOWN");
        }
        if (thisKey == RIGHT_ARROW_SCANCODE) {
            Keyboard.release(KEY_LEFT_ARROW);
            debug("Release LEFT");
            Keyboard.release(KEY_RIGHT_ARROW);
            debug("Release RIGHT");
        }

        // F-KEYS
        if (thisKey == F1_SCANCODE) {
            Keyboard.release(KEY_F2);
            debug("Release F2");
            Keyboard.release(KEY_F1);
            debug("Release F1");
        }
        if (thisKey == F3_SCANCODE) {
            Keyboard.release(KEY_F4);
            debug("Release F4");
            Keyboard.release(KEY_F3);
            debug("Release F3");
        }
        if (thisKey == F5_SCANCODE) {
            Keyboard.release(KEY_F6);
            debug("Release F6");
            Keyboard.release(KEY_F5);
            debug("Release F5");
        }
        if (thisKey == F7_SCANCODE) {
            Keyboard.release(KEY_F8);
            debug("Release F8");
            Keyboard.release(KEY_F7);
            debug("Release F7");
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
