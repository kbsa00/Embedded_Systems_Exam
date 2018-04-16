// This #include statement was automatically added by the Particle IDE.
#include <SparkTime.h>


// This #include statement was automatically added by the Particle IDE.
#include <HC-SR04.h>

// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h>

#define DHTTYPE DHT11
#define DHTPIN D0
#define TRIGGER A0

#define ECHO A1
#define buzzerOne D2
#define pauseSystemButton  D3
#define turnOnOffSystemButton D4
#define redpin D5
#define greenpin D6
#define bluepin D7

DHT dht(DHTPIN, DHTTYPE);

HC_SR04 moveSensor = HC_SR04(TRIGGER, ECHO);
SparkTime rtc;
UDP UDPClient; 

unsigned long thisTime;
unsigned long lastTime;
bool isSystem = false;  
bool sentTempInfo; 

int lastPausebuttonPressed = 0;
int pauseButtonState = 0;
int turnOnOfSystemState = 0; 
int lastTurnOfOnSystemPressed = 0; 

/**
 * Setting up everything from buttons, leds, sensors and timeZone 
 */

void setup() {
    dht.begin();
    rtc.begin(&UDPClient, "no.pool.ntp.org");
    rtc.setTimeZone(+1);
    
    sentTempInfo = false; 
    moveSensor.init();
    lastTime  = millis();
    pinMode(redpin, OUTPUT); 
    pinMode(greenpin, OUTPUT); 
    pinMode(bluepin, OUTPUT); 
    
    
    pinMode(pauseSystemButton, INPUT); 
    pinMode(turnOnOffSystemButton, INPUT); 
    Particle.function("updatesystem", TurnOnOrOffSystem);
    Particle.function("gettemp", getInstanceTempratureAndHumidity);
    digitalWrite(redpin, 255);
}


/**
 * A simple loop that always checks if the system is on or not. 
 * If the system is on it starts monitoring the sensors.
 */
 
void loop() {
    
    turnOnOfSystemState = digitalRead(turnOnOffSystemButton);
    
    //Checking wheather the whole system is on, If it is, it will keep on checking the sensors until someone
    //turns of the system.
    if(isSystem){
        digitalWrite(redpin, 0);
        digitalWrite(greenpin, 255);
        HomeWatcherSystem(); 

    }
    
    
    
    //Handling the on and off button for the System.
    if(turnOnOfSystemState == HIGH && lastTurnOfOnSystemPressed == LOW){
        
        //Turning on and off the System based on it's current state. 
        if(isSystem != true){
            Particle.publish("SystemNotifier", "Watch-Homer is now Turned ON"); 
            isSystem = true;
            digitalWrite(redpin, 0);
            digitalWrite(greenpin, 255);
        }else{
            Particle.publish("SystemNotifier", "Watch-Homer is now Turned OFF"); 
            isSystem = false;
             digitalWrite(greenpin, 0);
            digitalWrite(redpin, 255);
        }
    }
    
    lastTurnOfOnSystemPressed = turnOnOfSystemState;
}


/**
 * This is just an entire method for how the Home Watcher System works. 
 * It will constantly check if theres any movement. If theres a movement the method will then 
 * procede to let the owner know that there's movement in the apartment.
 * This method will also allow the user to press a button where you only have 30 seconds to get out.
 */ 
void HomeWatcherSystem(){
    
     thisTime = millis();
    
    pauseButtonState = digitalRead(pauseSystemButton); 
    
    checkRoomsCondition(); 
     
    
    if(thisTime - lastTime > 2000){
    
      //Checking the UltraSonic Sensor if theres any movement.
      
      digitalWrite(TRIGGER, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIGGER, HIGH);
      delayMicroseconds(10);
      
      long duration = pulseIn(ECHO, HIGH); 
      int distance = duration * 0.034/2; 
      
      if(distance < 100)
        {
          int nowTime = rtc.now(); 
          String TimeStamp;
          if(rtc.minute(nowTime) < 10){
             TimeStamp = String(rtc.hour(nowTime)) + " : 0" + String(rtc.minute(nowTime)); 
          }else{
             TimeStamp = String(rtc.hour(nowTime)) + " : " + String(rtc.minute(nowTime));    
          }
          
          Particle.publish("Notifier", "Home-Watcher picked up on movement in your apartment!! AT THIS TIME STAMP: " + TimeStamp);
          digitalWrite(greenpin, 0);
          soundTheAlarm();  
        }
        
        lastTime = millis();
    }
    
    if (pauseButtonState == HIGH && lastPausebuttonPressed == LOW ){
        Particle.publish("SystemNotifier", "The alarm was now turned off for 30 seconds! HURRY UP leaving the Student complex");
        digitalWrite(greenpin, 0);
        PauseTheHomeWatcher();   
    }
    
    lastPausebuttonPressed = pauseButtonState; 
}

/**
 * This is just a simple method that alerts the user of the current temperature and humidity of the 
 * room. The alert will happend only on specified hours of the day!
 */

void checkRoomsCondition(){
    //Checking the rooms condition, Temprature etc. 
    int lastHour = 0; 
    int currentHour = rtc.now();

    if (lastHour != currentHour && rtc.hour(currentHour) == 10 && sentTempInfo == false){
        sentTempInfo = true; 
        int temp = dht.getTempCelcius();
        int humidity = dht.getHumidity();
        Particle.publish("TempAlert", "Your Student Complex information: Temprature: " + String(temp) + " ºC " + " Humidity " + String(humidity) + "%" );
        lastHour = currentHour; 
    }

    
}


/**
 * Simple method for an alarm when people are moving around in the apartment.
 */

void soundTheAlarm(){
    
    digitalWrite(redpin, 255);
    digitalWrite(bluepin, 255);
    tone(buzzerOne, 2000, 5000);
    delay(1000);
    tone(buzzerOne, 2000, 5000);
    delay(1000);
    digitalWrite(redpin, 0);
    digitalWrite(bluepin, 0);

}


/**
 * Simple method that pauses the entire Home Watcher System.
 */

void PauseTheHomeWatcher(){
    
    int index = 0;
    
    while(index <= 30){
        
        digitalWrite(redpin, 255);
        delay(500);
        tone(buzzerOne, 2000, 500); 
        delay(500);
        digitalWrite(redpin, 0);
        
  
        index++; 
    }
}

/**
 * Simple method that allows the user to send commands from their phone for turning off the Home Watcher System.
 */
int TurnOnOrOffSystem(String command){
    
    if(command.toLowerCase() == "on"){
        Particle.publish("SystemNotifier", "Watch-Homer is now Turned ON");
        digitalWrite(redpin, 0);
        digitalWrite(greenpin, 255);
        isSystem = true; 
    }
    else if(command.toLowerCase() == "off"){
         Particle.publish("SystemNotifier", "Watch-Homer is now Turned OFF");
         isSystem = false;
         digitalWrite(greenpin, 0);
         digitalWrite(redpin, 255);
    }
}

/**
 * Simple method that allows the user to send commands from their phone. This method will then
 * send a message to the user on their current room temperature and humidity
 */
int getInstanceTempratureAndHumidity(String command){
        
    if(command == "now"){
      int temp = dht.getTempCelcius();
      int humidity = dht.getHumidity();
      Particle.publish("TempAlert", "Your Student Complex information: Temprature: " + String(temp) + " ºC " + " Humidity " + String(humidity) + "%" );
    }
}
