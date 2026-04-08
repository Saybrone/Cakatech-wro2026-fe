#include <EVN.h>
#include <SoftwareSerial.h>
#include <math.h>

#define SERVO_DIR_PORT    1
#define IMU_I2C_PORT      1
#define TOF_LEFT_PORT     3
#define TOF_RIGHT_PORT    2
#define PPR_VALUE         2200

const float SERVO_CENTER = 133;
const int   SERVO_RIGHT  = 70;
const int   SERVO_LEFT   = 210;

#define TURN_KP               16.0f
#define TURN_KD               0.8f
#define TURN_SPEED_MAX        1000
#define TURN_SPEED_MIN        200
#define TURN_TOLERANCE        2.0f

#define STRAIGHT_KP           0.5f
#define STRAIGHT_KD           0.2f
#define STRAIGHT_SPEED        600
#define STRAIGHT_DISTANCE_CM  50.0f

#define WALL_FOLLOW_SPEED     800
#define WALL_TARGET_MM        150.0f
#define WALL_KP               0.5f
#define WALL_KD               0.02f
#define WALL_MAX_CORRECTION   60.0f
#define NO_WALL_MM            10000.0f
#define WALL_FOLLOW_DIST_CM   100.0f
#define WALL_TOO_CLOSE_MM     20.0f
#define WALL_CLOSE_KP         1.2f
#define WALL_CLOSE_KD         0.05f

#define WHEEL_DIAMETER_MM     43.2f
#define GEAR_RATIO            1.0f

#define BRIGHTNESS_TURN_THRESH  25.0f

const byte rxPin = 1;
const byte txPin = 0;
int timeout = 500;

EVNAlpha          board;
EVNMotor          motor_left(1, EV3_MED);
EVNMotor          motor_right(2, EV3_MED);
EVNServo          steeringServo(SERVO_DIR_PORT);
EVNIMUSensor      imu(IMU_I2C_PORT);
EVNDistanceSensor tof_left(TOF_LEFT_PORT);
EVNDistanceSensor tof_right(TOF_RIGHT_PORT);
SoftwareSerial mySerial(rxPin, txPin);
volatile float yaw_angle = 0.0f;

// Locked turn direction after the first corner: 0=not yet set, 1=right, -1=left
int locked_turn_dir = 0;

void setup() {
  Serial.begin(9600);
  board.begin();
  board.setLinkLED(true);
  board.setMode(BUTTON_TOGGLE);
  imu.begin();
  imu.setGyroRange(IMU_GYRO_DPS_1000);
  tof_left.begin();
  tof_right.begin();
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  mySerial.begin(9600);
  delay(1000);
}

void setup1() {
  motor_left.begin();
  motor_right.begin();
  steeringServo.begin();
  steeringServo.write(SERVO_CENTER);
}

float getYaw() {
  static unsigned long last_us = micros();
  while ((micros() - last_us) < 2000UL);
  unsigned long now = micros();
  float dt = (now - last_us) / 1000000.0f;
  last_us = now;
  float gz = imu.readGyroZ();
  if (fabs(gz) < 0.1) gz = 0;
  float y = yaw_angle + gz * dt;
  yaw_angle = y;
  return yaw_angle;
}

void turnByDegree(float start_degree, float target_degrees,
                  int speed_min   = TURN_SPEED_MIN,
                  int speed_max   = TURN_SPEED_MAX,
                  float kp        = TURN_KP,
                  float kd        = TURN_KD,
                  float tolerance = TURN_TOLERANCE) {

  float goal_yaw   = start_degree + target_degrees;
  float error      = target_degrees;
  float prev_error = error;

  steeringServo.write(error > 0 ? SERVO_RIGHT : SERVO_LEFT);
  delay(200);

  while (fabs(error) > tolerance) {
    prev_error = error;
    error = goal_yaw - getYaw();

    float derivative = error - prev_error;
    int speed = (int)constrain(
      fabs(kp * error + kd * derivative),
      (float)speed_min,
      (float)speed_max
    );

    steeringServo.write(error > 0 ? SERVO_RIGHT : SERVO_LEFT);
    motor_left.runSpeed(speed);
    motor_right.runSpeed(speed);
  }

  steeringServo.write(SERVO_CENTER);
  motor_left.runSpeed(0);
  motor_right.runSpeed(0);
}

