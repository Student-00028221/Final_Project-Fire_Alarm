
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <Servo.h>

LiquidCrystal_I2C lcd(32, 16, 2);
Servo servo;

// temperature variables
int tmpPin = A0;
int tmpVal;
int tmpValC;

// gas variables
byte gasPin = 12;
int gasVal;

// photoresistor variables
int phtresistorPin = A1;
int phtresistorVal;

// IR variables
byte IRpin = 10;
IRrecv IR(IRpin);

// LED variables
byte redLEDpin = 8;
int greenLEDpin = 7;

// buzzer variables
byte buzPin = 4;

// PIR
byte PIRpin = 2;
int pirVal;

// SERVO
byte servoPin = 3;

// Relay
byte relayPin = 5;

// Parameters
// trigger
bool tmpTrigger = 0;
bool gasTrigger = 0;
bool phtresistorTrigger = 0;
bool selectTrig = false;
bool noTrig = false;
bool userTrig = false;
// scroll page
int page = 0;
// authority
int user;
int userChoice;

// millis() variables
unsigned long lastDisplay;
unsigned long prevTime;
unsigned long lastAuthority;
unsigned long currentTime;

int interval = 1000;
byte debounceDelay = 50;

/***************************************************************************************/

void setup()
{
  Serial.begin(9600);

  // OUTPUT PINS
  pinMode(relayPin, OUTPUT); // RELAY
  pinMode(redLEDpin, OUTPUT);  // RED LED
  pinMode(greenLEDpin, OUTPUT); // GREEN LED
  pinMode(buzPin, OUTPUT);    // BUZZER
  servo.attach(servoPin);   // SERVOR MOTOR
  servo.write(0);

  // INPUT PINS
  pinMode(tmpPin, INPUT); // TEMPERATURE
  pinMode(gasPin, INPUT); // GAS
  pinMode(phtresistorPin, INPUT); // PHOTORESISTOR
  pinMode(PIRpin, INPUT); // PIR
  pinMode(IRpin, INPUT);  // IR REMOTE
  IR.enableIRIn();


  // LCD
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.setBacklight(0);
}

/***************************************************************************************/

void loop()
{
  Read_values();
  Fire_detection();
  cooldown_detection();
}

/***************************************************************************************/

void Read_values()
{
  // analog read
  tmpVal = analogRead(tmpPin);
  tmpValC = map(tmpVal, 20, 358, -40, 125);
  phtresistorVal = analogRead(phtresistorPin);

  // digital read
  gasVal = digitalRead(gasPin);
  pirVal = digitalRead(PIRpin);

}

/***************************************************************************************/

void Fire_detection()
{
  if (phtresistorVal >= 600)
  {
    Read_values();
    if (gasVal == HIGH)
    {
      Read_values();
      if (tmpValC >= 100)
      {
        lcd.setBacklight(1);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("FIRE DETECTED!");
        lcd.setCursor(0,1);
        lcd.print("PLEASE EVACUATE");
        Read_values();

        digitalWrite(redLEDpin, HIGH);
        digitalWrite(buzPin, HIGH);
        digitalWrite(relayPin, HIGH);
        servo.write(180);
        tmpTrigger = 1;
        while(tmpValC >= 100){Read_values();}
      }
      gasTrigger = 1;
    }
    phtresistorTrigger = 1;
  }
}

/***************************************************************************************/

void cooldown_detection()
{
  if (tmpTrigger == 1 && tmpValC < 100)
  {
    Read_values();
    // "sudden change in light is detected"
    // "checking carbon dioxide level"
    if (gasTrigger == 1 && gasVal == LOW)
    {
      Read_values();
      // "Accumulation of carbon dioxide detected"
      if (phtresistorTrigger == 1 && phtresistorVal < 600)
      {
        Read_values();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Fire is");
        lcd.setCursor(0, 1);
        lcd.print("Extinguished");
        delay(3000);
        lcd.clear();
        lcd.setBacklight(0);
        while(pirVal != 1){Read_values();}
        Ask_turnOff();
      }
    }
  }
}

/***************************************************************************************/

void Ask_turnOff()
{
  lcd.clear();
  while(pirVal==1)
  {
    lcd.setBacklight(1);
    Read_values();
    lcd.setCursor(0, 0);
    lcd.print("Turn-off alarm?");
    lcd.setCursor(1, 1);
    lcd.print("[<]YES");
    lcd.setCursor(10, 1);
    lcd.print("[>]NO");
    while (IR.decode() == 0) {}
    Store_userAnswer();
  }
}


/***************************************************************************************/

void User_answer()
{
  while (userChoice == '<')
  {
    Display_users();
  }
  while (userChoice == '>')
  {
    Store_userAnswer();
  }
}

/***************************************************************************************/
void Store_userAnswer()
{
  currentTime = millis();
  while(IR.decode())
  {
    if(currentTime - prevTime >= debounceDelay)
    {
      prevTime = currentTime;
      remoteControl();
      User_answer();
    }
    IR.resume();
  }
}

