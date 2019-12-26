
#define OPTREX_INSTRUCTION_REGISTER 0
#define OPTREX_DATA_REGISTER 1

// Commands
#define OPTREX_CLEAR_DISPLAY    B00000001
#define OPTREX_HOME             B00000010
#define OPTREX_ENTRY_MODE       B00000100
#define OPTREX_DISPLAY_ONOFF    B00001000 
#define OPTREX_SHIFT            B00010000
#define OPTREX_FUNCTIONSET      B00100000
#define OPTREX_SET_CGADDR       B01000000
#define OPTREX_SET_DDADDR       B10000000

// Constants for Entry Mode set command
#define OPTREX_INCREMENT        B0010
#define OPTREX_DECREMENT        B0000
#define OPTREX_AUTOSHIFT_ON     B0001
#define OPTREX_AUTOSHIFT_OFF    B0000

// Constants for Display On/Off
#define OPTREX_DISPLAY_ON       B0100
#define OPTREX_DISPLAY_OFF      B0000
#define OPTREX_CURSOR_ON        B0010
#define OPTREX_CURSOR_OFF       B0000
#define OPTREX_BLINK_ON         B0001
#define OPTREX_BLINK_OFF        B0000

// Constants for Cursor / Display shift command
#define SHIFT_CURSOR_LEFT       B0000
#define SHIFT_CURSOR_RIGHT      B0100
#define SHIFT_DISPLAY_LEFT      B1000
#define SHIFT_DISPLAY_RIGHT     B1100

// Constants for Set Function command
#define OPTREX_1_LINE           B00000000
#define OPTREX_2_LINE           B00001000
#define OPTREX_5x10             B00000100
#define OPTREX_5x7              B00000000
#define OPTREX_4BIT             B00000000
#define OPTREX_8BIT             B00010000




class optrexDMCLCD
{
  public:
  optrexDMCLCD (byte dataPin, byte rwPin, byte rsPin, byte enPin, byte interface_size);
  void init(byte dataPin, byte rwPin, byte rsPin, byte enPin, byte interface_size);

  void clear(void);
  void goToHome(void);
  void setCursorMoveDir(byte dir);
  void setAutoShift(byte v);
  void shiftCursorLeft(void);
  void shiftCursorRight(void);
  void shiftDisplayLeft(void);
  void shiftDisplayRight(void); 
  void setCursorOn(void);
  void setCursorOff(void);
  void setBlinkOn(void);
  void setBlinkOff(void);
  void setDisplayOn(void);
  void setDisplayOff(void);
  void setCharacterSet(byte i);
  void setCGAddress(byte addr);
  void setDDAddress(byte addr);
  void setInterfaceWidth(byte width);
  void setDisplayLines(byte n);
  void setCharacterFont(byte cf);
  void gotoXY(byte x, byte y);
  void print(const char *szBuf);  

  private:
  void write(byte register, byte value);
  void setFunction(void);


  byte cursorMove=OPTREX_INCREMENT;
  byte autoShift=OPTREX_AUTOSHIFT_ON;
  byte isBlinkOn=OPTREX_BLINK_OFF;
  byte isCursorOn=OPTREX_CURSOR_OFF;
  byte isDisplayOn=OPTREX_DISPLAY_ON;
  byte charFont=OPTREX_5x10;
  byte numLines=OPTREX_1_LINE;
  byte datasize=OPTREX_4BIT;
  byte rw_pin;
  byte rs_pin;
  byte en_pin;
  byte d0_pin;
};



optrexDMCLCD::optrexDMCLCD(byte dataPin, byte rwPin, byte rsPin, byte enPin, byte interfaceSize)
{
  delay(15);                      // initial delay after power-up (probably not necessary)
  d0_pin = dataPin;
  rw_pin = rwPin;
  rs_pin = rsPin;
  en_pin = enPin;

  pinMode(rw_pin, OUTPUT);
  pinMode(rs_pin, OUTPUT);
  pinMode(en_pin, OUTPUT);

  for (byte i=0; i< (interfaceSize==OPTREX_4BIT ? 4 : 8); i++)
    pinMode(d0_pin + i, OUTPUT);
  
  
  // Send Set Command
  this->write (OPTREX_INSTRUCTION_REGISTER, B0011);
  delay(5);

  // Send Set Command
  this->write (OPTREX_INSTRUCTION_REGISTER, B0011);
  delayMicroseconds(101);

  // Send Set Command
  this->write (OPTREX_INSTRUCTION_REGISTER, B0011);
  delayMicroseconds(10);

  // Set the interface to the requested size
  this->setInterfaceWidth(interfaceSize);
  
  // Clear display
  this->clear();
}

void optrexDMCLCD::setFunction(void)
{
  this->write(OPTREX_INSTRUCTION_REGISTER,OPTREX_FUNCTIONSET | charFont | numLines | datasize);
  delayMicroseconds(40);
}

// CLEAR DISPLAY COMMAND
void optrexDMCLCD::clear(void)
{
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_CLEAR_DISPLAY);
  delay(16);
}

// RETURN HOME COMMAND
void optrexDMCLCD::goToHome(void)
{
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_HOME);
  delay(16);
}

// ENTRY MODE COMMANDS
void optrexDMCLCD::setCursorMoveDir(byte dir)
{
  cursorMove = dir;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | cursorMove | autoShift);
  delayMicroseconds(40);
}


void optrexDMCLCD::setAutoShift(byte v)
{
  autoShift = v;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | cursorMove | autoShift);
  delayMicroseconds(40);
}


