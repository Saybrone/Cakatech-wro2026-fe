#include <EVN.h>
#include <SoftwareSerial.h>
#include <math.h>

// -----------------------------------------------------------------------
// Port assignments
// -----------------------------------------------------------------------
#define SERVO_DIR_PORT        1
#define IMU_I2C_PORT          1
#define TOF_LEFT_PORT         3
#define TOF_RIGHT_PORT        2
#define PPR_VALUE             2200

// -----------------------------------------------------------------------
// Servo positions
// -----------------------------------------------------------------------
const float SERVO_CENTER = 133;
const int   SERVO_RIGHT  = 70;
const int   SERVO_LEFT   = 210;

// -----------------------------------------------------------------------
// Turn PD tuning
// -----------------------------------------------------------------------
#define TURN_KP               16.0f
#define TURN_KD               0.8f
#define TURN_SPEED_MAX        1000
#define TURN_SPEED_MIN        400
#define TURN_TOLERANCE        1.0f

// -----------------------------------------------------------------------
// Straight drive PD tuning
// -----------------------------------------------------------------------
#define STRAIGHT_KP           8.0f
#define STRAIGHT_KD           0.4f
#define STRAIGHT_SPEED        1000
#define STRAIGHT_DISTANCE_CM  50.0f

// -----------------------------------------------------------------------
// Wall / proximity config
// -----------------------------------------------------------------------
#define WALL_FOLLOW_SPEED     800
#define WALL_KP               0.5f
#define WALL_KD               0.02f
#define WALL_MAX_CORRECTION   60.0f
#define NO_WALL_MM            10000.0f
#define WALL_TOO_CLOSE_MM     50.0f
#define WALL_CLOSE_KP         1.2f
#define WALL_CLOSE_KD         0.05f
// -----------------------------------------------------------------------
// Pillar avoidance config
// -----------------------------------------------------------------------
// When a pillar error (pixels from center) exceeds this, start correcting
#define PILLAR_ERROR_THRESHOLD   300
// How strongly to steer toward correct side per pixel of error
#define PILLAR_STEER_KP          3.5f
// Minimum pillar area (pixels^2) to act on — filters distant/noise blobs
#define PILLAR_MIN_AREA          50
// Speed while actively avoiding a pillar
#define PILLAR_AVOID_SPEED       800
// Normal cruising speed between pillars
#define CRUISE_SPEED             800

// -----------------------------------------------------------------------
// Lap / direction config
// -----------------------------------------------------------------------
// Total corners to navigate for 3 laps
#define TOTAL_CORNERS            12
// Brightness threshold: below this means wall (corner) detected
#define BRIGHTNESS_WALL_THRESH   30.0f

// -----------------------------------------------------------------------
// OpenMV UART
// -----------------------------------------------------------------------
#define OPENMV_RX_PIN  1
#define OPENMV_TX_PIN  0
#define UART_TIMEOUT   500

// -----------------------------------------------------------------------
// Wheel geometry
// -----------------------------------------------------------------------
#define WHEEL_DIAMETER_MM     43.2f
#define GEAR_RATIO            1.0f

// -----------------------------------------------------------------------
// Hardware objects
// -----------------------------------------------------------------------
EVNAlpha          board;
EVNMotor          motor_left(1, EV3_MED);
EVNMotor          motor_right(2, EV3_MED);
EVNServo          steeringServo(SERVO_DIR_PORT);
EVNIMUSensor      imu(IMU_I2C_PORT);
EVNDistanceSensor tof_left(TOF_LEFT_PORT);
EVNDistanceSensor tof_right(TOF_RIGHT_PORT);
SoftwareSerial    openmv(OPENMV_RX_PIN, OPENMV_TX_PIN);
int locked_turn_dir = 0;
// -----------------------------------------------------------------------
// Shared gyro state
// -----------------------------------------------------------------------
volatile float yaw_angle = 0.0f;

// -----------------------------------------------------------------------
// Driving direction: +1 = clockwise (orange seen first, turn right = +90)
//                   -1 = counter-clockwise (blue seen first, turn left = -90)
// -----------------------------------------------------------------------
int drive_direction = 0;  // determined at start

// -----------------------------------------------------------------------
// setup / setup1
// -----------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  board.begin();
  board.setLinkLED(true);
  board.setMode(BUTTON_TOGGLE);
  imu.begin();
  imu.setGyroRange(IMU_GYRO_DPS_1000);
  tof_left.begin();
  tof_right.begin();
  openmv.begin(9600);
  delay(1000);
}

void setup1() {
  motor_left.begin();
  motor_right.begin();
  steeringServo.begin();
  steeringServo.write(SERVO_CENTER);
}

