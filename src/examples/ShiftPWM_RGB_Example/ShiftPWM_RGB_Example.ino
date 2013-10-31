/************************************************************************************************************************************
 * Please go to www.elcojacobs.com/shiftpwm for documentation, fuction reference and schematics.
 * If you want to use ShiftPWM with LED strips or high power LED's, visit the shop for boards.
 ************************************************************************************************************************************/

// ** uncomment this part to NOT use the SPI port and change the pin numbers. This is 2.5x slower **
//#define SHIFTPWM_NOSPI
//const int ShiftPWM_dataPin = 3;  // Data pin is MOSI (Uno and earlier: 11, Leonardo: ICSP 4, Mega: 51, Teensy 2.0: 2, Teensy 2.0++: 22) 
//const int ShiftPWM_clockPin = 5; // Clock pin is SCK (Uno and earlier: 13, Leonardo: ICSP 3, Mega: 52, Teensy 2.0: 1, Teensy 2.0++: 21)


const int ShiftPWM_latchPin=10; // You can choose the latch pin yourself.
const bool ShiftPWM_invertOutputs = true; 
const bool ShiftPWM_balanceLoad = true;
#include "ShiftPWM.h"   // include ShiftPWM.h after setting the pins!

// Here you set the number of brightness levels, the update frequency and the number of shift registers.
// These values affect the load of ShiftPWM.
// Choose them wisely and use the PrintInterruptLoad() function to verify your load.
// There is a calculator on my website to estimate the load.



/****** DDUINO ******/
//#include <Servo.h>
bool debug = true;

int index = 0;

char messageBuffer[24];
//Servo servo;
/****** DDUINO ******/
void setup(){
  /****** DDUINO ******/
  Serial.begin(115200);
  /****** DDUINO ******/

  startShiftPWM("", "75;1", "");
}

void startShiftPWM(char *pin, char *val, char *aux){
  unsigned char maxBrightness = 255;
  unsigned char pwmFrequency;
  int numRegisters;

  char *str;
  int i = 0;
  while ((str = strtok_r(val, ";", &val)) != NULL) {
    if (i==0) pwmFrequency = atoi(str);
    else if(i==1) numRegisters = atoi(str);
    i++;
  }

  ShiftPWM.SetAmountOfRegisters(numRegisters);
  ShiftPWM.Start(pwmFrequency, maxBrightness);
}

void setPinRGB(char *pin, char *val){
  Serial.print("pin=");
  Serial.println(pin);
  Serial.print("val=");
  Serial.println(val);

  int r, g, b, i = 0;
  char *str;
  int p = atoi(pin);
  Serial.print("p=");
  Serial.println(p);

  while ((str = strtok_r(val, ";", &val)) != NULL) {
    //Serial.println(str);
    if (i==0) r = atoi(str);
    else if(i==1) g = atoi(str);
    else if(i==2) b = atoi(str);
    i++;
  }

  ShiftPWM.SetRGB(p,r,g,b);
}


void loop()
{    
  /****** DDUINO ******/
  while(Serial.available() > 0) {
    char x = Serial.read();
    if (x == '!') index = 0;      // start
    else if (x == '.') process(messageBuffer); // end
    else messageBuffer[index++] = x;
  }
  /****** DDUINO ******/
/*
  char k[] = "255;0;0";
  setPinRGB("0",k);
  delay(2000);

  char k1[] = "0;255;0";

  setPinRGB("0",k1);
  delay(2000);
  //return;*/
}



/****** DDUINO ******/
/*
 * Deal with a full message and determine function to call
 */
void process(char *messageBuffer) {
  index = 0;
  //char *val;
  char val[11];
  char aux[4];
  char cmd[3];
  char pin[3];

  strncpy(cmd, messageBuffer, 2);
  cmd[2] = '\0';
  strncpy(pin, messageBuffer + 2, 2);
  pin[2] = '\0';
  int cmdid = atoi(cmd);
  
  if (cmdid == 5) { //setPinRGB
    strncpy(val, messageBuffer+4, 11);
    Serial.print("val(0)=");
    Serial.println(val);
    val[11] = '\0';
    Serial.print("val(0)=");
    Serial.println(val);
  }
  else if (cmdid > 90) {
    strncpy(val, messageBuffer + 4, 2);
    val[2] = '\0';
    strncpy(aux, messageBuffer + 6, 3);
    aux[3] = '\0';
  }
  else {
    strncpy(val, messageBuffer + 4, 3);
    val[3] = '\0';
  }

    Serial.print("val(0)=");
    Serial.println(val);
  if (debug) {
    Serial.print("messageBuffer=");
    Serial.println(messageBuffer);
  }

    Serial.print("val(0)=");
    Serial.println(val);

    Serial.print("val(0)=");
    Serial.println(val);
  if (debug) {
    Serial.print("cmdid=");
    Serial.println(cmdid);
    Serial.print("val(1)=");
    Serial.println(val);
  }
  switch(cmdid) {
  case 0:  
    sm(pin,val);                  
    break;
  case 1:  
    dw(pin,val);                  
    break;
  case 2:  
    dr(pin,val);                  
    break;
  case 3:  
    aw(pin,val);                  
    break;
  case 4:  
    ar(pin,val);                  
    break;
  case 5:  
    setPinRGB(pin,val);           
    break;
  case 96: 
    startShiftPWM(pin,val,aux);   
    break;
  case 97: 
    handlePing(pin,val,aux);      
    break;
  case 98: 
    handleServo(pin,val,aux);     
    break;
  case 99: 
    toggleDebug(val);             
    break;
  default:                               
    break;
  }
}

