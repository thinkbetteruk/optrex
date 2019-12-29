#define DEBUG_LCD


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
#define OPTREX_INCREMENT        B00000010
#define OPTREX_DECREMENT        B00000000
#define OPTREX_AUTOSHIFT_ON     B00000001
#define OPTREX_AUTOSHIFT_OFF    B00000000

// Constants for Display On/Off
#define OPTREX_DISPLAY_ON       B00000100
#define OPTREX_DISPLAY_OFF      B00000000
#define OPTREX_CURSOR_ON        B00000010
#define OPTREX_CURSOR_OFF       B00000000
#define OPTREX_BLINK_ON         B00000001
#define OPTREX_BLINK_OFF        B00000000

// Constants for Cursor / Display shift command
#define OPTREX_SHIFT_CURSOR     B00000000
#define OPTREX_SHIFT_DISPLAY    B00001000
#define OPTREX_SHIFT_LEFT       B00000000
#define OPTREX_SHIFT_RIGHT      B00000100

// Constants for Set Function command
#define OPTREX_1_LINE           B00000000
#define OPTREX_2_LINE           B00001000
#define OPTREX_5x10             B00000100
#define OPTREX_5x7              B00000000
#define OPTREX_4BIT             B00000000
#define OPTREX_8BIT             B00010000



class optrexLCD
{
  public:
  optrexLCD(byte rsPin, byte rwPin, byte enPin, byte dataPin, byte interfaceSize, byte numLines, byte fontSize);
  void init(void);

  void clearDisplay(void);
  void goToHome(void);
  void setCursorAutoMoveLeft(void);
  void setCursorAutoMoveRight(void);
  void setAutoShiftOn(void);
  void setAutoShiftOff(void);
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
  void setCGAddress(byte addr);
  void setDDAddress(byte addr);
  void gotoXY(byte x, byte y);
  void print(const char *szBuf);  

  private:
  void rawWriteByte(byte reg, byte value);
  void writeByte(byte reg, byte data);
  void rawWriteNibble(byte reg, byte start_pin, byte nib);
  void writeNibble(byte reg, byte start_pin, byte nib);
  byte readByte(byte reg);
  byte readNibble(byte reg, byte start_pin);
  void setReadMode(void);
  void setWriteMode(void);
  void waitForReady(void);

  byte _entryModeRegister;
  byte _displayOnOffRegister;
  byte _functionRegister;
  
  byte rw_pin;
  byte rs_pin;
  byte en_pin;
  byte d0_pin;
  byte _dataDirection=INPUT;
  byte _interfaceSize;
};


void optrexLCD::waitForReady(void)
{
  setReadMode();
  if (_interfaceSize == 8)
  {  
    while ((readByte(OPTREX_INSTRUCTION_REGISTER) & B10000000 == 0))
      delayMicroseconds(1);
  }
  else
  {
    while ((readByte(OPTREX_INSTRUCTION_REGISTER) & B10000000 == 0))
      delayMicroseconds(1);
  }
}

void optrexLCD::rawWriteByte(byte reg, byte data)
{
  if (_interfaceSize == 8)
  {
    setWriteMode();
    digitalWrite(rs_pin, reg);
    delayMicroseconds(1);    
    digitalWrite(rw_pin, LOW);
    delayMicroseconds(1);    
    digitalWrite(en_pin, HIGH);
    delayMicroseconds(1);    

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
    delayMicroseconds(1);    
    digitalWrite (rw_pin, HIGH);
    delayMicroseconds(1);   

  } 
  else
  {
    waitForReady();
    setWriteMode();
    rawWriteNibble (reg, d0_pin, data>>4);

//    waitForReady();
//    setWriteMode();
    rawWriteNibble (reg, d0_pin, data);
  }
#ifdef DEBUG_LCD
    Serial.print(data); Serial.println(" written"); 
#endif
}

void optrexLCD::writeByte(byte reg, byte data)
{
  waitForReady();
  rawWriteByte(reg, data);
}

void optrexLCD::rawWriteNibble(byte reg, byte start_pin, byte nib)
{
  digitalWrite(rs_pin, reg);
  digitalWrite(rw_pin, LOW);
  digitalWrite(en_pin, HIGH);

  digitalWrite (start_pin + 3, bitRead(nib, 3));
  digitalWrite (start_pin + 2, bitRead(nib, 2));
  digitalWrite (start_pin + 1, bitRead(nib, 1));
  digitalWrite (start_pin + 0, bitRead(nib, 0));
  delayMicroseconds(1);  
    
  digitalWrite (en_pin, LOW);
}

void optrexLCD::writeNibble(byte reg, byte start_pin, byte nib)
{
  waitForReady();
  rawWriteNibble(reg, start_pin, nib);
}

