// additional changes
// limit the number of times a client info is amountFetched
// saveTransaction should run repeatedly to make sure the transaction is realy saved
// updateBalance should run repeatedly to make sure the balance is updated

#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#include <SPI.h>
#include <MFRC522.h>

#include <avr/sleep.h>
#include "LowPower.h"

#define SS_PIN 10
#define RST_PIN 9

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

SoftwareSerial SIM800L(A1, A0);

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte nuidPICC[4];
String tagId = "";

const byte amp = 7;
const byte button1 = 3;
const byte button3 = 6;
const byte buzzer = A3;
const byte led = 5;
const byte valve = 4;

const byte _start = 8;
const byte _reset = A2;

volatile long flow_frequency; // Measures flow sensor pulses
unsigned long l_hour; //Calculated litres/hour
unsigned long totalL_hour = 0;
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;
boolean tag = false;
float volume = 0;
unsigned long totalTime = 0;
boolean volselected = false;
double reqVolume = 0.0;
String clientName = "";
String clientBalance = "";
float _balance = 0.0;
float _amount = 0.0;
long millisNow = 0;
long prevMillis = 0;
const long interval = 60000;
boolean validTag = false;
float amountFetched = 1.5;
String firstName, seconName, reply;
const float amountPerLiter = 0.05;
boolean enoughBalance = true;
byte attempts = 0;
String code = "";

void setup() {
  pinMode(_start, OUTPUT);
  pinMode(_reset, OUTPUT);
  pinMode(amp, OUTPUT);
  digitalWrite(amp, 1);

  digitalWrite(_reset, 0);

  digitalWrite(_start, 0);
  delay(500);
  digitalWrite(_start, 1);
  delay(500);
  digitalWrite(_start, 0);
  delay(500);
  digitalWrite(_start, 1);

  pinMode(valve, OUTPUT);
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
  attachInterrupt(0, flow, RISING); // Setup Interrupt
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;


  pinMode(button1, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(button1), wakeUp, FALLING);

  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  Serial.begin(38400);
  SIM800L.begin(38400);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
  lcd.begin(20, 4);
  lcd.setBacklight(HIGH);
  lcd.clear();

  lcd.home();
  lcd.print(F("AQUANAUT SYSTEMS"));
  lcd.setCursor(0,2);
  lcd.print(F("   WATER ATM 1.0"));
  delay(15000);
  lcd.clear();
  lcd.home();
  lcd.print(F("Please Wait!"));
  lcd.setCursor(0,1);
  lcd.print(F("Connecting to "));
  lcd.setCursor(0,2);
  lcd.print(F(" the network...."));
  gprsInit();
  lcd.clear();
}

void loop(){
  // while (1) {
  //   fetchWater2();
  // }
  //fetchWater();
  ////////////////////////////////////////////////
  prevMillis = millis();
  validTag = false;
  while (!validTag) {
  while(!tag) {
    //while(1){/////////
      prompt();
      readTag();
    //}////////
    millisNow = millis();
    if(millisNow - prevMillis >= interval)
      sleep();
    if(!digitalRead(button1))
    {
      wakeUpAgain();
      prompt();
      prevMillis = millis();
    }
  }
  alert(100);

  tag = false;
  lcd.clear();
  lcd.home();
  lcd.print(F("Please wait!"));
  lcd.setCursor(0, 2);
  lcd.print(F("Checking your info.."));
  //Serial.println(F("Getting info for tagID: "));
  //Serial.println(tagId);
  //tagId = "'" + tagId + "'";
  clientName = "";
  attempts = 0;
  getClientData(tagId);
  delay(2000);
  lcd.clear();

  //saveTransaction();
  }
  ///////end while////////////////////////////////////////////////////////////////////

  lcd.home();
  lcd.print(F("OPENING THE TAP"));
  lcd.setCursor(0, 1);
  lcd.print(F("NOW..."));
  delay(1000);
  lcd.clear();
  fetchWater2();
  float balanceLeft = _balance - amountFetched;
  if(balanceLeft <= 10.0)
    notifyBalance();
  //reply = "";
  //attempts = 0;
  //updateBalance(tagId, balanceLeft);
  if (amountFetched > 0) {
    Serial.println(F("saving transaction"));
    code = "";
    attempts = 0;
    lcd.home();
    lcd.print(F("Saving transaction"));
    lcd.setCursor(0,1);
    lcd.print(F("Please Wait!!!"));
    saveTransaction();
  }

  ///////////////////////////////////////////////////////////////////////////////////////

  lcd.clear();

}

