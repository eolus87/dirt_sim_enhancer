// Peripherals variables
// FANs
int PwmFanPin1     = 6;   // FAN
int PwmFanPin2     = 5;   // FAN
//int FanStartValue = 1000; 
int FanValue      = 785, newfanvalue = 0, newffan = -1;
//int FanMin        = 800;  //2.44V
int FanMin        = 20;  //2.44V
int FanMax        = 255; //11.75V
long timecheck    = 0;

// DC Motors
int DCMotorPin1    = 10;   // FAN
int DCMotorPin2    = 9;   // FAN
int MotorMin       = 0;
int MotorValue     = 0;

// Serial comms variables
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;
int i = 0;
int n_ofdata = 2;
int dataarray[] = {0,0,0,0,0,0,0,0};

void setup() {
  //Serial.begin(57600);
  Serial.begin(115200);
  Serial.setTimeout(1);
  //2----------- PWM frequency change for D5 and D6 (This seems to interfere with "delay(XXX)")
  // This PWM configuration modifies frequency in the serial port as well (not good!)
  TCCR0B = TCCR0B & B11111000 | B00000001; // for PWM frequency of 62500.00 Hz for pins D5 and D6
  //TCCR0B = TCCR0B & B11111000 | B00000011; // for PWM frequency of 976.56 Hz (The DEFAULT)
  
  //3----------- PWM frequency change for D9 and D10
  TCCR1B = TCCR1B & B11111000 | B00000001; // set timer 1 divisor to 1 for PWM frequency of 31372.55 Hz
  //TCCR1B = TCCR1B & B11111000 | B00000011; // for PWM frequency of 490.20 Hz (The DEFAULT)
  pinMode(PwmFanPin1, OUTPUT); // PwmFanPin as output
  pinMode(PwmFanPin2, OUTPUT); // PwmFanPin as output
  pinMode(DCMotorPin1, OUTPUT); // PwmFanPin as output
  pinMode(DCMotorPin2, OUTPUT); // PwmFanPin as output
  
  analogWrite(PwmFanPin1, FanMax);
  analogWrite(PwmFanPin2, FanMax);
  delay(1000);
  analogWrite(PwmFanPin1, FanMin);
  analogWrite(PwmFanPin2, FanMin);
  timecheck = millis();
}

void loop() {
  recvWithStartEndMarkers();
  if (newData == true)
  {
    showNewData();
    for (i=0; i < n_ofdata; i++)
    {Serial.print(dataarray[i]);
    Serial.print(',');}
    Serial.println("");
    newData = false;

    FanValue   = round(FanMin+(float(FanMax-FanMin)/100)*dataarray[0]);
    if (FanValue > FanMax) {FanValue = FanMax;}
    if (FanValue < FanMin) {FanValue = FanMin;}
    //if (dataarray[0]>100) {dataarray[0]=100;}
    //if (dataarray[0]<10)  {dataarray[0]=10;}
    analogWrite(PwmFanPin1, FanValue);
    analogWrite(PwmFanPin2, FanValue);

    MotorValue   = round(MotorMin+(float(FanMax-MotorMin)/100)*dataarray[1]);
    if (MotorValue > FanMax) {MotorValue = FanMax;}
    if (MotorValue < 0) {MotorValue = 0;}
//    if (dataarray[1]>100) {dataarray[1]=100;}
//    if (dataarray[1]<0)   {dataarray[1]=0;}
    analogWrite(DCMotorPin1, MotorValue);
    analogWrite(DCMotorPin2, MotorValue);

    timecheck = millis();
    
    delay(1);    
  
  }
//  else
//  {if((millis()-timecheck)>2000)
//    {
//      analogWrite(PwmFanPin1, FanMin);
//      analogWrite(PwmFanPin2, FanMin);
//      analogWrite(DCMotorPin1, MotorMin);
//      analogWrite(DCMotorPin2, MotorMin);}}
}
 
//________________________________________________________ AUXILIARY FUNCTIONS
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    int i = 0;
    if (newData == true) {
        //Serial.print("This just in ... ");
        //Serial.println(receivedChars);
          for (i=0; i < n_ofdata; i++)
          {
            int a = (int)receivedChars[4*i]   - 48; a *= 100;
            int b = (int)receivedChars[4*i+1] - 48; b *= 10;
            int c = (int)receivedChars[4*i+2] - 48;
            dataarray[i] = a+b+c;
          }
    }
}
