//imports the SPI library (needed to communicate with Gamebuino's screen)
#include <SPI.h>
//imports the Gamebuino library
#include <Gamebuino.h>
//creates a Gamebuino object named gb
Gamebuino gb;

const byte logo[] PROGMEM = {64,30,
  B00000000,B00000000,B00011111,B11111111,B11111111,B11110000,B00000000,B00000000,
  B01110010,B00000000,B00100001,B00000010,B00000100,B00001000,B00001110,B01000000,
  B01000000,B01110000,B01001001,B00110010,B01100100,B11000100,B00001000,B00001110,
  B01000010,B01000000,B01000001,B00000010,B00000010,B00000100,B00001000,B01001000,
  B01100010,B01100000,B11111111,B11111111,B11111111,B11111110,B00001100,B01001100,
  B01000010,B01000000,B10000010,B00000010,B00000010,B00000010,B00001000,B01001000,
  B01000010,B01000001,B00110010,B00000010,B00110010,B00101001,B00001000,B01001000,
  B00000000,B00000001,B00110010,B00000010,B00110001,B00101001,B00000000,B00000000,
  B00000000,B00000010,B00110010,B00000010,B00110001,B00010100,B10000000,B00000000,
  B01000011,B10000010,B00000010,B00000010,B00000001,B00000000,B10000001,B00001110,
  B01000010,B10000011,B11111111,B11111111,B11111111,B11111111,B10000001,B00001010,
  B01100011,B10000100,B00000100,B00000010,B00000000,B10000000,B01000001,B10001110,
  B01000010,B00000101,B11000100,B00110010,B00001000,B10010011,B01000001,B00001000,
  B01110011,B10001001,B01000100,B01000010,B00111000,B10010001,B00100001,B11001110,
  B00000000,B00001001,B10000100,B01110010,B00011000,B10001011,B00100000,B00000000,
  B00000000,B00010000,B10000100,B01110001,B00111000,B01001011,B00010000,B00000000,
  B01110000,B00010000,B00001000,B00000001,B00000000,B01000000,B00010000,B00001110,
  B01010000,B00100000,B00001000,B00000001,B00000000,B01000000,B00001000,B00001010,
  B01110000,B00111111,B11111111,B11111111,B11111111,B11111111,B11111000,B00001110,
  B01000000,B00100000,B00001000,B00000001,B00000000,B00100000,B00001000,B00001000,
  B01110000,B01000000,B00001000,B00000001,B00000000,B00100000,B00000100,B00001110,
  B00000000,B01000100,B00010000,B11100001,B00010001,B00100010,B01110100,B00000000,
  B00000000,B10001100,B00010000,B10100001,B00110001,B00100110,B01010010,B00000000,
  B01100000,B10000100,B00010000,B11100001,B00010011,B00010010,B01010010,B00001100,
  B01010001,B00000100,B00010000,B10100001,B00010111,B10010010,B01010001,B00001010,
  B01010001,B00001110,B00010000,B11100001,B00111001,B00010111,B01110001,B00001010,
  B01010010,B00000000,B00010000,B00000001,B00000000,B00010000,B00000000,B10001010,
  B01010010,B00000000,B00100000,B00000001,B00000000,B00001000,B00000000,B10001010,
  B00000010,B00000000,B00100000,B00000001,B00000000,B00001000,B00000000,B10000000,
  B00000011,B11111111,B11111111,B11111111,B11111111,B11111111,B11111111,B10000000,
};

#define SIZE_MIN 3
#define SIZE_MAX 5

int size = 3;
int gridSize = size * size;

unsigned char buffer[SIZE_MAX * SIZE_MAX];

boolean gameWin = false;

byte popupTimeLeft;
const __FlashStringHelper* popupText;

int emptyindex = 0;
int emptyrow = 0;
int emptycol = 0;

void setup() {
  Serial.begin(9600);
  gb.begin();
  mainMenu();
}

void loop() {
  if(gb.update()){
    if(gb.buttons.pressed(BTN_C)){
      mainMenu();
    }
    if(gb.buttons.pressed(BTN_B)){
      initBoard();
    }
    if(gameWin) {
      popup(F("\21 You won! \20"));
    } else {
      if(gb.buttons.pressed(BTN_RIGHT)){ 
        int newCol = emptycol - 1;
        if (newCol >= 0) {
          int temp = buffer[emptyindex];
          buffer[emptyindex] = buffer[emptyrow * size + newCol];
          emptyindex = emptyrow * size + newCol;
          emptycol = newCol;
          buffer[emptyindex] = temp;
        }
      } else if(gb.buttons.pressed(BTN_LEFT)){
        int newCol = emptycol + 1;
        if (newCol < size) {
          int temp = buffer[emptyindex];
          buffer[emptyindex] = buffer[emptyrow * size + newCol];
          emptyindex = emptyrow * size + newCol;
          emptycol = newCol;
          buffer[emptyindex] = temp;
        }
      } else if(gb.buttons.pressed(BTN_DOWN)){ 
        int newRow = emptyrow - 1;
        if (newRow >= 0) {
          int temp = buffer[emptyindex];
          buffer[emptyindex] = buffer[newRow * size + emptycol];
          emptyindex = newRow * size + emptycol;
          emptyrow = newRow;
          buffer[emptyindex] = temp;
        }
      } else if(gb.buttons.pressed(BTN_UP)){
        int newRow = emptyrow + 1;
        if (newRow < size) {
          int temp = buffer[emptyindex];
          buffer[emptyindex] = buffer[newRow * size + emptycol];
          emptyindex = newRow * size + emptycol;
          emptyrow = newRow;
          buffer[emptyindex] = temp;
        }
      }
      if(isSolved()) {
        gameWin = true;
      }
    }

    drawField();
    updatePopup();
  }
}

