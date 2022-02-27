#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

const uint64_t pipeIn = 0xE8E8F0F0E1LL; 

#define motorPin 3
#define servoPin 2

RF24 radio(2,4);
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

    data.throttle = 0;
    data.yaw = 127;
    data.pitch = 127;
    data.roll = 127;
    data.AUX1 = 0;
    data.AUX2 = 0;

}


void setup(){
    radio.begin();
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.openReadingPipe(1,pipeIn);
    radio.startListening();

    servo.attach(servoPin);
}

unsigned long lastRecvTime = 0;
bool motorarm=false;

void recvData()
{
    while ( radio.available() ){
        radio.read(&data, sizeof(MyData));
        lastRecvTime = millis();
        }
}

void loop(){
    recvData();
    unsigned long now = millis();
    
    if ( now - lastRecvTime > 1000 ) {
            resetData();
    }
    
    if (data.AUX1==1 && data.AUX2==1)
    {
        motorarm=!motorarm;
    }

    if (motorarm)
    {
        int pwm=data.pitch+data.throttle;
        pwm=map(pwm,255,510,0,255);
        analogWrite(motorPin,pwm);
        servo.write(data.roll);
    }
    
    
}

