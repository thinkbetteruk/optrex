
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



class optrexDMCLCD
{
  public:
  optrexDMCLCD (byte dataPin, byte rwPin, byte rsPin, byte enPin, byte interface_size);
  void init(void);

  void clearLCD(void);
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
  void setInterfaceSize(byte s);
  void setDisplayLines(byte lines);
  void setCharacterFont5x7(void);
  void setCharacterFont5x10(void);
  void setCGAddress(byte addr);
  void setDDAddress(byte addr);
  void gotoXY(byte x, byte y);
  void print(const char *szBuf);  

  private:
  void write(byte reg, byte value);
  byte read(byte reg);
  void writeNibble(byte reg, byte value);
  void setReadMode(void);
  void setWriteMode(void);
  void waitForReady(void);

  void setFunction(void);


  byte _entryModeRegister;
  byte _displayOnOffRegister;
  byte _setFunctionRegister;
  
 // byte cursorMove=OPTREX_INCREMENT;
  //byte autoShift=OPTREX_AUTOSHIFT_ON;
  //byte isBlinkOn=OPTREX_BLINK_OFF;
  //byte isCursorOn=OPTREX_CURSOR_OFF;
  //byte isDisplayOn=OPTREX_DISPLAY_ON;

 
  byte rw_pin;
  byte rs_pin;
  byte en_pin;
  byte d0_pin;
  byte _dataDirection=INPUT;
  byte _interfaceSize;
};


void optrexDMCLCD::waitForReady(void)
{
  while ((this->read(OPTREX_INSTRUCTION_REGISTER) & B10000000 == 0))
    delayMicroseconds(1);
}

void optrexDMCLCD::writeNibble(byte reg, byte nib)
{
  digitalWrite(rs_pin, reg);
  digitalWrite(rw_pin, LOW);
  digitalWrite(en_pin, HIGH);

  digitalWrite (d0_pin + 7, bitRead(nib, 3));
  digitalWrite (d0_pin + 6, bitRead(nib, 2));
  digitalWrite (d0_pin + 5, bitRead(nib, 1));
  digitalWrite (d0_pin + 4, bitRead(nib, 0));
  delayMicroseconds(1);  
    
  digitalWrite (en_pin, LOW);
}

byte optrexDMCLCD::read(byte reg)
{
  byte v;
  
  setReadMode();
  
  digitalWrite(rs_pin, reg);
  digitalWrite(rw_pin, HIGH);
  digitalWrite(en_pin, HIGH);

  // To be improved to support 4 bit interface
  if (_interfaceSize == OPTREX_8BIT)
  {
    bitWrite(v, 0, digitalRead (d0_pin + 0));
    bitWrite(v, 1, digitalRead (d0_pin + 1));
    bitWrite(v, 2, digitalRead (d0_pin + 2));
    bitWrite(v, 3, digitalRead (d0_pin + 3));
  }
  bitWrite(v, 4, digitalRead (d0_pin + 4));
  bitWrite(v, 5, digitalRead (d0_pin + 5));
  bitWrite(v, 6, digitalRead (d0_pin + 6));
  bitWrite(v, 7, digitalRead (d0_pin + 7));

  delayMicroseconds(1);  
    
  digitalWrite (en_pin, LOW);
  return v;
}

void optrexDMCLCD::setWriteMode()
{
  if (_dataDirection != OUTPUT)
  {
    for (byte i=0; i<((_interfaceSize==OPTREX_4BIT) ? 4 : 8); i++)
      pinMode(d0_pin + i, OUTPUT);
    _dataDirection = OUTPUT; 
  }
}

void optrexDMCLCD::setReadMode()
{
  if (_dataDirection != INPUT)
  {
    for (byte i=0; i<((_interfaceSize==OPTREX_4BIT) ? 4 : 8); i++)
      pinMode(d0_pin + i, INPUT);
    _dataDirection = INPUT; 
  }
}


optrexDMCLCD::optrexDMCLCD(byte rsPin, byte rwPin, byte enPin, byte dataPin, byte interfaceSize)
{

  d0_pin = dataPin;
  rw_pin = rwPin;
  rs_pin = rsPin;
  en_pin = enPin;
  _interfaceSize = interfaceSize;
}

void optrexDMCLCD::init(void)
{
  pinMode(rw_pin, OUTPUT);
  pinMode(rs_pin, OUTPUT);
  pinMode(en_pin, OUTPUT);

#ifdef DEBUG_LCD
  Serial.begin(9600);
  Serial.println("Setting write mode");
#endif

  setWriteMode();

  delayMicroseconds(15000);                      // initial delay after power-up (probably not necessary)

  // Send Set Command 0B0011
  writeNibble (OPTREX_INSTRUCTION_REGISTER, B0011);
  delayMicroseconds(5000);

  // Send Set Command
  writeNibble (OPTREX_INSTRUCTION_REGISTER, B0011);
  delayMicroseconds(101);

  // Send Set Command
  writeNibble (OPTREX_INSTRUCTION_REGISTER, B0011);
#ifdef DEBUG_LCD
  Serial.println("3rd set command processed");
#endif
  // Set the interface to the requested size
  waitForReady();
  this->setInterfaceSize(_interfaceSize);

#ifdef DEBUG_LCD
  Serial.println("Interface size set");
#endif

  
  // Clear display
  this->clearLCD();
#ifdef DEBUG_LCD
  Serial.println("ClearDisplay done");
#endif

  this->setDisplayOff();
#ifdef DEBUG_LCD
  Serial.println("SetDisplayOff done");
#endif

  this->setDisplayOn();
#ifdef DEBUG_LCD
  Serial.println("SetDisplayOn done");
#endif
}

