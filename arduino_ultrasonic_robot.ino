#include <AFMotor.h>

// specify the trig & echo pins used for the ultrasonic sensors
#define TRIG_PIN A4
#define ECHO_PIN A5

#define MAX_SPEED 100
#define ROTATE_SPEED 150
#define TIME_STOP 400
#define TIME_ROTATE 300
#define TIME_TOTAL_ROTATE 600

AF_DCMotor MOTORE_SINISTRA(4);
AF_DCMotor MOTORE_DESTRA(3);

long ultrasonicDuration;
int distance;

const int delayDisplayDistance = 20;       // voglio stampare i dati del sensore ogni 2 secondi
unsigned long lastTimeDisplayDistance = 0; // Ultimo tempo registrato quando ho stampato il valore a schermo

int timeToRotate = TIME_ROTATE; //  300ms è il tempo stimato per rotare
int rotateDirection = 0;        // 0 sinistra, 1 destra, 2 rotazione di 360*

void setup()
{
    Serial.begin(9600);

    // ultrasonic sensor pin configurations
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    MOTORE_SINISTRA.setSpeed(MAX_SPEED);
    MOTORE_DESTRA.setSpeed(MAX_SPEED);
    MOTORE_SINISTRA.run(RELEASE);
    MOTORE_DESTRA.run(RELEASE);
}

void loop()
{
    setDistance(); // read and store the measured distances
    // displayDistance(); // prints debugging messages to the serial console
    stateMachine();
}

int stateMachine()
{
    if (distance > 6 || distance < 0) // if there's nothing in front of us (note: ultrasonicDistance will be negative for some ultrasonics if there's nothing in range)
    {
        AVANTI(); // drive forward
    }
    else // there's an object in front of us
    {
        Serial.print("Ostacolo rilevato a: ");
        Serial.print(distance);
        Serial.println("cm");
        STOP(TIME_STOP);

        // Serial.println("sono ripartito dopo 5 secondi");
        INDIETRO(500, MAX_SPEED);
        getRotate();
    }
}

void getRotate()
{
    switch (rotateDirection)
    {
    case 1: // giro a destra
        timeToRotate = TIME_TOTAL_ROTATE;
        rotateDirection = 2;
        rotate(FORWARD, BACKWARD, ROTATE_SPEED);
        break;
    case 2: // giro di 360*
        timeToRotate = TIME_ROTATE;
        rotateDirection = 0;
        rotate(FORWARD, BACKWARD, ROTATE_SPEED);
        break;
    default: // giro a sinistra
        timeToRotate = TIME_ROTATE;
        rotateDirection = 1;
        rotate(BACKWARD, FORWARD, ROTATE_SPEED);
        break;
    }
}

void setDistance()
{
    // ultrasonic 2
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10); // must keep the trig pin high for at least 10us
    digitalWrite(TRIG_PIN, LOW);

    ultrasonicDuration = pulseIn(ECHO_PIN, HIGH);
    distance = (ultrasonicDuration / 2) / 29;
}

void displayDistance()
{
    if ((millis() - lastTimeDisplayDistance) > delayDisplayDistance)
    {
        Serial.print("Distanza: ");
        Serial.print(distance);
        Serial.print("cm");
        Serial.println();

        lastTimeDisplayDistance = millis();
    }
}

void rotate(int leftDirection, int rightDirection, int speed)
{
    unsigned long turnStartRotate = millis();           // save the time that we started turning
    while ((millis() - turnStartRotate) < timeToRotate) // stay in this loop until turnStartRotate (1.1 seconds) has elapsed
    {
        MOTORE_SINISTRA.setSpeed(speed);
        MOTORE_DESTRA.setSpeed(speed);
        MOTORE_SINISTRA.run(leftDirection);
        MOTORE_DESTRA.run(rightDirection);
    }
}

void INDIETRO(int timeToBackward, int speed)
{
    unsigned long turnStartTime = millis();
    while ((millis() - turnStartTime) < timeToBackward)
    {
        MOTORE_SINISTRA.setSpeed(speed);
        MOTORE_DESTRA.setSpeed(speed);
        MOTORE_SINISTRA.run(FORWARD);
        MOTORE_DESTRA.run(FORWARD);
    }
}

void STOP(int timeStop)
{
    unsigned long lastTimeStop = millis();
    while ((millis() - lastTimeStop) < timeStop)
    {
        MOTORE_SINISTRA.setSpeed(MAX_SPEED);
        MOTORE_DESTRA.setSpeed(MAX_SPEED);
        MOTORE_SINISTRA.run(RELEASE);
        MOTORE_DESTRA.run(RELEASE);
    }
}

void AVANTI()
{
    MOTORE_SINISTRA.setSpeed(MAX_SPEED);
    MOTORE_DESTRA.setSpeed(MAX_SPEED + 2); // Inserisco un gap sul motore di destra per compensare la differenza tra i due motori
    MOTORE_SINISTRA.run(BACKWARD);
    MOTORE_DESTRA.run(BACKWARD);
}