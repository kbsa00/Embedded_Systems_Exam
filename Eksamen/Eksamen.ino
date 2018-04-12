
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


unsigned long thisTime;
unsigned long lastTime;
bool isSystem; 

int lastPausebuttonPressed = 0;
int pauseButtonState = 0;
int turnOnOfSystemState = 0; 
int lastTurnOfOnSystemPressed = 0; 


void setup() {
    dht.begin();
    moveSensor.init();
    lastTime  = millis();
    pinMode(redpin, OUTPUT); 
    pinMode(greenpin, OUTPUT); 
    pinMode(bluepin, OUTPUT); 
    
    pinMode(pauseSystemButton, INPUT); 
    pinMode(turnOnOffSystemButton, INPUT); 
    Particle.function("updatesystem", TurnOnOrOffSystem);
    Particle.function("Pause System", pauseHomeWatcherFromPhone); 
}

void loop() {
    
    turnOnOfSystemState = digitalRead(turnOnOffSystemButton);
    
    if(isSystem){
        digitalWrite(redpin, 0);
        digitalWrite(greenpin, 255);
        HomeWatcherSystem(); 

    }

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


void HomeWatcherSystem(){
    
     thisTime = millis();
    
    pauseButtonState = digitalRead(pauseSystemButton); 
    
    checkRoomsCondition(); 
     
    
    if(thisTime - lastTime > 2000){
        float cm[10];
        float avgCm; 
        
        for(int i = 0; i < 10; i++)
        {
            cm[i] =  moveSensor.distCM();
            avgCm += cm[i];
        }
        
        avgCm /= 10.0;
        
        if(avgCm < 20)
        {
          Particle.publish("Notifier", "Home-Watcher picked up on movement in your apartment!!");
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



void checkRoomsCondition(){
    //Checking the rooms condition, Temprature etc. 
    
    int static lastHour = 24; 
    int currentHour = Time.hour(); 
    
    if (lastHour != currentHour && currentHour == 16 || lastHour != currentHour && currentHour == 13 || lastHour != currentHour && currentHour == 20){
        String temp = String(dht.getTempCelcius());
        String humidity = String(dht.getHumidity());
        Particle.publish("TempAlert", "Your Student Complex information: Temprature: " + temp + "C" + " Humidity " + humidity + "%" );
    }
    
    lastHour = currentHour; 
    
}



void soundTheAlarm(){
    
    digitalWrite(redpin, 255);
    digitalWrite(bluepin, 255);
    tone(buzzerOne, 2000, 5000);
    delay(1000);
    digitalWrite(redpin, 0);
    digitalWrite(bluepin, 0);
}


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

//From the phone
int TurnOnOrOffSystem(String command){
    
    if(command == "on"){
        digitalWrite(redpin, 0);
        digitalWrite(greenpin, 255);
        isSystem = true; 
    }
    else if(command == "off"){
        isSystem = false;
         digitalWrite(greenpin, 0);
         digitalWrite(redpin, 255);
    }
    
}

//from the phone..
int pauseHomeWatcherFromPhone(String amount){
                
    int index = atoi(amount.c_str());
    
    if(index == NULL){
        index = 30; 
    }
    
    while(index >= 0){
        
        tone(buzzerOne, 2000, 500); 
        delay(1000);
        index--; 
    }
}
