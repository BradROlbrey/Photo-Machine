
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <Wire.h>
#include <util/atomic.h> // this library includes the ATOMIC_BLOCK macro.


/*
 *  TMC2208 Driver
 */
#define SW_RX  12  // SoftwareSerial receive pin
#define SW_TX  11  // SoftwareSerial transmit pin
#define R_SENSE 0.11f 
TMC2208Stepper driver(SW_RX, SW_TX, R_SENSE);
// Seeing as how I pass those in as parameters, I could probably control two different drivers
// easily enough by just having two instances, esp since it's software serial.

#define MICROSTEPS 256L  // Don't do zero; I changed the library with the first program that has this comment.

// Don't think these need to be volatile because they're read-only.
const long TOTAL_STEPS_AROUND = 200L * MICROSTEPS;
const long TOTAL_STEPS_UP = 300L * MICROSTEPS;
//const long TOTAL_STEPS_UP = 50L * MICROSTEPS;

/*
 *  AccelStepper
 */
AccelStepper *stepper_arm;  // Need step_forw to be initialized first, so wait until setup() to instantiate an instance.
AccelStepper *stepper_line;

#define ENA_PIN             7  // For both motors

#define ARM_DIR_PIN         8   // Direction
#define ARM_STEP_PIN        9   // Step

#define LINE_DIR_PIN        5
#define LINE_STEP_PIN       6


/*
 *  Photo stuff
 */
// Don't think these need to be volatile, but just in case because they are written in I2C interrupts.
volatile long steps_per_photo_around = NULL;  // To be assigned after receiving photos per rotation
  // (steps / rev) / (photos / rev) => (steps / photo)
  // May not always divide cleanly, but it's good enough with our epic microstepping, and we'll
  //  be switching direction anyways, so not going to do type double for now.
volatile long steps_per_level = NULL;

bool arm_dir = false;
bool line_dir = false;

/*
 *  I2C
 */
volatile char next_byte = ' ';  // Stores the value from Wire.read().
volatile byte moving = 0;       // Tells the Pi whether the camera is still moving. Pi will wait
                                // until camera stops before taking a picture.
bool executed_moving = false;
volatile byte processing = 0;   // Tells the Pi when the Arduino is ready to receive
                                // more instructions (bytes).
bool executed_processing = false;

/*
 *  Serial.printing
 */
unsigned long prev_time;
unsigned int print_interval;


void setup() {
  Serial.begin(9600);

  /*
   *  TMC2208 Driver
   */
  driver.beginSerial(9600);     // SW UART drivers

  driver.begin();                 // SPI: Init CS pins and possible SW SPI pins
                                  // UART: Init SW UART (if selected) with default 115200 baudrate
  driver.toff(5);                 // Enables driver in software
  driver.microsteps(MICROSTEPS);  // Set microsteps to whatever

  driver.I_scale_analog(1);
  driver.internal_Rsense(0);
  //driver.intpol(0);
  driver.ihold(1);
  driver.irun(31);
  driver.iholddelay(15);

//driver.en_spreadCycle(false);   // Toggle spreadCycle on TMC2208
  driver.pwm_autoscale(true);     // Needed for stealthChop


  /*
   *  AccelStepper
   */
  pinMode(ENA_PIN, OUTPUT);
  digitalWrite(ENA_PIN, HIGH);  // HIGH is disabled
  
  stepper_arm = new AccelStepper(step_arm, step_arm);  // Yes, they are the same
  stepper_arm->setMaxSpeed(50L * MICROSTEPS);
  stepper_arm->setAcceleration(50L * MICROSTEPS);
  
  pinMode(ARM_STEP_PIN, OUTPUT);
  digitalWrite(ARM_STEP_PIN, LOW);
  pinMode(ARM_DIR_PIN, OUTPUT);
  digitalWrite(ARM_DIR_PIN, LOW);
  
  stepper_line = new AccelStepper(step_line, step_line);
  stepper_line->setMaxSpeed(100L * MICROSTEPS);
  stepper_line->setAcceleration(100L * MICROSTEPS);
  
  pinMode(LINE_STEP_PIN, OUTPUT);
  digitalWrite(LINE_STEP_PIN, LOW);
  pinMode(LINE_DIR_PIN, OUTPUT);
  digitalWrite(LINE_DIR_PIN, LOW);


  /*
   *  I2C
   */
  Wire.begin(0x7f);               // Join i2c bus with address
  Wire.onReceive(receiveEvent);   // Register event for receiving data from master
  Wire.onRequest(requestEvent);   // Register event for sending data to master upon request

  /*
   *  Serial
   */
  while (!Serial);  // Wait for the Serial monitor, apparently it's s/w instead of h/w
  Serial.println("Hello, World!");  // so it takes longer to initialize.

  /*
   *  Photo stuff
   */
  
  /*print_interval = 5000;
  prev_time = millis() - print_interval;
  // Wait to receive over I2C
  while (steps_per_photo_around == NULL ||
         steps_per_level == NULL) {
    if (millis() - prev_time > print_interval) {
      Serial.println("Waiting to receive num_photos and num_levels");
      prev_time += print_interval;
    }
  }*/
  print_interval = 250;
  Serial.println();
}


