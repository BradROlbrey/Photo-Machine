
#include <TMCStepper.h>
#include <AccelStepper.h>


#define ARM_DIR_PIN          2  // Direction
#define ARM_STEP_PIN         3  // Step

/*
 *  TMC2208 Driver
 */
#define SW_RX  5  // SoftwareSerial receive pin
#define SW_TX  4  // SoftwareSerial transmit pin
#define R_SENSE 0.11f 
TMC2208Stepper driver(SW_RX, SW_TX, R_SENSE);

/*
 *  AccelStepper
 */
AccelStepper *stepper_arm;  // Need step_forw to be initialized first, so wait until setup().


/*
 *  Photo stuff
 */
const int STEP_SIZE_ARM = 16;   // 1/step_size is the actual step size, of course, but efficiency!
const int STEPS_PER_REV_ARM = 200 * STEP_SIZE_ARM;
const int NUM_PHOTOS_ARM = 10;  // Number of pictures to take all the way around the object.
const double STEPS_PER_PHOTO = 10000; //STEPS_PER_REV_ARM / (double) NUM_PHOTOS_ARM;
  // (steps / rev) / (photos / rev) => (steps / photo)
  // May not always divide cleanly, want to make sure to go full 360 degree rotation!

bool dir_arm = false;


void setup() {
  Serial.begin(9600);

  /*
   *  TMC2208 Driver
   */
  driver.beginSerial(9600);     // SW UART drivers

  driver.begin();                 // SPI: Init CS pins and possible SW SPI pins
                                  // UART: Init SW UART (if selected) with default 115200 baudrate
  driver.toff(5);                 // Enables driver in software
  driver.microsteps(256);          // Set microsteps to 1/16th

  driver.rms_current(200);        // Set motor RMS current (done over UART >:)
  driver.I_scale_analog(1);
  driver.internal_Rsense(0);
  //driver.intpol(0);
  driver.ihold(2);
  driver.irun(31);
  //driver.iholddelay(15);

//driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208
  driver.pwm_autoscale(true);     // Needed for stealthChop


  /*
   *  AccelStepper
   */
  stepper_arm = new AccelStepper(step_arm, step_back_arm);
  stepper_arm->setMaxSpeed(5000);
  stepper_arm->setAcceleration(10000);
  
  
  pinMode(ARM_STEP_PIN, OUTPUT);
  digitalWrite(ARM_STEP_PIN, LOW);
  pinMode(ARM_DIR_PIN, OUTPUT);
  digitalWrite(ARM_DIR_PIN, LOW);

  
  while (!Serial);  // Wait for the Serial monitor, apparently it's s/w instead of h/w
  Serial.println("Hello, World!");  // so it takes longer to initialize.

  one_move();
  
  //Serial.print("Steps per photo: ");
  //Serial.println(STEPS_PER_PHOTO);
}

int counter = 0;
unsigned long prev_time;
unsigned long curr_time;

void loop() {

  // TODO: WASD these serial reads

  while (true) {
    if (Serial.available()) {
      Serial.read();
      //Serial.print("Moving ");
      
      one_move();
      
      //Serial.println(++counter);
    }
  }
  
}


void one_move() {
  Serial.println("Moving ");
  stepper_arm->move(STEPS_PER_PHOTO);
  
  while (stepper_arm->distanceToGo() > 0) {
    stepper_arm->run();
  }
}


void step_arm() {
  digitalWrite(ARM_STEP_PIN, HIGH);
  digitalWrite(ARM_STEP_PIN, LOW);
  /*
  curr_time = micros();
  //Serial.print(++counter);
  //Serial.print('\t');
  Serial.println(curr_time - prev_time);
  //Serial.print('\t');
  //Serial.println(1000000/(curr_time - prev_time));
  prev_time = micros();
  */
}
void step_back_arm() { /* Just reverse direction and call step_arm() */ }
