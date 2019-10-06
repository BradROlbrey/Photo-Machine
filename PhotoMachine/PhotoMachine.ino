//#include <Arduino.h>

const byte MS1 = 0;
const byte MS2 = 1;
const byte MS3 = 2;

const byte STP = 3;
const byte DIR = 4;

const int step_size = 16;   // 1/step_size is the actual step size, of course, but efficiency!
const int steps_per_rev = 200 * step_size;
const int num_photos = 5;  // Number of pictures to take all the way around the object.
const double steps_per_photo = steps_per_rev/num_photos;  // (steps / rev) / (photos / rev) => (steps / photo)


void setup() {
  Serial.begin(9600);
  delay(500);
  
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

  one_move();

  //Serial.print("Steps per photo: ");
  //Serial.println(steps_per_photo);
}

void loop() {
  //Serial.println(radians(1), 10);
  
  int count = 0;
  while (true) {
    if (Serial.available()) {
      Serial.read();
      //Serial.print("Moving ");
      one_move();
    
      Serial.println(++count);
    }
  }
  
}

// one_move rotates the camera from one picture position to the next.
// We want long delays to begin with to start slow, then short delays to move fast,
//  then long delays to come to a smooth stop.
const long max_delay = 50000;  // microseconds
const int min_delay = 1000;
// Going to use a cosine wave to produce a smooth motion. 0 to 360 degrees will go from
//  1 down to -1 up to 1.
// Need to go 360 degrees in the number of steps required for one move.
const double deg_per_step = 180 / steps_per_photo;
// Note that this degree count has nothing to do with the distance traveled by the stepper,
//  but rather the distance traveled along half a cosine wavelength.

void one_move() {

  int steps = 0; 
  double degree_count = 0;
  
  while (steps < steps_per_photo) {
    
    one_step();  // Take a single step.
    ++steps;
    
    // Then calculate how long the delay should be, using cosine.
    double cosine_point = sin(radians(degree_count + 180));  // from 0 to -1 to 0.
    cosine_point += 1;  // from 1 to 0 to 1, don't want negatives.
    cosine_point *= 1000;  // Shifting decimal place because map doesn't take decimals.
    //Serial.print("cosine point: ");
    //Serial.println(cosine_point);
      
    // Map from 0-1000 to 100-1000 microseconds of delay.
    long delay_time = map(int(cosine_point), 0, 1000, min_delay, max_delay);

    //Serial.print("Delay time: ");
    Serial.println(delay_time);
    
    // Delay...
    delayMicroseconds(delay_time);

    // Increment degree count.
    degree_count += deg_per_step;
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
