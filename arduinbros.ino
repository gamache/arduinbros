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
  "8D",              // idx = 0, len = 2.  CHOAD.
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
#define dickfor(N) dicks[(N) & 0xF]

// here we define our sequence of rainbow colors
byte rainbow_r[7] = {0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x4B, 0xEE};
byte rainbow_g[7] = {0x00, 0xA5, 0xFF, 0xFF, 0x00, 0x00, 0x82};
byte rainbow_b[7] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0x82, 0xEE};
int _rainbow = 0; // starting offset from RED for first char of a line
// rainbow_rgb returns the r, g, b bytes for the rainbow color N=0..6
#define rainbow_rgb(N) rainbow_r[(N)], rainbow_g[(N)], rainbow_b[(N)]

// GOLDELOX text stuff
#define FONT 0     // 5x7
#define NROWS 8
#define NCOLS 16
#define BGCOLOR 0,0,0
byte _row=0, _col=0;   // current text cursor position
#define advance_rainbow  if (_rainbow==6) _rainbow=0; else _rainbow++;
#define newline _row++; _col=0; advance_rainbow;
#define origin  _row=0; _col=0; advance_rainbow;

// macros to pack byte R, G, B data into two bytes, rg and gb
#define rg(R,G,B) (((R) & 0xf8) | ((G) >> 5))
#define gb(R,G,B) (((G) << 5) | ((B) >> 3))

// this macro handles the ack byte returned by the GOLDELOX.
// returns 1 on success (ACK), 0 on error (NAK)
#define handle_ack \
        while (! Serial.available()) ; return Serial.read()==0x06 ? 1 : 0;


// drawChar prints a single character at the current position, 
// in the specified color.  Advances the current position (_row, _col).
int drawChar(char c, byte r, byte g, byte b) {
  Serial.write(0x54);
  Serial.write(c);
  if (_col >= NCOLS) { newline; }
  Serial.write(_col++);
  Serial.write(_row);
  Serial.write(rg(r,g,b));
  Serial.write(gb(r,g,b));
  handle_ack;
}

// drawString prints an ASCII string at the current position, with newline.
// Color is in a rainbow fashion, a la lolcat.
void drawString(char *str) {
  int color;
  color = _rainbow;

  while (*str) {
    drawChar(*str++, rainbow_rgb(color));
    if (++color == 7) color = 0; // like color = ++color % 7, but cheaper
  }
  newline;
}

// draw a screenful of dicks
void drawDicks() {
  for (int row=0; row < NROWS; row++) {
    drawString(dickfor(random(0,15))); 
  }
}

// sets the screen font (yup)
int setFont(byte f) {
  Serial.write(0x46);
  Serial.write(f);
  handle_ack;
}

// changes screen background color
int setBg(int r, int g, int b) {
  Serial.write(0x42);
  Serial.write(rg(r,g,b));
  Serial.write(gb(r,g,b));
  handle_ack;
}

// clears the screen and moves text cursor to origin
int clearScreen() {
  origin;
  Serial.write(0x45);
  handle_ack;
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
  delay(2500);
  clearScreen();
}