/***************************************************************************************/

void Count_page_number()
{
  switch (page)
  {
    case 0:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("^");
      lcd.setCursor(2, 0);
      lcd.print("Who is the");
      lcd.setCursor(15, 1);
      lcd.print("v");
      lcd.setCursor(4, 1);
      lcd.print("user?");
      break;

    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("^");
      lcd.setCursor(2, 0);
      lcd.print("[1]Sir.Mikko");
      lcd.setCursor(15, 1);
      lcd.print("v");
      lcd.setCursor(2, 1);
      lcd.print("[2]Geducos");
      break;

    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("^");
      lcd.setCursor(2, 0);
      lcd.print("[2]Geducos");
      lcd.setCursor(15, 1);
      lcd.print("v");
      lcd.setCursor(2, 1);
      lcd.print("[3]Toralba");
      break;

    case 3:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("^");
      lcd.setCursor(2, 0);
      lcd.print("[3]Toralba");
      lcd.setCursor(15, 1);
      lcd.print("v");
      lcd.setCursor(2, 1);
      lcd.print("[4]Pacaldo");
      break;
    
    case 4:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("^");
      lcd.setCursor(2, 0);
      lcd.print("[4]Pacaldo");
      lcd.setCursor(15, 1);
      lcd.print("v");
      lcd.setCursor(2, 1);
      lcd.print("[5]De Mesa");
      break;
    
    case 5:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("^");
      lcd.setCursor(2, 0);
      lcd.print("[5]De Mesa");
      lcd.setCursor(15, 1);
      lcd.print("v");
      lcd.setCursor(2, 1);
      lcd.print("[6]Cao");
      break;
    
    case 6:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("^");
      lcd.setCursor(2, 0);
      lcd.print("[6]Cao");
      lcd.setCursor(15, 1);
      lcd.print("v");
      lcd.setCursor(2, 1);
      lcd.print("Select No.");
      break;
  }
}

/***************************************************************************************/

void Display_users()
{
  unsigned long currentDisplay;
  currentDisplay = millis();
  while (IR.decode())
  {
    if (currentDisplay - lastDisplay >= debounceDelay)
    {
      lastDisplay = currentDisplay;
      remoteControl();
      Count_page_number();
    }
    IR.resume();
  }
}

/***************************************************************************************/

void Authorized_users()
{
  switch(user)
  {
    case 1 ... 3:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Access Granted!");
    delay(1000);
    Safety_turnOff();
    break;
    
    case 4 ... 6:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Access Denied!");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please contact");
    lcd.setCursor(0,1);
    lcd.print("the authority");
    delay(2000);
    lcd.clear();
    page = 0;
    user = 0;
    userTrig = true;
    break;
  }
  while(userTrig)
  {
    Display_users();
  }
}

/***************************************************************************************/

void Select_authority()
{
  while(page >= 6)
  {
    lcd.setCursor(0,0);
    lcd.print("Selected No:");
    while (IR.decode())
    {
      if (millis() - lastAuthority >= debounceDelay)
      {
        lastAuthority = millis();
        remoteControl();
        Authorized_users();
        while(1){}
      }
      IR.resume();
    }
  }
}

/***************************************************************************************/

void Safety_turnOff()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Area is Safe");
  digitalWrite(redLEDpin, LOW);
  digitalWrite(greenLEDpin, HIGH);
  digitalWrite(buzPin, LOW);
  digitalWrite(relayPin, LOW);
  delay(3000);
  digitalWrite(greenLEDpin, LOW);
  lcd.clear();
  lcd.setBacklight(0);
  while(1){}
}

/***************************************************************************************/

void remoteControl()
{
  switch (IR.decodedIRData.decodedRawData)
  {
    case 0xFB04BF00:
      Serial.println("Rewind");
      userChoice = '<';
      break;

    case 0xF906BF00:
      Serial.println("Fast forward");
      userChoice = '>';
      break;

    case 0xF708BF00:
      Serial.println("Down");
      lcd.clear();
      if (page < 6)
      {
        page = page + 1;
      }
      else
      {
        Select_authority();
      }
      break;

    case 0xF50ABF00:
      Serial.println("Up");
      lcd.clear();
      if (page > 0)
      {
        page = page - 1;
      }
      else
      {
        page = 0;
      }
      break;

    case 0xEF10BF00:
      Serial.println("1");
      user = 1;
      break;

    case 0xEE11BF00:
      Serial.println("2");
      user = 2;
      break;

    case 0xED12BF00:
      Serial.println("3");
      user = 3;
      break;

    case 0xEB14BF00:
      Serial.println("4");
      user = 4;
      break;

    case 0xEA15BF00:
      Serial.println("5");
      user = 5;
      break;

    case 0xE916BF00:
      Serial.println("6");
      user = 6;
      break;
  }
}

/***************************************************************************************/