// -----------------------------------------------------------------------
// getYaw — fixed-rate gyro integration
// -----------------------------------------------------------------------
float getYaw() {
  static unsigned long last_us = micros();

  while ((micros() - last_us) < 2000UL);
  unsigned long now = micros();
  float dt = (now - last_us) / 1000000.0f;
  last_us = now;

  float gz = imu.readGyroZ();
  if (fabs(gz) < 0.1f) gz = 0;

  float y = yaw_angle + gz * dt;
  if (y >  180.0f) y -= 360.0f;
  if (y < -180.0f) y += 360.0f;
  yaw_angle = y;
  return yaw_angle;
}
void turnByDegreeReverse(float start_degree, float target_degrees,
                         int speed_min   = TURN_SPEED_MIN,
                         int speed_max   = TURN_SPEED_MAX,
                         float kp        = TURN_KP,
                         float kd        = TURN_KD,
                         float tolerance = TURN_TOLERANCE) {

  float goal_yaw = start_degree + target_degrees;
  float error = goal_yaw - getYaw();
  float prev_error = error;

  // Reverse steering logic
  steeringServo.write(error > 0 ? SERVO_LEFT : SERVO_RIGHT);
  delay(200);

  while (fabs(error) > tolerance) {

    prev_error = error;

    error = goal_yaw - getYaw();
    if (error > 180.0f) error -= 360.0f;
    if (error < -180.0f) error += 360.0f;

    float derivative = error - prev_error;

    int speed = (int)constrain(
      fabs(kp * error + kd * derivative),
      (float)speed_min,
      (float)speed_max
    );

    // Steering reversed because car is backing up
    steeringServo.write(error > 0 ? SERVO_LEFT : SERVO_RIGHT);

    motor_left.runSpeed(-speed);
    motor_right.runSpeed(-speed);
  }

  steeringServo.write(SERVO_CENTER);
  motor_left.runSpeed(0);
  motor_right.runSpeed(0);
}
// -----------------------------------------------------------------------
// turnByDegree — PD, full servo lock
// -----------------------------------------------------------------------
void turnByDegree(float start_degree, float target_degrees,
                  int speed_min   = TURN_SPEED_MIN,
                  int speed_max   = TURN_SPEED_MAX,
                  float kp        = TURN_KP,
                  float kd        = TURN_KD,
                  float tolerance = TURN_TOLERANCE) {

  float goal_yaw   = start_degree + target_degrees;
  float error = goal_yaw - getYaw();
  float prev_error = error;

  steeringServo.write(error > 0 ? SERVO_RIGHT : SERVO_LEFT);
  delay(200);

  while (fabs(error) > tolerance ) {
    prev_error = error;
    error = goal_yaw - getYaw();
    if (error >  180.0f) error -= 360.0f;
    if (error < -180.0f) error += 360.0f;

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
int timeout = 500;
// -----------------------------------------------------------------------
// driveStraight — PD, encoder distance
// -----------------------------------------------------------------------
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
    if (error >  180.0f) error -= 360.0f;
    if (error < -180.0f) error += 360.0f;

    float derivative = error - prev_error;
    prev_error = error;

    float servo_pos = constrain(
      SERVO_CENTER + kp * error + kd * derivative,
      (float)SERVO_RIGHT,
      (float)SERVO_LEFT
    );

    steeringServo.write(servo_pos);
    motor_left.runSpeed(drive_speed);
    motor_right.runSpeed(drive_speed);
  }

  steeringServo.write(SERVO_CENTER);
  motor_left.runSpeed(0);
  motor_right.runSpeed(0);
}

// -----------------------------------------------------------------------
// OpenMV communication helpers
// -----------------------------------------------------------------------
char m[100];
String readMessage() {
  bool started = false;
  int i = 0;
  unsigned long startTime = millis();

  while (true) {
    getYaw();
    if (openmv.available() > 0) {
      char c = openmv.read();
      if (c == '<') {
        started = true;
        i = 0;
      } else if (c == '*' && started) {
        m[i] = '\0';
        String result = String(m);
        Serial.println(result);
        openmv.write('0');
        return result;
      } else if (started && i < (int)sizeof(m) - 1) {
        m[i++] = c;
      }
    }
    if (millis() - startTime > timeout) return "";
  }
}

String requestGreen() {
  openmv.write('1');
  return readMessage();
}

String requestRed() {
  openmv.write('2');
  return readMessage();
}

String requestBrightness() {
  openmv.write('3');
  String msg = readMessage();
  if (msg == "") msg = "100";
  return msg;
}

String requestOrange() {
  openmv.write('4');
  String msg = readMessage();
  if (msg == "") msg = "False";
  return msg;
}

String requestBlue() {
  openmv.write('5');
  String msg = readMessage();
  if (msg == "") msg = "False";
  return msg;
}