void loop() {
  //Serial.println("\nRunning");
  stepper_arm->run();
  stepper_line->run();
  

  if (stepper_arm->distanceToGo() == 0
      && stepper_line->distanceToGo() == 0
      && executed_moving == true  // ensure that we've run through a case before reseting moving to false
    )
    {
    Serial.println("Moving set to 0");
    moving = 0;
    executed_moving = false;
  }
  

  /*Serial.print("before switch '");
  Serial.print(next_byte);
  Serial.print("' ");
  Serial.print(processing);
  Serial.print(' ');
  Serial.println(executed);*/
  switch (next_byte) {
    case ' ':
      //executed_processing = true;  // This one doesn't count!
      //Serial.println("Blank");
      break;
      
    case 'w':  // Slide camera up
      executed_processing = true;
      executed_moving = true;
      Serial.println('w');
      digitalWrite(LINE_DIR_PIN, HIGH);
      stepper_line->move(steps_per_level);
      break;
      
    case 'd':  // Rotate arm clockwise
      executed_processing = true;
      executed_moving = true;
      Serial.println('d');
      digitalWrite(ARM_DIR_PIN, LOW);
      stepper_arm->move(steps_per_photo_around);
      break;
      
    case 's':  // Slide camera down to bottom
      /*Serial.print("Curr stepper_line position: ");
      Serial.println(stepper_line->currentPosition());
      Serial.print("Targ stepper_line position: ");
      Serial.println(stepper_line->targetPosition());*/
      
      executed_processing = true;
      executed_moving = true;
      Serial.println('s');
      digitalWrite(LINE_DIR_PIN, LOW);
      stepper_line->moveTo(0);  // absolute
      
      /*Serial.print("Curr stepper_line position: ");
      Serial.println(stepper_line->currentPosition());
      Serial.print("Targ stepper_line position: ");
      Serial.println(stepper_line->targetPosition());*/
      
      break;
      
    case 'a':  // Rotate arm all the way counter-clockwise (returns to start position)
      executed_processing = true;
      executed_moving = true;
      Serial.println('a');
      digitalWrite(ARM_DIR_PIN, HIGH);
      stepper_arm->moveTo(0);  // absolute
      break;

    case 'e':  // Enable motors
      executed_processing = true;
      Serial.println('e');
      digitalWrite(ENA_PIN, LOW);
      break;
      
    case 'q':  // Disable motors
      executed_processing = true;
      Serial.println('q');
      digitalWrite(ENA_PIN, HIGH);
      break;
      
    default:
      executed_processing = true;
      Serial.println("Invalid input");
  }
  
  /*Serial.print("after switch '");
  Serial.print(next_byte);
  Serial.print("' ");
  Serial.print(processing);
  Serial.print(' ');
  Serial.println(executed);*/
    
  if (executed_processing) {
    delay(1000);  // To force check Pi's wait_for_processing
    // In here so that outer loop can run and Arduino can "finish" moving.
    Serial.println("executed_processing ");
    next_byte = ' ';
    //Serial.print(next_byte);
    //Serial.print("' ");
    processing = 0;  // We have processed the Pi's input.
    //Serial.println(processing);
    executed_processing = false;
  }
  // Receiving bytes needs to be blocking in case the Pi sends two bytes in quick
  // succession before we get around to processing the first. This could happen anywhere!
}


// Receive data from Pi
// Interrupts delay(), but Arduino will continue waiting after returning from receiveEvent.
void receiveEvent(int howMany) {
  if (howMany == 1) {
    byte recvd_byte = Wire.read();
    Serial.print("Rec: ");
    Serial.println(int(recvd_byte));
    
    // Two cases:
    //  1. Receiving a "register", as the Pi smbus docs call it, which we'll ignore.
    //  2. Receiving the usual wasd or qe, which we'll process.
    if (recvd_byte != 0 && recvd_byte != 1) {
      processing = 3;
      
      next_byte = recvd_byte;
      Serial.print("Set ");
      Serial.println(next_byte);
      
      if (next_byte != 'e' && next_byte != 'q')  // 'e' and 'q' don't constitute moving
        moving = 2;
    }
  }
  else /* howMany == 3 */ {
    // Receiving two important setup integers (actually they're bytes, 0-255).
    int dont_care = Wire.read();  // Could be useful in the future as a code or something.
    
    // Don't think these need to be volatile b/c they're inside the interrupt.
    int num_photos_per_rev = Wire.read();  // Number of pictures to take all the way around the object.
    int num_levels = Wire.read();  // Number of rotations to make, number of times camera moves up.

    // Probably shouldn't have print statements in an interrupt...
    Serial.print("Pictures per rotation: ");
    Serial.println(num_photos_per_rev);
    Serial.print("Number of levels: ");
    Serial.println(num_levels);
    Serial.print("Total number of pictures to take: ");
    Serial.println(num_photos_per_rev * num_levels);
  
    steps_per_photo_around = TOTAL_STEPS_AROUND / num_photos_per_rev;
    steps_per_level = TOTAL_STEPS_UP / (num_levels - 1);
  }
}
// Send data to Pi
void requestEvent() {
  delayMicroseconds(8);
  Serial.print("Sending requestEvent()\t");
  Serial.print(moving);
  Serial.print("\t");
  Serial.println(processing);
  byte temp[] = {moving, processing};
  Wire.write(temp, 2);
}


void step_arm() {
  digitalWrite(ARM_STEP_PIN, HIGH);
  digitalWrite(ARM_STEP_PIN, LOW);
}
void step_back_arm() { /* Just reverse direction and call step_arm() */ }

void step_line() {
  digitalWrite(LINE_STEP_PIN, HIGH);
  digitalWrite(LINE_STEP_PIN, LOW);
}
void step_back_line() { }