void driveStraight(float start_degree,
                   int   drive_speed = STRAIGHT_SPEED,
                   float distance_cm = STRAIGHT_DISTANCE_CM,
                   float kp          = STRAIGHT_KP,
                   float kd          = STRAIGHT_KD,
                   float gear_ratio  = GEAR_RATIO) {

  float wheel_circ_cm = PI * (WHEEL_DIAMETER_MM / 10.0f);
  float target_ticks  = (distance_cm / wheel_circ_cm) * gear_ratio * PPR_VALUE;

  long start_left  = motor_left.getPosition();
  long start_right = motor_right.getPosition();
  float prev_error = 0.0f;

  while (true) {
    float travelled = (fabs(motor_left.getPosition()  - start_left) +
                       fabs(motor_right.getPosition() - start_right)) / 2.0f;
    if (travelled >= target_ticks) break;

    float error = getYaw() - start_degree;

    float derivative = error - prev_error;
    prev_error = error;

    float servo_pos = constrain(
      SERVO_CENTER + kp * error + kd * derivative,
      (float)SERVO_RIGHT, (float)SERVO_LEFT
    );

    steeringServo.write(servo_pos);
    motor_left.runSpeed(drive_speed);
    motor_right.runSpeed(drive_speed);
  }

  steeringServo.write(SERVO_CENTER);
  motor_left.runSpeed(0);
  motor_right.runSpeed(0);
}

void driveUntilDark(float start_degree,
                    int   corner_count  = 1,
                    int   speed         = WALL_FOLLOW_SPEED,
                    float bright_thresh = BRIGHTNESS_TURN_THRESH,
                    float too_close_mm  = WALL_TOO_CLOSE_MM,
                    float close_kp      = WALL_CLOSE_KP,
                    float gyro_kp       = STRAIGHT_KP,
                    float gyro_kd       = STRAIGHT_KD,
                    float close_kd      = WALL_CLOSE_KD) {

  float prev_gyro_error  = 0.0f;
  float prev_close_error = 0.0f;

  while (true) {
    // --- Brightness check ---
    String bright_str = requestMiddleBrightness();
    float brightness = bright_str.toFloat();
    if (brightness < bright_thresh) {
      motor_left.runSpeed(0);
      motor_right.runSpeed(0);
      steeringServo.write(SERVO_CENTER);

      // First corner: decide direction from ToF
      if (locked_turn_dir == 0) {
        float dist_left  = tof_left.read();
        float dist_right = tof_right.read();
        if (dist_left  <= 0) dist_left  = 0;
        if (dist_right <= 0) dist_right = 0;
        locked_turn_dir = (dist_left >= dist_right) ? -1 : 1;
      }

      // Turn to absolute heading: corner_count * 90 from origin (0)
      turnByDegree(start_degree, locked_turn_dir * corner_count * 90.0f);
      return;
    }

    // --- ToF close-wall PD correction ---
    float dist_left  = tof_left.read();
    float dist_right = tof_right.read();
    float correction = 0.0f;

    if (dist_left > 0 && dist_left < too_close_mm) {
      float close_error      = too_close_mm - dist_left;
      float close_derivative = close_error - prev_close_error;
      prev_close_error       = close_error;
      correction = close_kp * close_error + close_kd * close_derivative;

    } else if (dist_right > 0 && dist_right < too_close_mm) {
      float close_error      = too_close_mm - dist_right;
      float close_derivative = close_error - prev_close_error;
      prev_close_error       = close_error;
      correction = -(close_kp * close_error + close_kd * close_derivative);

    } else {
      // --- Gyro straight-hold PD ---
      prev_close_error = 0.0f;
      float gyro_error      = getYaw() - start_degree - locked_turn_dir*(corner_count-1)*90;
      float gyro_derivative = gyro_error - prev_gyro_error;
      prev_gyro_error       = gyro_error;
      Serial.println(gyro_error);
      correction = -(gyro_kp * gyro_error + gyro_kd * gyro_derivative);
    }

    float servo_pos = constrain(
      SERVO_CENTER - correction,
      (float)SERVO_RIGHT, (float)SERVO_LEFT
    );

    steeringServo.write(servo_pos);
    motor_left.runSpeed(speed);
    motor_right.runSpeed(speed);
  }
}

