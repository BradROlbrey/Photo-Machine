#include <AccelStepper.h>

AccelStepper *stepper;

const byte MS1 = 0;
const byte MS2 = 1;
const byte MS3 = 2;

const byte STP = 3;
const byte DIR = 4;

const int step_size = 16;   // 1/step_size is the actual step size, of course, but efficiency!
const int steps_per_rev = 200 * step_size;
const int num_photos = 10;  // Number of pictures to take all the way around the object.
const double steps_per_photo = 100; //steps_per_rev / num_photos;
  // (steps / rev) / (photos / rev) => (steps / photo)
  // May not always divide cleanly, want to make sure to go full 360 degree rotation!

long curr_time;
long prev_time = micros();

void setup() {
  Serial.begin(9600);

  stepper = new AccelStepper(step_forw, step_back);
  stepper->setMaxSpeed(50);
  stepper->setAcceleration(50);
  
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  pinMode(STP, OUTPUT);
  pinMode(DIR, OUTPUT);
  
  //full();
  //half();
  quarter();
  //eighth();
  //sixteenth();
  
  digitalWrite(DIR, LOW);
  digitalWrite(STP, LOW);

  while (!Serial);  // Wait for the Serial monitor, apparently it's s/w instead of h/w
  Serial.println("Hello, World!");  // so it takes longer to initialize.
  
  Serial.print("Steps per photo: ");
  Serial.println(steps_per_photo);

  //one_move();
}

int my_personal_counter;

void loop() {
  
  //int my_personal_counter = 0;
  while (true) {
    if (Serial.available()) {
      my_personal_counter = 0;
      Serial.read();
      //Serial.print("\n\nMoving ");
      one_move();
      Serial.println(my_personal_counter);
      //Serial.println(++my_personal_counter);
      delay(1000);
    }
  }
  
}

void one_move() {
  stepper->move(steps_per_photo);

  prev_time = micros();
  while (stepper->distanceToGo() > 0) {
    stepper->run();
  }
}
  
void step_forw() {
  digitalWrite(DIR, LOW);
  digitalWrite(STP, HIGH);
  digitalWrite(STP, LOW);
  ++my_personal_counter;

  //curr_time = micros();
  //Serial.println(curr_time - prev_time);
  //prev_time = micros();
}
void step_back() {
  digitalWrite(DIR, HIGH);
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
void quarter() {
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