// -----------------------------------------------------------------------
// parsePillarResponse
// Parses OpenMV response like "[3200, -45]?[1800, 12]?" into the
// largest-area blob's area and pixel error from center.
// Returns false if no valid blob found.
// -----------------------------------------------------------------------
bool parsePillarResponse(String response, int &area, int &error) {
  area  = 0;
  error = 0;

  if (response.length() == 0) return false;

  int best_area  = 0;
  int best_error = 0;
  bool found = false;

  int idx = 0;
  while (idx < (int)response.length()) {
    int open  = response.indexOf('[', idx);
    int close = response.indexOf(']', idx);
    if (open < 0 || close < 0) break;

    String entry = response.substring(open + 1, close);
    int comma = entry.indexOf(',');
    if (comma > 0) {
      int a = entry.substring(0, comma).toInt();
      int e = entry.substring(comma + 1).toInt();
      if (a > best_area) {
        best_area  = a;
        best_error = e;
        found = true;
      }
    }
    idx = close + 1;
  }

  if (found) {
    area  = best_area;
    error = best_error;
  }
  return found;
}

// -----------------------------------------------------------------------
// detectDirection
// Determines CW (+1) or CCW (-1) by looking for orange or blue line.
// Drives slowly forward until a line is seen.
// Returns +1 (orange/clockwise) or -1 (blue/counter-clockwise).
// -----------------------------------------------------------------------
int detectDirection() {
  float start_yaw = getYaw();

  while (true) {
    float error = getYaw() - start_yaw;
    if (error >  180.0f) error -= 360.0f;
    if (error < -180.0f) error += 360.0f;

    steeringServo.write(constrain(
      SERVO_CENTER + STRAIGHT_KP * error,
      (float)SERVO_RIGHT,
      (float)SERVO_LEFT
    ));
    motor_left.runSpeed(400);
    motor_right.runSpeed(400);

    if (requestOrange() == "True") {
      motor_left.runSpeed(0);
      motor_right.runSpeed(0);
      return 1;
    }
    if (requestBlue() == "True") {
      motor_left.runSpeed(0);
      motor_right.runSpeed(0);
      return -1;
    }
  }
}

