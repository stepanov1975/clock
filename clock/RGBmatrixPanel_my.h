#ifndef RGBMATRIXPANEL_MY_H_INCLUDED
#define RGBMATRIXPANEL_MY_H_INCLUDED
#endif // RGBMATRIXPANEL_MY_H_INCLUDED
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
 #include "pins_arduino.h"
#endif
#include "Adafruit_GFX.h"

class RGBmatrixPanel_my  {

 public:

  // Constructor for 16x32 panel:
  RGBmatrixPanel_my(uint8_t a, uint8_t b, uint8_t c,
    uint8_t sclk, uint8_t latch, uint8_t oe, boolean dbuf);

  // Constructor for 32x32 panel (adds 'd' pin):
  RGBmatrixPanel_my(uint8_t a, uint8_t b, uint8_t c, uint8_t d,
    uint8_t sclk, uint8_t latch, uint8_t oe, boolean dbuf, uint8_t width=32);

  void
    begin(void),
    drawPixel(uint8_t x, uint8_t y, uint8_t r, uint8_t g,uint8_t b),
    cleanScreen(),
    updateDisplay(void),
    swapBuffers(boolean),
    dumpMatrix(void);
  uint8_t
	_width,
	WIDTH,
	_height,
	HEIGHT,
    *backBuffer(void);

 private:

  uint8_t         *matrixbuff[2];
  uint8_t          nRows;
  volatile uint8_t backindex;
  volatile boolean swapflag;
  boolean upsidedown=0,flip=0;

  // Init/alloc code common to both constructors:
  void init(uint8_t rows, uint8_t a, uint8_t b, uint8_t c,
	    uint8_t sclk, uint8_t latch, uint8_t oe, boolean dbuf,
	    uint8_t width);

  // PORT register pointers, pin bitmasks, pin numbers:
  volatile uint8_t
    *latport, *oeport, *addraport, *addrbport, *addrcport, *addrdport;
  uint8_t
    sclkpin, latpin, oepin, addrapin, addrbpin, addrcpin, addrdpin,
    _sclk, _latch, _oe, _a, _b, _c, _d;

  // Counters/pointers for interrupt handler:
  volatile uint8_t row, plane;
  volatile uint8_t *buffptr;
};