void prompt()
{
  //lcd.clear();
  lcd.home();
  lcd.print(F("You are welcome :)"));
  lcd.setCursor(0, 2);
  lcd.print(F("Please show your tag"));
  lcd.setCursor(0, 3);
  lcd.print(F("to proceed ..."));
}

void updateBalance(String id, float balance_left)//update successful
{
  while (reply.indexOf("successful") < 0 && attempts < 5)
  {
    Serial.println("Updating with new balance: " + String(balance_left) + " for tagId: " + id);
    SIM800L.println(F("AT+HTTPINIT\r\n"));
    delay(500);
    updateSerial();
    SIM800L.println(F("AT+HTTPPARA=\"CID\",1\r\n"));
    delay(500);
    updateSerial();
    SIM800L.println("AT+HTTPPARA=\"URL\",\"http://aqwateratm.000webhostapp.com/update.php?nb=" + String(balance_left)  + "&id=" + id + "\"");
    delay(3000);
    updateSerial();
    SIM800L.println(F("AT+HTTPACTION=0\r\n"));
    delay(4000);
    updateSerial();
    SIM800L.println(F("AT+HTTPREAD\r\n"));
    delay(4000);
    getReply();
    //updateSerial();
    //Serial.println("reply: " + String(reply));
    SIM800L.println(F("AT+HTTPTERM\r\n"));
    delay(500);
    updateSerial();
    delay(500);
    attempts++;
  }

}

void saveTransaction()
{
  while (code.indexOf("201") < 0 && attempts < 5) {
    //String customer = firstName + "_" + seconName;
    //customer = "'"+ customer +"'";
    //String _amountFetched = "'"+ String(amountFetched) + "'";
    //_amountFetched = "0.2";
    Serial.println("Saving transaction for: " + tagId + " amount fetched: " + String(amountFetched));
    SIM800L.println(F("AT+HTTPINIT\r\n"));
    delay(500);
    updateSerial();
    SIM800L.println(F("AT+HTTPPARA=\"CID\",1\r\n"));
    delay(500);
    updateSerial();
    //SIM800L.println("AT+HTTPPARA=\"URL\",\"http://aqwateratm.000webhostapp.com/transactions.php?customer=" + customer  + "&amount=" + _amountFetched + "\"");
    //SIM800L.println("AT+HTTPPARA=\"URL\",\"http://device.aquanautllc.com/api/v1/card?customer=" + tagId  + "&amount=" + _amountFetched + "\"");
    SIM800L.println("AT+HTTPPARA=\"URL\",\"http://device.aquanautllc.com/api/v1/water-transaction/add?customer=" + tagId  + "&amount=" + String(amountFetched) + "\"");
    delay(3000);
    updateSerial();
    SIM800L.println(F("AT+HTTPACTION=0\r\n"));
    delay(4000);
    //updateSerial();
    getHttpActionCode();

    //Serial.println("***********");
    SIM800L.println(F("AT+HTTPREAD\r\n"));
    delay(4000);
    //updateSerial();
    getReply();
    SIM800L.println(F("AT+HTTPTERM\r\n"));
    delay(500);
    updateSerial();
    attempts++;
  }

}

void  alert(int d)
{
  digitalWrite(buzzer, 1);
  digitalWrite(led, 1);
  delay(d);
  digitalWrite(buzzer, 0);
  digitalWrite(led, 0);
}

void readTag()
{
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;

  //Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  //Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    //Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }

    tagId = getTagId(rfid.uid.uidByte, rfid.uid.size);
    tag = true;
    //Serial.println(tagId);/////////////

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}

String getTagId(byte *buffer, byte bufferSize){
  String id;
  for (byte i = 0; i < bufferSize; i++)
  {
    if(buffer[i] < 0x10)
      id += "0" + String(buffer[i], HEX);
    else
      id += String(buffer[i], HEX);
  }
     return id;
 }