// DISPLAY / CURSOR ON/OFF CONTROL
void optrexDMCLCD::setDisplayOff(void)
{
  isDisplayOn = OPTREX_DISPLAY_OFF;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | isDisplayOn | isCursorOn | isBlinkOn);
  delayMicroseconds(40);
}

void optrexDMCLCD::setDisplayOn(void)
{
  isDisplayOn = OPTREX_DISPLAY_ON;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | isDisplayOn | isCursorOn | isBlinkOn);
}

void optrexDMCLCD::setCursorOn(void)
{
  isCursorOn = OPTREX_CURSOR_ON;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | isDisplayOn | isCursorOn | isBlinkOn);
  delayMicroseconds(40);  
}

void optrexDMCLCD::setCursorOff(void)
{
  isCursorOn = OPTREX_CURSOR_OFF;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | isDisplayOn | isCursorOn | isBlinkOn);
  delayMicroseconds(40);
}

void optrexDMCLCD::setBlinkOn(void)
{
  isBlinkOn = OPTREX_BLINK_ON;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | isDisplayOn | isCursorOn | isBlinkOn);
  delayMicroseconds(40);
}

void optrexDMCLCD::setBlinkOff(void)
{
  isBlinkOn = OPTREX_BLINK_OFF;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | isDisplayOn | isCursorOn | isBlinkOn);
  delayMicroseconds(40);
}


// CURSOR / DISPLAY SHIFT COMMANDS
void optrexDMCLCD::shiftCursorLeft(void)
{
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | SHIFT_CURSOR_LEFT);
  delayMicroseconds(40);
}

void optrexDMCLCD::shiftCursorRight(void)
{
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | SHIFT_CURSOR_RIGHT);
  delayMicroseconds(40);
}

void optrexDMCLCD::shiftDisplayLeft(void)
{
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | SHIFT_DISPLAY_LEFT);
  delayMicroseconds(40);
}

void optrexDMCLCD::shiftDisplayRight(void)
{
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | SHIFT_DISPLAY_RIGHT);
  delayMicroseconds(40);
}

// FUNCTION SET COMMANDS
void optrexDMCLCD::setInterfaceWidth(byte width)
{
  this->datasize=width;
  this->setFunction();
}

void optrexDMCLCD::setDisplayLines(byte n)
{
  this->numLines=n;
  this->setFunction();
}

void optrexDMCLCD::setCharacterFont(byte cf)
{
  this->charFont = cf;
  this->setFunction();
}

void optrexDMCLCD::setCGAddress(byte addr)
{
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SET_CGADDR);
  delayMicroseconds(40);
}

void optrexDMCLCD::setDDAddress(byte addr)
{
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SET_DDADDR);
  delayMicroseconds(40);
}


void optrexDMCLCD::gotoXY(byte x, byte y)
{
  setDDAddress (x + 0x40*y);
}


void optrexDMCLCD::print(const char *buf)
{
  byte i;

  for (i=0; i<strlen(buf); i++)
  {
    this->write(OPTREX_DATA_REGISTER, buf[i]);
    delayMicroseconds(40);
  }
}

void optrexDMCLCD::write(byte reg, byte data)
{
  if (datasize == OPTREX_8BIT)
  {
    digitalWrite(rs_pin, reg);
    digitalWrite(rw_pin, LOW);
    digitalWrite(en_pin, HIGH);

    digitalWrite (d0_pin + 7, bitRead (data, 7));
    digitalWrite (d0_pin + 6, bitRead (data, 6));
    digitalWrite (d0_pin + 5, bitRead (data, 5));
    digitalWrite (d0_pin + 4, bitRead (data, 4));
    digitalWrite (d0_pin + 3, bitRead (data, 3));
    digitalWrite (d0_pin + 2, bitRead (data, 2));
    digitalWrite (d0_pin + 1, bitRead (data, 1));
    digitalWrite (d0_pin + 0, bitRead (data, 0));
    delayMicroseconds(1);    
    digitalWrite (en_pin, LOW);
    digitalWrite (rw_pin, HIGH);
  }
  
  else
  {
    digitalWrite(rs_pin, reg);
    digitalWrite(rw_pin, LOW);
    digitalWrite(en_pin, HIGH);
    digitalWrite (d0_pin + 3, bitRead (data, 7));
    digitalWrite (d0_pin + 2, bitRead (data, 6));
    digitalWrite (d0_pin + 1, bitRead (data, 5));
    digitalWrite (d0_pin + 0, bitRead (data, 4));
    delayMicroseconds(1);    
    digitalWrite (en_pin, LOW);
    digitalWrite (rw_pin, HIGH);
    
    digitalWrite(rw_pin, LOW);
    digitalWrite(en_pin, HIGH);
    digitalWrite (d0_pin + 3, bitRead (data, 3));
    digitalWrite (d0_pin + 2, bitRead (data, 2));
    digitalWrite (d0_pin + 1, bitRead (data, 1));
    digitalWrite (d0_pin + 0, bitRead (data, 0));
    delayMicroseconds(1);    
    digitalWrite (en_pin, LOW);
    digitalWrite (rw_pin, HIGH);
  }
}

optrexDMCLCD myLCD (10, 8, 7, 9, OPTREX_4BIT);


// Test
void setup() {
  // put your setup code here, to run once:
  myLCD.clear();
  myLCD.gotoXY(0,0);
}

void loop() {
  char szBuf[16];
  
  // put your main code here, to run repeatedly:
  for(int i=0; ; i++)
  {
    myLCD.clear();
    myLCD.gotoXY(0,0);
    sprintf (szBuf, "%d", i);
    myLCD.print(szBuf);
    delay (500);
  }
}
