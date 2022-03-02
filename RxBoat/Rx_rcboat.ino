#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
const uint64_t pipeIn = 0xE8E8F0F0E1LL; 

#define thro 6
#define ser 5
bool motorarm=false;

RF24 radio(7,8);

int throttle = 0;

Servo servo;

struct MyData {
    byte throttle; 
    byte yaw;
    byte pitch;
    byte roll;
    byte AUX1;
    byte AUX2;
};

MyData data;

void resetData()
{

    data.throttle = 129;
    data.yaw = 127;
    data.pitch = 127;
    data.roll = 127;
    data.AUX1 = 0;
    data.AUX2 = 0;

}


void setup()
{
    Serial.begin(9600);

    resetData();
    radio.begin();
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.openReadingPipe(1,pipeIn);
    radio.startListening();

    
    servo.attach(ser);
   

}



unsigned long lastRecvTime = 0;

void recvData()
{
    while ( radio.available() ){
        radio.read(&data, sizeof(MyData));
        lastRecvTime = millis();
        }
}


void loop()
{
    recvData();
    unsigned long now = millis();

    if ( now - lastRecvTime > 1000 ) {

            resetData();
    }
    

    if (data.throttle==0 && data.pitch==0)
    {
        motorarm=!motorarm;
    }

    if (motorarm)
    {
        int pwm=data.pitch+data.throttle;
        if(pwm<255){
            pwm=255;
        }
        pwm=map(pwm,254,510,0,255);
        analogWrite(thro,pwm);
        servo.write(data.roll);
        Serial.print("Throttle: ");  Serial.print(pwm);        Serial.print("    ");
        Serial.print("Pitch: ");     Serial.print(data.roll);  Serial.print("\n");
    }
}