void flow () // Interrupt function
{
   flow_frequency++;
}
/////////////////////////////////////////////////////////////////////////////
void fetchWater2()
{
  alert(50);
  delay(50);
  alert(50);
  delay(50);
  lcd.print(F("Press the black"));
  lcd.setCursor(0,1);
  lcd.print(F("button to fetch"));
  while(digitalRead(button1));
  lcd.clear();
  valveOC(1);
  //lcd.clear();
  lcd.home();
  lcd.print(F("****TAP OPEN!!!****"));
  delay(1000);
  lcd.clear();
  totalL_hour = 0;
  totalTime = 0;
  volume = 0.0;
  amountFetched = 0.0;
  flow_frequency = 0; // Reset Counter
  while(digitalRead(button3))
  {
    currentTime = millis();
    // Every second, calculate and print litres/hour
    if(currentTime >= (cloopTime + 1000))
    {
      Serial.println("flow_frequency: " + String(flow_frequency));
      lcd.home();
      lcd.print("FLOW UNITS");
      lcd.setCursor(0,1);
      lcd.print(flow_frequency);
      lcd.setCursor(0,3);
      lcd.print(F("Press green to stop"));
      cloopTime = currentTime;
    }
  }
  valveOC(0);
  delay(2000);
  // amountFetched = volume * amountPerLiter;
  // Serial.println("amountFetched: " + String(amountFetched));
  if(flow_frequency > 50 && flow_frequency < 5000)
    amountFetched = 0.1; // 10p
  if(flow_frequency > 5000 && flow_frequency < 10000)
    amountFetched = 0.2; // 20p
  if(flow_frequency > 10000 && flow_frequency < 15000)
    amountFetched = 0.3; // 30p
  if(flow_frequency > 15000 && flow_frequency < 20000)
    amountFetched = 0.4; // 40p

  Serial.println("amountFetched: " + String(amountFetched));

  lcd.home();
  lcd.clear();
  lcd.print(F("    PIPE CLOSED!!!     "));
  lcd.setCursor(0,1);
  lcd.print("Amount: GHC " + String(amountFetched));
  lcd.setCursor(0,2);
  lcd.print(F("Thanks for your "));
  lcd.setCursor(0,3);
  lcd.print(F("Patronage ):"));
  delay(2000);
  lcd.home();
  alert(1000);
  lcd.clear();
}
/////////////////////////////////////////////////////////////////////////////
void fetchWater()
{
  alert(50);
  delay(50);
  alert(50);
  delay(50);
  lcd.print(F("Press black button"));
  while(digitalRead(button1));
  lcd.clear();
  valveOC(1);
  //lcd.clear();
  lcd.home();
  lcd.print(F("****TAP OPEN!!!****"));
  totalL_hour = 0;
  totalTime = 0;
  volume = 0.0;
  amountFetched = 0.0;
  while(digitalRead(button3))
  {
    currentTime = millis();
    // Every second, calculate and print litres/hour
    if(currentTime >= (cloopTime + 1000))
    {
      if(flow_frequency > 0)
        totalTime++;
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      totalL_hour += l_hour;
      volume = (float)totalL_hour * (float)totalTime/3600.0;
      volume = volume * 0.00345;
      Serial.println("flow_frequency: " + String(flow_frequency));
      flow_frequency = 0; // Reset Counter
      lcd.setCursor(0, 2);
      lcd.print("Flow Rate: " + String(l_hour) + " L/hr ");
      lcd.setCursor(0, 3);
      lcd.print("Volume: " + String(volume)+ " L ");
      Serial.println("totalTime: " + String(totalTime));
      Serial.println("totalL_hour: " + String(totalL_hour));
      //Serial.println("totalL_hour: " + String(totalL_hour));
    }
  }
  valveOC(0);
  delay(2000);
  amountFetched = volume * amountPerLiter;
  Serial.println("amountFetched: " + String(amountFetched));
  lcd.home();
  lcd.clear();
  lcd.print(F("    PIPE CLOSED!!!     "));
  lcd.setCursor(0,2);
  lcd.print(F("Thanks for your "));
  lcd.setCursor(0,3);
  lcd.print(F("Patronage ):"));
  delay(2000);
  lcd.home();
  alert(1000);
}
//////////////////////////////////////////////////////////////////////////////////////
void valveOC(boolean b)
{
  digitalWrite(valve, b);
}

void updateSerial() {
  while(SIM800L.available() > 0)
      Serial.write(SIM800L.read());
  while(Serial.available() > 0)
      SIM800L.write(Serial.read());
}

void getClientData(String tagId)
{
  while (clientName.indexOf("*") < 0 && attempts < 5)
  {
    SIM800L.println(F("AT+HTTPINIT\r\n"));
    delay(200);
    updateSerial();
    SIM800L.println(F("AT+HTTPPARA=\"CID\",1\r\n"));
    delay(200);
    updateSerial();
    //SIM800L.println("AT+HTTPPARA=\"URL\",\"http://aqwateratm.000webhostapp.com/data.php?id=" + tagId + "\"");
    SIM800L.println("AT+HTTPPARA=\"URL\",\"http://device.aquanautllc.com/api/v1/card/balance/" + tagId + "\"");
    //SIM800L.println("AT+HTTPPARA=\"URL\",\"http://device.aquanautllc.com/api/v1/card/balance/9f567402\"");// + tagId + "\"");
    delay(2000);
    updateSerial();
    SIM800L.println(F("AT+HTTPACTION=0\r\n"));
    delay(4000);
    updateSerial();
    SIM800L.println(F("AT+HTTPREAD\r\n"));
    delay(5000);
    //updateSerial();
    getClientInfo();
    SIM800L.println(F("AT+HTTPTERM\r\n"));
    delay(500);
    updateSerial();
    attempts++;
  }

}