byte optrexLCD::readByte(byte reg)
{
  byte v;
  
  setReadMode();
  
  digitalWrite(rs_pin, reg);
  digitalWrite(rw_pin, HIGH);
  digitalWrite(en_pin, HIGH);

  if (_interfaceSize == 8)
  {
    bitWrite(v, 0, digitalRead (d0_pin + 0));
    bitWrite(v, 1, digitalRead (d0_pin + 1));
    bitWrite(v, 2, digitalRead (d0_pin + 2));
    bitWrite(v, 3, digitalRead (d0_pin + 3));
    bitWrite(v, 4, digitalRead (d0_pin + 4));
    bitWrite(v, 5, digitalRead (d0_pin + 5));
    bitWrite(v, 6, digitalRead (d0_pin + 6));
    bitWrite(v, 7, digitalRead (d0_pin + 7));

    delayMicroseconds(1);  
    
    digitalWrite (en_pin, LOW);
  }
  else
  {
    bitWrite(v, 4, digitalRead (d0_pin + 0));
    bitWrite(v, 5, digitalRead (d0_pin + 1));
    bitWrite(v, 6, digitalRead (d0_pin + 2));
    bitWrite(v, 7, digitalRead (d0_pin + 3));
    delayMicroseconds(1);  
    digitalWrite (en_pin, LOW);

    delayMicroseconds(50);
    digitalWrite(en_pin, HIGH);
    bitWrite(v, 0, digitalRead (d0_pin + 0));
    bitWrite(v, 1, digitalRead (d0_pin + 1));
    bitWrite(v, 2, digitalRead (d0_pin + 2));
    bitWrite(v, 3, digitalRead (d0_pin + 3));
    delayMicroseconds(1);  
    digitalWrite (en_pin, LOW);
  }
  return v;
}

// Read a nibble from register reg, LSB at start_pin
byte optrexLCD::readNibble(byte reg, byte start_pin)
{
  byte v=0;
  
  setReadMode();
  
  digitalWrite(rs_pin, reg);
  digitalWrite(rw_pin, HIGH);
  digitalWrite(en_pin, HIGH);

  bitWrite(v, 0, digitalRead (start_pin + 0));
  bitWrite(v, 1, digitalRead (start_pin + 1));
  bitWrite(v, 2, digitalRead (start_pin + 2));
  bitWrite(v, 3, digitalRead (start_pin + 3));
  delayMicroseconds(1);  
  digitalWrite (en_pin, LOW);
  return v;
}

// Set the data pins to write mode
void optrexLCD::setWriteMode()
{
  if (_dataDirection != OUTPUT)
  {
    for (byte i=0; i<_interfaceSize; i++)
      pinMode(d0_pin + i, OUTPUT);
    _dataDirection = OUTPUT; 
  }
}

// Set the data pins to read mode
void optrexLCD::setReadMode()
{
  if (_dataDirection != INPUT)
  {
    for (byte i=0; i<_interfaceSize; i++)
      pinMode(d0_pin + i, INPUT);
    _dataDirection = INPUT; 
  }
}


optrexLCD::optrexLCD(byte rsPin, byte rwPin, byte enPin, byte dataPin, byte interfaceSize, byte numLines=OPTREX_2_LINE, byte fontSize=OPTREX_5x10)
{

  d0_pin = dataPin;
  rw_pin = rwPin;
  rs_pin = rsPin;
  en_pin = enPin;
  _interfaceSize = interfaceSize;
  _functionRegister = (numLines | fontSize);
  _entryModeRegister = OPTREX_INCREMENT;
}



void optrexLCD::init(void)
{
  pinMode(rw_pin, OUTPUT);
  pinMode(rs_pin, OUTPUT);
  pinMode(en_pin, OUTPUT);

#ifdef DEBUG_LCD
  Serial.begin(9600);
  Serial.println("Initialising display");
#endif

  setWriteMode();

  delayMicroseconds(15000);                      // initial delay after power-up (probably not necessary)

  // Send Set Command 0B0011
  rawWriteNibble (OPTREX_INSTRUCTION_REGISTER, d0_pin + _interfaceSize - 4, B0011);
  delayMicroseconds(5000);

  // Send Set Command
  rawWriteNibble (OPTREX_INSTRUCTION_REGISTER, d0_pin + _interfaceSize - 4, B0011);
  delayMicroseconds(101);

  // Send Set Command
  rawWriteNibble (OPTREX_INSTRUCTION_REGISTER, d0_pin + _interfaceSize - 4, B0011);

  // Set the interface to the requested size
  if (_interfaceSize == 4)
  {
    // Set interface to 4 bits, 
    writeNibble(OPTREX_INSTRUCTION_REGISTER, d0_pin + _interfaceSize - 4, B0010);

    // Function set - number of lines and character font
    writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_FUNCTIONSET | 0);  // If I set the Function Set register to OPTREX_2_LINE | OPTREX_5x10 I get only 1 line on the display
    setDisplayOff();
    clearDisplay();

    // Entry mode set
    writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);

    setDisplayOn();
  }
  else
  {
    setDisplayOff();
    clearDisplay();

    // Set interface to 8 bits
    writeByte(OPTREX_INSTRUCTION_REGISTER, B00000111);  

    setDisplayOff();
    clearDisplay();

    // Entry mode set
    writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);

    setDisplayOn();
  }
