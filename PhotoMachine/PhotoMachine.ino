
#include <TMCStepper.h>
#include <AccelStepper.h>


#define ARM_DIR_PIN         5  // Direction
#define ARM_STEP_PIN        6  // Step

#define LINE_DIR_PIN        8
#define LINE_STEP_PIN       9


/*
 *  TMC2208 Driver
 */
#define SW_RX  12  // SoftwareSerial receive pin
#define SW_TX  11  // SoftwareSerial transmit pin
#define R_SENSE 0.11f 
TMC2208Stepper driver(SW_RX, SW_TX, R_SENSE);

#define MICROSTEPS 256

/*
 *  AccelStepper
 */
AccelStepper *stepper_arm;  // Need step_forw to be initialized first, so wait until setup().
AccelStepper *stepper_line;


/*
 *  Photo stuff
 */
const int STEPS_PER_REV_ARM = 200 * (MICROSTEPS+1);
const int NUM_PHOTOS_ARM = 10;  // Number of pictures to take all the way around the object.
const double STEPS_PER_PHOTO = 25 * (MICROSTEPS+1); //STEPS_PER_REV_ARM / (double) NUM_PHOTOS_ARM;
  // (steps / rev) / (photos / rev) => (steps / photo)
  // May not always divide cleanly, want to make sure to go full 360 degree rotation!

bool arm_dir = false;
bool line_dir = false;


void setup() {
  Serial.begin(9600);

  /*
   *  TMC2208 Driver
   */
  driver.beginSerial(9600);     // SW UART drivers

  driver.begin();                 // SPI: Init CS pins and possible SW SPI pins
                                  // UART: Init SW UART (if selected) with default 115200 baudrate
  driver.toff(5);                 // Enables driver in software
  driver.microsteps(MICROSTEPS);          // Set microsteps to 1/16th

  driver.rms_current(200);        // Set motor RMS current (done over UART >:)
  driver.I_scale_analog(1);
  driver.internal_Rsense(0);
  driver.intpol(0);
  driver.ihold(2);
  driver.irun(31);
  driver.iholddelay(15);

//driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208
  driver.pwm_autoscale(true);     // Needed for stealthChop


  /*
   *  AccelStepper
   */
  stepper_arm = new AccelStepper(step_arm, step_back_arm);
  stepper_arm->setMaxSpeed(10 * (MICROSTEPS+1));
  stepper_arm->setAcceleration(10 * (MICROSTEPS+1));
  
  
  pinMode(ARM_STEP_PIN, OUTPUT);
  digitalWrite(ARM_STEP_PIN, LOW);
  pinMode(ARM_DIR_PIN, OUTPUT);
  digitalWrite(ARM_DIR_PIN, LOW);

  
  stepper_line = new AccelStepper(step_line, step_back_line);
  stepper_line->setMaxSpeed(10 * (MICROSTEPS+1));
  stepper_line->setAcceleration(10 * (MICROSTEPS+1));
  
  
  pinMode(LINE_STEP_PIN, OUTPUT);
  digitalWrite(LINE_STEP_PIN, LOW);
  pinMode(LINE_DIR_PIN, OUTPUT);
  digitalWrite(LINE_DIR_PIN, LOW);

  
  while (!Serial);  // Wait for the Serial monitor, apparently it's s/w instead of h/w
  Serial.println("Hello, World!");  // so it takes longer to initialize.

  //one_move();
  
  //Serial.print("Steps per photo: ");
  //Serial.println(STEPS_PER_PHOTO);
}

int counter = 0;
unsigned long prev_time;
unsigned long curr_time;
char next_byte;

void loop() {
  driver.microsteps(MICROSTEPS);  // Because I keep powering the driver after the arduino...

  // TODO: WASD these serial reads

  if (Serial.available()) {
    next_byte = Serial.read();
    if (next_byte != '\n') {
      Serial.print("Moving\t");
      Serial.print(next_byte);
    }


    switch (next_byte) {
      
      case 'w':  // Slide camera up
        Serial.print("\tw");
        
        if (!line_dir) {
          line_dir = true;
          digitalWrite(LINE_DIR_PIN, line_dir);
        }
        one_move_line();
        
        break;
        
      case 's':  // Slide camera down
        Serial.print("\ts");
        
        if (line_dir) {
          line_dir = false;
          digitalWrite(LINE_DIR_PIN, line_dir);
        }
        one_move_line();
        
        break;
        
      case 'a':  // Rotate arm counter-clockwise
        Serial.print("\ta");
        
        if (arm_dir) {
          arm_dir = false;
          digitalWrite(ARM_DIR_PIN, arm_dir);
        }
        one_move_arm();
        
        break;
        
      case 'd':  // Rotate arm clockwise
        Serial.print("\td");
        
        if (!arm_dir) {
          arm_dir = true;
          digitalWrite(ARM_DIR_PIN, arm_dir);
        }
        one_move_arm();
        
        break;

      case '\n':
        break;
      
      default:
        Serial.print("Invalid input, wasd only plz");
    }
    if (next_byte != '\n')
      Serial.println();
    
    //Serial.println(++counter);
  }
  
}


void one_move_arm() {
  stepper_arm->move(STEPS_PER_PHOTO);
  
  while (stepper_arm->distanceToGo() > 0) {
    stepper_arm->run();
  }
}
void one_move_line(){
  stepper_line->move(STEPS_PER_PHOTO);
  
  while (stepper_line->distanceToGo() > 0) {
    stepper_line->run();
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

void step_line() {
  digitalWrite(LINE_STEP_PIN, HIGH);
  digitalWrite(LINE_STEP_PIN, LOW);
  
  //curr_time = micros();
  //Serial.print(++counter);
  //Serial.print('\t');
  //Serial.println(curr_time - prev_time);
  //Serial.print('\t');
  //Serial.println(1000000/(curr_time - prev_time));
  //prev_time = micros();
}
void step_back_line() { ; }