// CLEAR DISPLAY COMMAND
void optrexDMCLCD::clearLCD(void)
{
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_CLEAR_DISPLAY);
  delayMicroseconds(1640);
}

// RETURN HOME COMMAND
void optrexDMCLCD::goToHome(void)
{
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_HOME);
  delayMicroseconds(1640);
}

// ENTRY MODE COMMANDS
void optrexDMCLCD::setCursorAutoMoveLeft(void)
{
  _entryModeRegister &= ~OPTREX_INCREMENT;
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setCursorAutoMoveRight(void)
{
  _entryModeRegister |= OPTREX_INCREMENT;
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setAutoShiftOn(void)
{
  waitForReady();
  _entryModeRegister |= OPTREX_AUTOSHIFT_ON;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setAutoShiftOff(void)
{
  waitForReady();
  _entryModeRegister &= ~OPTREX_AUTOSHIFT_ON;
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_ENTRY_MODE | _entryModeRegister);
  delayMicroseconds(40);
}

// DISPLAY / CURSOR ON/OFF CONTROL
void optrexDMCLCD::setDisplayOn(void)
{
  _displayOnOffRegister |= OPTREX_DISPLAY_ON;
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setDisplayOff(void)
{
  _displayOnOffRegister &= ~OPTREX_DISPLAY_ON;
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setCursorOn(void)
{
  _displayOnOffRegister |= OPTREX_CURSOR_ON;
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setCursorOff(void)
{
  _displayOnOffRegister &= ~OPTREX_CURSOR_ON;
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setBlinkOn(void)
{
  _displayOnOffRegister |= OPTREX_BLINK_ON;
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setBlinkOff(void)
{
  _displayOnOffRegister &= ~OPTREX_BLINK_ON;
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_DISPLAY_ONOFF | _displayOnOffRegister);
  delayMicroseconds(40);
}


// CURSOR / DISPLAY SHIFT COMMANDS
void optrexDMCLCD::shiftCursorLeft(void)
{
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | OPTREX_SHIFT_CURSOR | OPTREX_SHIFT_LEFT);
  delayMicroseconds(40);
}

void optrexDMCLCD::shiftCursorRight(void)
{
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | OPTREX_SHIFT_CURSOR | OPTREX_SHIFT_RIGHT);
  delayMicroseconds(40);
}

void optrexDMCLCD::shiftDisplayLeft(void)
{
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | OPTREX_SHIFT_DISPLAY | OPTREX_SHIFT_LEFT);
  delayMicroseconds(40);
}

void optrexDMCLCD::shiftDisplayRight(void)
{
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SHIFT | OPTREX_SHIFT_DISPLAY | OPTREX_SHIFT_RIGHT);
  delayMicroseconds(40);
}

// FUNCTION SET COMMANDS
void optrexDMCLCD::setInterfaceSize(byte s)
{
  switch (s)
  {
    case 4:
      _setFunctionRegister &= ~OPTREX_8BIT;
      _interfaceSize=4;

    case 8:
      _setFunctionRegister |= OPTREX_8BIT;
      _interfaceSize=8;

    default:
#ifdef DEBUG_LCD
      Serial.println ("Incorrect argument in optrexDMCLCD::setInterfaceSize");
#endif
      return;
  }

  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_FUNCTIONSET | _setFunctionRegister);
  delayMicroseconds(40);
}


void optrexDMCLCD::setDisplayLines(byte lines)
{
  switch (lines)
  {
    case 1: 
      _setFunctionRegister &= ~OPTREX_2_LINE;
      break;

    case 2:
      _setFunctionRegister |= OPTREX_2_LINE;
      break;

    default: 
#ifdef DEBUG_LCD
      Serial.println ("Invalid argument in optrexDMCLCD::setDisplayLines");
#endif
      return;
  }
  
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_FUNCTIONSET | _setFunctionRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setCharacterFont5x7(void)
{
  _setFunctionRegister &= ~OPTREX_5x10;

  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_FUNCTIONSET | _setFunctionRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setCharacterFont5x10(void)
{
  _setFunctionRegister |= OPTREX_5x10;

  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_FUNCTIONSET | _setFunctionRegister);
  delayMicroseconds(40);
}

void optrexDMCLCD::setCGAddress(byte addr)
{
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SET_CGADDR | (addr & B00111111));
  delayMicroseconds(42);
}

void optrexDMCLCD::setDDAddress(byte addr)
{
  waitForReady();
  this->write(OPTREX_INSTRUCTION_REGISTER, OPTREX_SET_DDADDR | (addr & B01111111));
  delayMicroseconds(42);
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
  }
}

void optrexDMCLCD::write(byte reg, byte data)
{
  setWriteMode();
 
  if (_interfaceSize == 8)
  {
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

#ifdef DEBUG_LCD
    Serial.print(data); Serial.println(" written"); 
#endif
  } 
  else
  {
    writeNibble (rs_pin, (data & 0xF0) >>4);
    delayMicroseconds(50);
    writeNibble (rs_pin, (data & 0x0F));
  }

  delayMicroseconds(50);
}

// optrexDMCLCD (byte rsPin, byte rwPin, byte enPin, byte dataPin, byte enPin, byte interface_size);
optrexDMCLCD myLCD (3, 4, 5, 6, 8);


// Test
void setup() {
  Serial.begin(9600);
  Serial.println ("Entering setup()");
  myLCD.init();
  myLCD.setDisplayLines(2);


  // put your setup code here, to run once:
  myLCD.clearLCD();
  myLCD.goToHome();

  Serial.println ("LCD initialised");
}

int i=0;
char szBuf[16];

void loop() {
  myLCD.clearLCD();
  myLCD.goToHome();

  sprintf (szBuf, "%i", i);
  myLCD.print(szBuf);
  
  delay(1000);

  i++;
}