#ifdef DEBUG_LCD
  Serial.println ("Init complete");
#endif
}

// CLEAR DISPLAY COMMAND
void optrexLCD::clearDisplay(void)
{
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_CLEAR_DISPLAY);
  delayMicroseconds(1640);
}

// RETURN HOME COMMAND
void optrexLCD::goToHome(void)
{
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_HOME);
  delayMicroseconds(1640);
}

// ENTRY MODE COMMANDS
void optrexLCD::setCursorAutoMoveLeft(void)
{
  _entryModeRegister &= ~OPTREX_INCREMENT;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);
  delayMicroseconds(40);
}

void optrexLCD::setCursorAutoMoveRight(void)
{
  _entryModeRegister |= OPTREX_INCREMENT;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);
  delayMicroseconds(40);
}

void optrexLCD::setAutoShiftOn(void)
{
  _entryModeRegister |= OPTREX_AUTOSHIFT_ON;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);
  delayMicroseconds(40);
}

void optrexLCD::setAutoShiftOff(void)
{
  _entryModeRegister &= ~OPTREX_AUTOSHIFT_ON;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);
  delayMicroseconds(40);
}

// DISPLAY / CURSOR ON/OFF CONTROL
void optrexLCD::setDisplayOn(void)
{
  _displayOnOffRegister |= OPTREX_DISPLAY_ON;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexLCD::setDisplayOff(void)
{
  _displayOnOffRegister &= ~OPTREX_DISPLAY_ON;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexLCD::setCursorOn(void)
{
  _displayOnOffRegister |= OPTREX_CURSOR_ON;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexLCD::setCursorOff(void)
{
  _displayOnOffRegister &= ~OPTREX_CURSOR_ON;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexLCD::setBlinkOn(void)
{
  _displayOnOffRegister |= OPTREX_BLINK_ON;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexLCD::setBlinkOff(void)
{
  _displayOnOffRegister &= ~OPTREX_BLINK_ON;
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}


// CURSOR / DISPLAY SHIFT COMMANDS
void optrexLCD::shiftCursorLeft(void)
{
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | OPTREX_SHIFT_CURSOR | OPTREX_SHIFT_LEFT);
  delayMicroseconds(40);
}

void optrexLCD::shiftCursorRight(void)
{
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | OPTREX_SHIFT_CURSOR | OPTREX_SHIFT_RIGHT);
  delayMicroseconds(40);
}

void optrexLCD::shiftDisplayLeft(void)
{
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | OPTREX_SHIFT_DISPLAY | OPTREX_SHIFT_LEFT);
  delayMicroseconds(40);
}

void optrexLCD::shiftDisplayRight(void)
{
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | OPTREX_SHIFT_DISPLAY | OPTREX_SHIFT_RIGHT);
  delayMicroseconds(40);
}

void optrexLCD::setCGAddress(byte addr)
{
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_SET_CGADDR | (addr & B00111111));
  delayMicroseconds(42);
}

void optrexLCD::setDDAddress(byte addr)
{
  writeByte(OPTREX_INSTRUCTION_REGISTER, OPTREX_SET_DDADDR | (addr & B01111111));
  delayMicroseconds(42);
}


void optrexLCD::gotoXY(byte x, byte y)
{
  setDDAddress (x + 0x40*y);
}


void optrexLCD::print(const char *buf)
{
  byte i;

  for (i=0; i<strlen(buf); i++)
  {
    writeByte(OPTREX_DATA_REGISTER, buf[i]);
  }
}


// optrexLCD(byte rsPin, byte rwPin, byte enPin, byte dataPin, byte interfaceSize, byte numLines, byte fontSize)
optrexLCD myLCD (3, 4, 5, 10, 4);


// Test
void setup() {
  Serial.begin(9600);
  Serial.println ("Entering setup()");
  myLCD.init();


  // put your setup code here, to run once:
  myLCD.clearDisplay();
  myLCD.goToHome();

  Serial.println ("LCD initialised");
}

int i=0;
char szBuf[16];

void loop() {
  sprintf (szBuf, "%i", i);

  myLCD.gotoXY(0, i%2);
  myLCD.print(szBuf);
  
  delay(1000);
#ifdef DEBUG_LCD
  Serial.print ("Written "); Serial.println (szBuf);
#endif
  i++;
}