// -----------------------------------------------------------------------
// avoidPillar
// Called when a pillar is visible ahead.
// Red pillar:   must pass to the RIGHT of robot → steer left to go around
// Green pillar: must pass to the LEFT of robot  → steer right to go around
//
// error from OpenMV: negative = pillar is left of camera center
//                    positive = pillar is right of camera center
void avoidPillar(bool is_red, int pillar_error, float heading) {
  unsigned long start_ms = millis();

  while (true) {
    String response = is_red ? requestRed() : requestGreen();
    int area = 0, px_error = 0;
    bool visible = parsePillarResponse(response, area, px_error);

    if (!visible || area < PILLAR_MIN_AREA) break;
    if (millis() - start_ms > 3000) break;

    float correction;
    if (is_red) {
      // RED: push pillar to the right → steer left (negative correction)
      correction = PILLAR_STEER_KP * (-PILLAR_ERROR_THRESHOLD - px_error);
    } else {
      // GREEN: push pillar to the left → steer right (positive correction)
      correction = PILLAR_STEER_KP * (PILLAR_ERROR_THRESHOLD - px_error);
    }

    // Wall avoidance on top of pillar steering
    float dist_left  = tof_left.read();
    float dist_right = tof_right.read();

    if (dist_left > 0 && dist_left < WALL_TOO_CLOSE_MM) {
      correction += WALL_CLOSE_KP * (WALL_TOO_CLOSE_MM - dist_left);   // steer right
    } else if (dist_right > 0 && dist_right < WALL_TOO_CLOSE_MM) {
      correction -= WALL_CLOSE_KP * (WALL_TOO_CLOSE_MM - dist_right);  // steer left
    }

    // Light gyro heading hold
    float gyro_err = getYaw() - heading;
    if (gyro_err >  180.0f) gyro_err -= 360.0f;
    if (gyro_err < -180.0f) gyro_err += 360.0f;

    float servo_pos = constrain(
      SERVO_CENTER + correction + STRAIGHT_KP * gyro_err * 0.3f,
      (float)SERVO_RIGHT, (float)SERVO_LEFT
    );

    steeringServo.write(servo_pos);
    motor_left.runSpeed(PILLAR_AVOID_SPEED);
    motor_right.runSpeed(PILLAR_AVOID_SPEED);
  }
}
// For RED (pass on right):
//   We want the pillar to end up on our right → steer LEFT (negative servo)
//   Target: keep error > +threshold (pillar stays right of center)
//
// For GREEN (pass on left):
//   We want the pillar to end up on our left → steer RIGHT (positive servo)
//   Target: keep error < -threshold (pillar stays left of center)
// -----------------------------------------------------------------------
bool checkAndAvoidPillar1(float heading) {
    // Get red detection
    String redResp = requestRed();
    int redArea = 0, redPxErr = 0;
    bool redDetected =
        parsePillarResponse(redResp, redArea, redPxErr) &&
        redArea >= PILLAR_MIN_AREA;

    // Get green detection
    String greenResp = requestGreen();
    int greenArea = 0, greenPxErr = 0;
    bool greenDetected =
        parsePillarResponse(greenResp, greenArea, greenPxErr) &&
        greenArea >= PILLAR_MIN_AREA;

    // Nothing detected
    if (!redDetected && !greenDetected) {
        return false;
    }

    // Choose the larger pillar
    if (redDetected && (!greenDetected || redArea >= greenArea)) {
        Serial.println("RED pillar selected (larger area), passing on right");

        avoidPillar(true, redPxErr, heading);

        driveStraight(heading, 800, 3.0, 0.5, 0.01, 1);
        turnByDegree(0, heading);
        delay(1000);
        driveStraight(heading,800,2,0.5,0.01,1);
        turnByDegree(0, heading - 60);
        turnByDegree(0, heading);

        return true;
    }

    // Green must be larger
    Serial.println("GREEN pillar selected (larger area), passing on left");

    avoidPillar(false, greenPxErr, heading);

    driveStraight(heading, 800, 3.0, 0.5, 0.01, 1);
    turnByDegree(0, heading);
    delay(1000);
    driveStraight(heading,800,2,0.5,0.01,1);
    turnByDegree(0, heading + 60);
    turnByDegree(0, heading);

    return true;
}
// -----------------------------------------------------------------------
// driveUntilCorner
// Drives forward avoiding any visible pillars until the camera detects
// a wall ahead (brightness < threshold), then stops.
// Returns immediately so the caller can do the corner turn.
// -----------------------------------------------------------------------
void driveUntilDark(float start_degree,
                    bool is_back=false,
                    int   corner_count  = 1,
                    int   speed         = WALL_FOLLOW_SPEED,
                    float bright_thresh = BRIGHTNESS_WALL_THRESH,
                    float too_close_mm  = WALL_TOO_CLOSE_MM,
                    float close_kp      = WALL_CLOSE_KP,
                    float gyro_kp       = STRAIGHT_KP,
                    float gyro_kd       = STRAIGHT_KD,
                    float close_kd      = WALL_CLOSE_KD) {

  float prev_gyro_error  = 0.0f;
  float prev_close_error = 0.0f;

  while (true) {
    // --- Brightness check ---
    String bright_str = requestBrightness();
    float brightness = bright_str.toFloat();
    String redResp = requestRed();
    int redArea = 0, redPxErr = 0;

    String greenResp = requestGreen();
    int greenArea = 0, greenPxErr = 0;

    bool redDetected =parsePillarResponse(redResp, redArea, redPxErr) && redArea >= PILLAR_MIN_AREA;
    bool greenDetected = parsePillarResponse(greenResp, greenArea, greenPxErr) && greenArea >= PILLAR_MIN_AREA;

    if (brightness < bright_thresh && !redDetected && !greenDetected) {
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
      if (is_back){
        turnByDegreeReverse(start_degree, locked_turn_dir * corner_count * 90.0f);
      }else{
        turnByDegree(start_degree, locked_turn_dir * corner_count * 90.0f);
      }
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

void pass_line(float heading){
  String grn_resp = requestGreen();
  int area = 0, px_err = 0;
  checkAndAvoidPillar1(heading);
  checkAndAvoidPillar1(heading);
  driveUntilDark(heading,false);
  driveStraight(heading+locked_turn_dir*90,-800,3,0.5,0.01,1);
}
// -----------------------------------------------------------------------
// parkingSequence
// After 3 laps the robot needs to find and enter the parking lot.
// The parking lot is in the starting section; the robot drives back
// toward it using gyro, detects the magenta markers with the ToF sensors,
// and performs parallel parking:
//   1. Drive to align past the far marker
//   2. Turn into the lot
//   3. Reverse in
//   4. Straighten up parallel to the wall
// -----------------------------------------------------------------------
// -----------------------------------------------------------------------
bool button_output = false;

void loop() {
  button_output = board.buttonRead(); 
  if (button_output) {
    float heading = getYaw();
    driveUntilDark(heading,false);
    driveStraight(heading+locked_turn_dir*90,-800,4,0.5,0.01,1);
    for(int i = 1;i<=12;i++){
      pass_line(heading+locked_turn_dir*90*i);
      delay(1000);
    }

    
    motor_left.runSpeed(0);
    motor_right.runSpeed(0);
    while(true){
      delay(100);
    }

  } else {
    motor_left.runSpeed(0);
    motor_right.runSpeed(0);
  }
}
