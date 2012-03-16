/* arduinbros.ino
   a port of rainbros (dicks + lolcat) for the Arduino and GOLDELOX display
   march 2012  pete gamache  pete@#$!@#gamache.org
    ,
   (k) all rights reversed
*/


// pins and byte buffer
#define RX  0
#define TX  1
#define LED 13
byte received_byte;

// dicks
char *dicks[8] = {
  " 8===D",
  " 8=====D",
  " 8======D",
  " 8=======D",
  " 8========D",
  " 8==========D",
  " 8============D",
  " 8==============D"
};
#define dickfor(N) dicks[(N) & 0x7]

// here we define our sequence of rainbow colors
byte rainbow_r[7] = {0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x4B, 0xEE};
byte rainbow_g[7] = {0x00, 0xA5, 0xFF, 0xFF, 0x00, 0x00, 0x82};
byte rainbow_b[7] = {0x00, 0x00, 0x00, 0x00, 0xFF, 0x82, 0xEE};
int _rainbow = 0; // starting offset from RED for first char of a line

// GOLDELOX text stuff
#define FONT 0     // 5x7
#define MAXROW 8
#define MAXCOL 20
byte _row=0, _col=0;   // current text position
#define newline _row++; _col=0; _rainbow++;
#define origin  _row=0; _col=0; _rainbow++;

// macros to pack R, G, B data into two bytes, gb and gb
#define rg(R,G,B) (((R) & 0x1f)<<3 | (((G)>>3) & 0x07))
#define gb(R,G,B) (((G) & 0x07)<<5 | ((B) & 0x1f))

// this macro handles the ack byte returned by the GOLDELOX.
// returns 1 on success (ACK), 0 on error (NAK)
#define handle_ack \
        while (! Serial.available()) ; return Serial.read()==0x06 ? 1 : 0;


// drawChar prints a single character at the current position, 
// in the specified color.  Advances the current position (_row, _col).
int drawChar(char c, byte r, byte g, byte b) {
  Serial.write(0x54);
  Serial.write(c);
  if (_col >= MAXCOL) { newline; }
  Serial.write(_col++);
  Serial.write(_row);
  Serial.write(rg(r,g,b));
  Serial.write(gb(r,g,b));
  handle_ack;
}

// drawString prints an ASCII string at the current position, with newline.
// Color is in a rainbow fashion, a la lolcat.
void drawString(char *str) {
  char c;
  int color;
  color = _rainbow;

  while (c = *str++) {
    drawChar(c, rainbow_r[color], rainbow_g[color], rainbow_b[color]);
    color = (color + 1) % 7;
  }
  newline;
}

// draw a screenful of dicks
void drawDicks() {
  for (int row=0; row < MAXROW; row++) {
    drawString(dickfor(random(0,7))); 
  }
}

// set the screen font (yup)
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

int clear_screen() {
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
}

void loop() {
  setBg(0, 0, 0);
  drawDicks();
  delay(2500);
  clear_screen();
  origin;
}