void wallFollow(float distance_cm = WALL_FOLLOW_DIST_CM,
                int   speed       = WALL_FOLLOW_SPEED,
                float kp          = WALL_KP,
                float kd          = WALL_KD,
                float max_corr    = WALL_MAX_CORRECTION,
                float gear_ratio  = GEAR_RATIO) {

  float wheel_circ_cm = PI * (WHEEL_DIAMETER_MM / 10.0f);
  float target_ticks  = (distance_cm / wheel_circ_cm) * gear_ratio * PPR_VALUE;

  long start_left  = motor_left.getPosition();
  long start_right = motor_right.getPosition();
  float prev_error = 0.0f;

  while (true) {
    float travelled = (fabs(motor_left.getPosition()  - start_left) +
                       fabs(motor_right.getPosition() - start_right)) / 2.0f;
    if (travelled >= target_ticks) break;

    float dist_left  = tof_left.read();
    float dist_right = tof_right.read();
    float error      = dist_left - dist_right;
    float derivative = error - prev_error;
    prev_error = error;

    float correction = constrain(kp * error + kd * derivative, -max_corr, max_corr);
    float servo_pos  = constrain(SERVO_CENTER - correction,
                                 (float)SERVO_RIGHT, (float)SERVO_LEFT);

    steeringServo.write(servo_pos);
    motor_left.runSpeed(speed);
    motor_right.runSpeed(speed);
  }

  steeringServo.write(SERVO_CENTER);
  motor_left.runSpeed(0);
  motor_right.runSpeed(0);
}

// -----------------------------------------------------------------------
// UART
// -----------------------------------------------------------------------
char m[100];
String readMessage() {
  bool started = false;
  int i = 0;
  unsigned long startTime = millis();

  while (true) {
    getYaw();
    if (mySerial.available() > 0) {
      char c = mySerial.read();
      if (c == '<') {
        started = true;
        i = 0;
      } else if (c == '*' && started) {
        m[i] = '\0';
        String result = String(m);
        Serial.println(result);
        mySerial.write('0');
        return result;
      } else if (started && i < (int)sizeof(m) - 1) {
        m[i++] = c;
      }
    }
    if (millis() - startTime > timeout) return "";
  }
}

String requestGreenBox() {
  mySerial.write('1');
  return readMessage();
}

String requestRedBox() {
  mySerial.write('2');
  return readMessage();
}

String requestMiddleBrightness() {
  mySerial.write('3');
  String msg = readMessage();
  if (msg == "") msg = "100";
  return msg;
}

String requestOrangeLine() {
  mySerial.write('4');
  String msg = readMessage();
  if (msg == "") msg = "False";
  return msg;
}

String requestBlueLine() {
  mySerial.write('5');
  String msg = readMessage();
  if (msg == "") msg = "False";
  return msg;
}

// -----------------------------------------------------------------------
// Main loop
// -----------------------------------------------------------------------
bool button_output = false;

void loop() {
  button_output = board.buttonRead();
  if (button_output) {
    // reset direction lock on each new run
    Serial.println(getYaw());
 
    float heading = getYaw();
    for(int i = 1;i<=12;i++){
      driveUntilDark(heading, i, 1000, 25, WALL_TOO_CLOSE_MM, WALL_CLOSE_KP, 4, 0.2);
    }
    driveStraight(getYaw()/abs(getYaw())*12*90,1000,8,4.0,2);
    while(true){
      motor_left.runSpeed(0);
      motor_right.runSpeed(0);
      delay(100);
    }
  } else {
    motor_left.runSpeed(0);
    motor_right.runSpeed(0);
  }
}