void gprsInit()
{
  SIM800L.println("AT\r\n");
  delay(500);
  updateSerial();
  SIM800L.println("AT+CFUN=1\r\n");
  delay(500);
  updateSerial();
  SIM800L.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"\r\n");
  delay(500);
  updateSerial();
  SIM800L.println("AT+CSTT=\"MTN\",\"\",\"\"\r\n");
  delay(3000);
  updateSerial();
  SIM800L.println("AT+SAPBR=1,1\r\n");
  delay(1000);
  updateSerial();
}

void getReply()
{
  reply = "";
  while (SIM800L.available()>0)
  {
    char c = SIM800L.read();
    reply += String(c);
  }

  Serial.println("reply: " + String(reply));
}

void getHttpActionCode()
{
  code = "";
  while (SIM800L.available()>0)
  {
    char c = SIM800L.read();
    code += String(c);
  }
  //Serial.println();
  //Serial.println("The code is: " + code);
  //if(code.indexOf("201")!=-1)
    //Serial.println("Hurray!!!!!");
  //else
    //Serial.println("Subhaanallah");
}

void getClientInfo()
{
    while (SIM800L.available()>0)
    {
      char c = SIM800L.read();
      clientName += String(c);
    }
//*Hen Nu Mensah&0@

  firstName = clientName.substring(clientName.indexOf("*")+1, clientName.indexOf("&"));
  //seconName = clientName.substring(clientName.indexOf("#")+1, clientName.indexOf("&"));
  String cbalance = clientName.substring(clientName.indexOf("&")+1, clientName.indexOf("@"));
  Serial.println("****clientName: ");
  Serial.print(clientName);
  Serial.println("****");
  if(clientName.indexOf("&") != -1)
  {
    lcd.clear();
    lcd.home();
    Serial.println("Name: " + firstName);// + " " + seconName);
    lcd.print("Name: ");
    lcd.setCursor(0, 1);
    lcd.print(firstName);
    //lcd.setCursor(0, 2);
    //lcd.print(seconName);
    lcd.setCursor(0, 3);
    _balance = cbalance.toFloat();
    lcd.print("Balance: GHC" + String(_balance) );
    Serial.println("balance: " + String(_balance));
    delay(3000);
    if(_balance > 0.1)
        validTag = true;
    else{
      lcd.clear();
      lcd.home();
      lcd.print("Your Balance is low!");
      lcd.setCursor(0,2);
      lcd.print("Please top up");
      delay(2000);
      prevMillis = millis();
    }

  }

  if(clientName.indexOf("not") != -1)
  {
    lcd.clear();
    lcd.home();
    lcd.print("Unknown Customer!");
    lcd.setCursor(0, 1);
    lcd.print("Please Register");
    lcd.setCursor(0, 3);
    lcd.print("Thank You ):");
    clientName = "#";
    validTag = false;
  }
}

void notifyBalance()
{
  digitalWrite(_reset, 1);
  delay(500);
  digitalWrite(_reset, 0);
  digitalWrite(amp, 0);
  digitalWrite(_start, 0);
  delay(1000);
  digitalWrite(_start, 1);
  delay(5000);
  digitalWrite(_start, 0);
  delay(1000);
  digitalWrite(_start, 1);
  digitalWrite(_reset, 1);
  delay(500);
  digitalWrite(_reset, 0);
  digitalWrite(amp, 1);
}

void sleep()
{
  lcd.clear();
  lcd.setBacklight(LOW);
  lcd.off();
  while (digitalRead(button1));

//  attachInterrupt(digitalPinToInterrupt(intPin), wakeUpAgain, RISING);
//  sleep_enable();
//  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//  sleep_cpu();
}

void wakeUpAgain()
{
  //sleep_disable();
  lcd.begin(20, 4);
  lcd.setBacklight(HIGH);
  lcd.home();
  lcd.print(F("Please Wait ..."));
  digitalWrite(buzzer, 1);
  delay(300);
  digitalWrite(buzzer, 0);
  prevMillis = millis();
  gprsInit();
  lcd.clear();
  //detachInterrupt(intPin);
}