void chooseMap() {
  while (1) {
    if (gb.update()) {
      gb.display.cursorY = LCDHEIGHT - 11;
      printCentered(F("\21 Select level \20"));
      
      if (size == 3) {
        gb.display.cursorY = LCDHEIGHT/2 - 11;
        printCentered(F("easy"));
        gb.display.cursorY = LCDHEIGHT/2 - 2;
        printCentered(F("3 x 3"));
      } else if (size == 4) {
        gb.display.cursorY = LCDHEIGHT/2 - 11;
        printCentered(F("normal"));
        gb.display.cursorY = LCDHEIGHT/2 - 2;
        printCentered(F("4 x 4"));
      } else if (size == 5) {
        gb.display.cursorY = LCDHEIGHT/2 - 11;
        printCentered(F("hard"));
        gb.display.cursorY = LCDHEIGHT/2 - 2;
        printCentered(F("5 x 5"));
      }

      if (gb.buttons.pressed(BTN_A)) {
        initBoard();
        return;
      }
      if (gb.buttons.pressed(BTN_RIGHT))
        size = size + 1;
        if (size > SIZE_MAX) size = SIZE_MIN;
      if (gb.buttons.pressed(BTN_LEFT))
        size = size - 1;
        if (size < SIZE_MIN) size = SIZE_MAX;
      if (gb.buttons.pressed(BTN_C) || gb.buttons.pressed(BTN_B)) {
        mainMenu();
      }
    }
  }
}

void drawField() {
  int CELL_WIDTH  = (LCDWIDTH / size); 
  int CELL_HEIGHT = (LCDHEIGHT / size);
  // draw grid
  gb.display.drawRect(0, 0, LCDWIDTH, LCDHEIGHT);

  for(int i = 1; i < size; i++) {
    gb.display.drawLine(0, CELL_HEIGHT * i, LCDWIDTH, CELL_HEIGHT * i);
    gb.display.drawLine(CELL_WIDTH * i, 0, CELL_WIDTH * i, LCDHEIGHT);
  }

  // print array
  int row = 0;
  int col = 0;
  for (int i=0; i < gridSize; i++) {
    if (buffer[i] > 0) {
      gb.display.cursorX = col * CELL_WIDTH + 5;
      gb.display.cursorY = row * CELL_HEIGHT + 4 ;
      gb.display.println(buffer[i]);
    }
    col++;
    if (col >= size) {
      col = 0;
      row++;
    }
  }
}

void mainMenu() {
  gb.titleScreen(logo);
  gb.pickRandomSeed();
  gb.battery.show = false;
  chooseMap();
}

boolean isSolved() {
  int count = 1;
  for (int i=0; i < gridSize-1; i++) {
    if (buffer[i] != count) {
      return false;
    }
    count++;
  }
  return true;
}

void initBoard() {
  gameWin=false;
  gridSize = size * size;
  gb.pickRandomSeed();
  // init 
  for (int i=0; i < (SIZE_MAX*SIZE_MAX); i++) {
    if ( i < gridSize-1 ) buffer[i] = i+1;
    else buffer[i] = 0;
  }

  // shuffle
  for (int i=0; i < gridSize; i++) {
    int n = random(0, gridSize); 
    int temp = buffer[n];
    buffer[n] =  buffer[i];
    buffer[i] = temp;
  }
  for (int i=0; i < gridSize; i++) {
   if (buffer[i] == 0) {
      emptyindex = i;
      emptyrow = i / size;
      emptycol = i % size;
    }
  }

  gb.sound.playOK();
}

void printCentered(const __FlashStringHelper* text) {
  gb.display.cursorX = (LCDWIDTH / 2) - (strlen_PF((unsigned long) text) * gb.display.fontSize * gb.display.fontWidth / 2);
  gb.display.print(text);
}

void printCentered(char* text) {
  gb.display.cursorX = (LCDWIDTH / 2) - (strlen(text) * gb.display.fontSize * gb.display.fontWidth / 2);
  gb.display.print(text);
}

void popup(const __FlashStringHelper* text) {
  popup(text, 20);
}
void popup(const __FlashStringHelper* text, uint8_t duration) {
  popupText = text;
  popupTimeLeft = duration + 12;
}

void updatePopup() {
  if (popupTimeLeft) {
    uint8_t yOffset = 0;
    if (popupTimeLeft < 12) {
      yOffset = popupTimeLeft - 12;
    }
    byte width = strlen_PF((unsigned long) popupText) * gb.display.fontSize * gb.display.fontWidth;
    gb.display.fontSize = 1;
    gb.display.setColor(BLACK);
    gb.display.drawRect(LCDWIDTH / 2 - width / 2 - 2, yOffset - 1, width + 2, gb.display.fontHeight + 2);
    gb.display.setColor(WHITE);
    gb.display.fillRect(LCDWIDTH / 2 - width / 2 - 1, yOffset - 1, width + 1, gb.display.fontHeight + 1);
    gb.display.setColor(BLACK);
    gb.display.cursorY = yOffset;
    printCentered(popupText);
    popupTimeLeft--;
  }
}