/*
 * Toggle debug mode
 */
void toggleDebug(char *val) {
  if (atoi(val) == 0) {
    debug = false;
    Serial.println("goodbye");
  } 
  else {
    debug = true;
    Serial.println("hello");
  }
}

/*
 * Set pin mode
 */
void sm(char *pin, char *val) {
  if (debug) Serial.println("sm");
  int p = getPin(pin);
  if(p == -1) { 
    if(debug) Serial.println("badpin"); 
    return; 
  }
  if (atoi(val) == 0) {
    pinMode(p, OUTPUT);
  } 
  else {
    pinMode(p, INPUT);
  }
}

/*
 * Digital write
 */
void dw(char *pin, char *val) {
  if (debug) Serial.println("dw");
  int p = getPin(pin);
  if(p == -1) { 
    if(debug) Serial.println("badpin"); 
    return; 
  }
  pinMode(p, OUTPUT);
  if (atoi(val) == 0) {
    digitalWrite(p, LOW);
  } 
  else {
    digitalWrite(p, HIGH);
  }
}

/*
 * Digital read
 */
void dr(char *pin, char *val) {
  if (debug) Serial.println("dr");
  int p = getPin(pin);
  if(p == -1) { 
    if(debug) Serial.println("badpin"); 
    return; 
  }
  pinMode(p, INPUT);
  int oraw = digitalRead(p);
  char m[7];
  sprintf(m, "%02d::%02d", p,oraw);
  Serial.println(m);
}

/*
 * Analog read
 */
void ar(char *pin, char *val) {
  if(debug) Serial.println("ar");
  int p = getPin(pin);
  if(p == -1) { 
    if(debug) Serial.println("badpin"); 
    return; 
  }
  pinMode(p, INPUT); // don't want to sw
  int rval = analogRead(p);
  char m[8];
  sprintf(m, "%s::%03d", pin, rval);
  Serial.println(m);
}

void aw(char *pin, char *val) {
  if(debug) Serial.println("aw");
  int p = getPin(pin);
  pinMode(p, OUTPUT);
  if(p == -1) { 
    if(debug) Serial.println("badpin"); 
    return; 
  }
  analogWrite(p,atoi(val));
}

int getPin(char *pin) { //Converts to A0-A5, and returns -1 on error
  int ret = -1;
  if(pin[0] == 'A' || pin[0] == 'a') {
    switch(pin[1]) {
    case '0':  
      ret = A0; 
      break;
    case '1':  
      ret = A1; 
      break;
    case '2':  
      ret = A2; 
      break;
    case '3':  
      ret = A3; 
      break;
    case '4':  
      ret = A4; 
      break;
    case '5':  
      ret = A5; 
      break;
    default:             
      break;
    }
  } 
  else {
    ret = atoi(pin);
    if(ret == 0 && (pin[0] != '0' || pin[1] != '0')) {
      ret = -1;
    }
  }
  return ret;
}

/*
 * Handle Ping commands
 * fire, read
 */
void handlePing(char *pin, char *val, char *aux) {
  if (debug) Serial.println("ss");
  int p = getPin(pin);

  if(p == -1) { 
    if(debug) Serial.println("badpin"); 
    return; 
  }
  Serial.println("got signal");

  // 01(1) Fire and Read
  if (atoi(val) == 1) {
    char m[16];

    pinMode(p, OUTPUT);
    digitalWrite(p, LOW);
    delayMicroseconds(2);
    digitalWrite(p, HIGH);
    delayMicroseconds(5);
    digitalWrite(p, LOW);

    Serial.println("ping fired");

    pinMode(p, INPUT);
    sprintf(m, "%s::read::%08d", pin, pulseIn(p, HIGH));
    Serial.println(m);

    delay(50);
  }
}

/*
 * Handle Servo commands
 * attach, detach, write, read, writeMicroseconds, attached
 */
void handleServo(char *pin, char *val, char *aux) {
  if (debug) Serial.println("ss");
  int p = getPin(pin);
  if(p == -1) { 
    if(debug) Serial.println("badpin"); 
    return; 
  }
  Serial.println("signal: servo");

  // 00(0) Detach
  if (atoi(val) == 0) {
    //servo.detach();
    char m[12];
    sprintf(m, "%s::detached", pin);
    Serial.println(m);

    // 01(1) Attach
  } 
  else if (atoi(val) == 1) {
    // servo.attach(p, 750, 2250);
    //servo.attach(p);
    char m[12];
    sprintf(m, "%s::attached", pin);
    Serial.println(m);

    // 02(2) Write
  } 
  else if (atoi(val) == 2) {
    Serial.println("writing to servo");
    Serial.println(atoi(aux));
    // Write to servo
    //servo.write(atoi(aux));
    delay(15);

    // TODO: Experiment with microsecond pulses
    // digitalWrite(pin, HIGH);   // start the pulse
    // delayMicroseconds(pulseWidth);  // pulse width
    // digitalWrite(pin, LOW);    // stop the pulse

    // 03(3) Read
  } 
  else if (atoi(val) == 3) {
    Serial.println("reading servo");
    //int sval = servo.read();
    char m[13];
    //sprintf(m, "%s::read::%03d", pin, sval);
    Serial.println(m);
  }
}



