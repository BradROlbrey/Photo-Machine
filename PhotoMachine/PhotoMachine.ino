#include <AccelStepper.h>

const byte MS1 = 0;
const byte MS2 = 1;
const byte MS3 = 2;

const byte STP = 3;
const byte DIR = 4;

const int step_size = 16;   // 1/step_size is the actual step size, of course, but efficiency!
const int steps_per_rev = 200 * step_size;
const int num_photos = 5;  // Number of pictures to take all the way around the object.
const double steps_per_photo = 1000;//steps_per_rev / num_photos;
  // (steps / rev) / (photos / rev) => (steps / photo)
  // May not always divide cleanly, but want to make sure to go full 360 degree rotation!


void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for the Serial monitor, apparently it's s/w instead of h/w?
  Serial.println("Hello, World!");
  
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(STP, OUTPUT);
  pinMode(DIR, OUTPUT);
  
  //full();
  //half();
  //quarter();
  //eighth();
  sixteenth();
  
  digitalWrite(DIR, LOW);
  digitalWrite(STP, LOW);

  //one_move();

  Serial.print("Steps per photo: ");
  Serial.println(steps_per_photo);
}

void loop() {
  
  int count = 0;
  while (true) {
    if (Serial.available()) {
      Serial.read();
      Serial.print("\n\nMoving ");
      one_move();
    
      Serial.println(++count);
    }
  }
  
}

// one_move rotates the camera from one picture position to the next.
// We want long delays to begin with to start slow, then short delays to move fast,
//  then long delays to come to a smooth stop.
// The delay_time (initialized in one_move()) will increase and decrease linearly
//  to produce a(n angular) velocity with a constant rate of change,
//  i.e. constant (angular) acceleration.
const long max_delay = 10000;  // microseconds
const int min_delay = 100;
const int interval = ((max_delay - min_delay) / steps_per_photo) * 2;
  // x2 because delay_time needs to go down AND up, twice the "distance".

void one_move() {

  int steps = 0; 
  int delay_time = max_delay;  // Start slow
  
  while (steps < steps_per_photo) {
    
    one_step();  // Take a single step.
    ++steps;
    Serial.print(steps);
    Serial.print('\t');
    delayMicroseconds(delay_time);

    //Serial.print("Delay time: ");
    Serial.println(delay_time);

    if (steps < steps_per_photo/2)
      delay_time -= interval;  // Make faster in first half
    else
      delay_time += interval;  // Make slower in second half
  }
  
}

void one_step() {
  digitalWrite(STP, HIGH);
  digitalWrite(STP, LOW);
}

void full() {
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);
}
void half() {
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW);
}
void fourth() {
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, LOW);
}
void eighth() {
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, LOW);
}
void sixteenth() {
  digitalWrite(MS1, HIGH);
  digitalWrite(MS2, HIGH);
  digitalWrite(MS3, HIGH);
}
