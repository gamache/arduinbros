/* arduinbros.ino
   a port of rainbros (dicks + lolcat) for the Arduino and GOLDELOX display
   march 2012  pete gamache  pete@#$!@#gamache.org
    ,
   (k) all rights reversed
*/


// pins and byte buffer
#define LED 13
byte received_byte;

// dicks
char *dicks[16] = {
  "8D",               // idx = 0, len = 2.  CHOAD.
  "8=D",
  "8==D",
  "8===D",
  "8====D",
  "8=====D",
  "8======D",
  "8=======D",
  "8========D",
  "8=========D",
  "8==========D",
  "8===========D",
  "8============D",
  "8=============D",
  "8==============D",
  "8===============D" // idx = 15, len = 17. SWANGIN.
};
#define DICKFOR(N) dicks[(N) & 0xF]

// here we define our sequence of rainbow colors
byte rainbow_r[7] = {0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x4B, 0xCC};
byte rainbow_g[7] = {0x00, 0x80, 0xFF, 0xFF, 0x00, 0x00, 0x22};
byte rainbow_b[7] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0x82, 0x88};
int _rainbow = 0; // starting offset from RED for first char of a line
// RAINBOW_RGB returns the r, g, b bytes for the rainbow color N=0..6
#define RAINBOW_RGB(N) rainbow_r[(N)], rainbow_g[(N)], rainbow_b[(N)]

// GOLDELOX text stuff
#define FONT 0     // 5x7
#define NROWS 8
#define NCOLS 16
#define BGCOLOR 0,0,0
byte _row=0, _col=0;   // current text cursor position
#define ADVANCE_RAINBOW  if (_rainbow==6) _rainbow=0; else _rainbow++;
#define NEWLINE _row++; _col=0; ADVANCE_RAINBOW;
#define ORIGIN  _row=0; _col=0; ADVANCE_RAINBOW;

// macros to pack byte R, G, B data into two bytes, rg and gb
#define RG(R,G,B) (((R) & 0xf8) | ((G) >> 5))
#define GB(R,G,B) (((G) << 5) | ((B) >> 3))

// this macro handles the ack byte returned by the GOLDELOX.
// returns 1 on success (ACK), 0 on error (NAK)
#define HANDLE_ACK \
        while (! Serial.available()) ; return Serial.read()==0x06 ? 1 : 0;


// drawChar prints a single character at the current position, 
// in the specified color.  Advances the current position (_row, _col).
int drawChar(char c, byte r, byte g, byte b) {
  Serial.write(0x54);
  Serial.write(c);
  if (_col >= NCOLS) { NEWLINE; }
  Serial.write(_col++);
  Serial.write(_row);
  Serial.write(RG(r,g,b));
  Serial.write(GB(r,g,b));
  HANDLE_ACK;
}

// drawString prints an ASCII string at the current position, with NEWLINE.
// Color is in a rainbow fashion, a la lolcat.
void drawString(char *str) {
  int color;
  color = _rainbow;

  while (*str) {
    drawChar(*str++, RAINBOW_RGB(color));
    if (color == 6) color = 0; else color++;
  }
  NEWLINE;
}

// draw a screenful of dicks
void drawDicks() {
  for (int row=0; row < NROWS; row++) {
    drawString(DICKFOR(random(0,15))); 
  }
}

// sets the screen font (yup)
int setFont(byte f) {
  Serial.write(0x46);
  Serial.write(f);
  HANDLE_ACK;
}

// changes screen background color
int setBg(int r, int g, int b) {
  Serial.write(0x42);
  Serial.write(RG(r,g,b));
  Serial.write(GB(r,g,b));
  HANDLE_ACK;
}

// clears the screen and moves text cursor to ORIGIN
int clearScreen() {
  ORIGIN;
  Serial.write(0x45);
  HANDLE_ACK;
}



void setup() {
  randomSeed(analogRead(0));
  
  Serial.begin(19200);
  pinMode(LED, OUTPUT);

  // send init byte (55h) to auto-set baud on display
  Serial.write(0x55);
  
  // wait for ack (06h)
  while (!Serial.available()) ;
  received_byte = Serial.read();
  if (received_byte != 0x06) {
    while (true) {
      digitalWrite(LED, 1);
      delay(500);
      digitalWrite(LED, 0);
      delay(500);
    }
  }
  
  setFont(FONT);
  setBg(BGCOLOR);
}

void loop() {
  drawDicks();
  delay(1500);
  clearScreen();
}


