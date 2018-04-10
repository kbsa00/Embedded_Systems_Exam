// This #include statement was automatically added by the Particle IDE.
#include <HC-SR04.h>

// This #include statement was automatically added by the Particle IDE.
#include <Adafruit_DHT.h>

#define DHTTYPE DHT11
#define DHTPIN D0
#define TRIGGER A0
#define ECHO A1
#define buzzerOne D2
#define buzzerTwo D3 




DHT dht(DHTPIN, DHTTYPE);

HC_SR04 moveSensor = HC_SR04(TRIGGER, ECHO);


unsigned long thisTime;
unsigned long lastTime;


void setup() {
    dht.begin();
    moveSensor.init();
    lastTime  = millis();
}

void loop() {
    
    thisTime = millis();
    
    if(thisTime - lastTime > 300000)
    {
        String temp = String(dht.getTempCelcius());
        Particle.publish("Notifier", temp);
    }
    
    
     if(thisTime - lastTime > 2000)
    {
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
              Particle.publish("Notifier", "SOMEONE IS HERE");
              soundTheAlarm();  
           // notifySlackMessageBoard("BURGLER ALERT", "SOMEONE IS IN YOUR APARTMENT!"); 
            //TODO start the alarms!! 
        }
        
        lastTime = millis();
    }
    
  
}


void soundTheAlarm(){
    
    tone(buzzerOne, 2000, 5000);
    tone(buzzerTwo, 6000, 5000);
    
}
