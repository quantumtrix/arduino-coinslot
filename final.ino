#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <SPI.h>
#include <SD.h>

File myFile;

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display
 

int coinpin = 2;
int ledpin = 8;

 // Variables
volatile int pulse = 0;
volatile unsigned long timelastPulse = 0;

boolean bInserted = false;
int credits = 0;
int  price  = 20;
int error;

int screenWidth = 16;
int screenHeight = 2;


String line1 = " iBank "; // Scrolling
String line2 = "Savings:"; // Static


// just some reference flags
int stringStart, stringStop = 0;
int scrollCursor = screenWidth;
 
void setup()
{
    
   
   pinMode( ledpin , OUTPUT);
   digitalWrite( ledpin, LOW);
  
  // Print a message to the LCD.
 
  //Serial.begin(115200);
  Serial.begin(9600);
  Wire.begin();
  Wire.beginTransmission(0x27); //Your LCD Address
  error = Wire.endTransmission();
  Serial.print("Error: ");
  Serial.print(error);

 if (error == 0) {
    Serial.println(": LCD found.");
 
  } else {
    Serial.println(": LCD not found.");
  } // if

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

   

  // if the file opened okay, write to it:
  
 
  lcd.begin(16, 2); // initialize the lcd
 
  
  attachInterrupt(digitalPinToInterrupt(coinpin), coinInterrupt, RISING);
  pinMode(ledpin, OUTPUT);

  

} 

 void loop() {
 
  unsigned long  lastTime = millis() - timelastPulse;
  
  if( (pulse > 0 ) && ( lastTime > 200) ){ // for show LED ON when Inserted
     
     bInserted = false;
      
     credits +=  pulse;
     pulse = 0; 
     lcd.setBacklight(255); 
     digitalWrite(ledpin, HIGH);
     delay(100);
     
    SD.remove("iBank.txt");
     
     myFile = SD.open("iBank.txt", FILE_WRITE);
     
    if (myFile) {
    Serial.print("Writing to iBank.txt...");
    myFile.println(credits);
    // close the file:
    myFile.close();
    Serial.println("done.");
    
    
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening iBank.txt");
  
  }  
     }
   
  digitalWrite(ledpin, LOW); 

  lcd.setBacklight(255);
  lcd.setCursor(0,1);
  lcd.print( line2);

  lcd.setCursor(8,1);
  lcd.print(credits);
  
  lcd.setCursor(scrollCursor, 0);
  lcd.print(line1.substring(stringStart,stringStop));
  
  delay(300);
  
  lcd.clear();
  
  if(stringStart == 0 && scrollCursor > 0){
    scrollCursor--;
    stringStop++;
  } else if (stringStart == stringStop){
    stringStart = stringStop = 0;
    scrollCursor = screenWidth;
  } else if (stringStop == line1.length() && scrollCursor == 0) {
    stringStart++;
  } else {
    stringStart++;
  }
  
} // end loop

// Interrupt
void coinInterrupt(){
 
  // Each time a pulse is sent from the coin acceptor,
  // interrupt main loop to add 1  and flip on the LED
 
  pulse++ ;
  timelastPulse = millis();
  

 // digitalWrite(ledpin, HIGH);
 // Serial.println( pulse );
   
}
