#define LTR analogRead(A3) <= 100
#define LTMR A0 
#define LTML A2
#define LTL analogRead(A1) <= 100

#define FW_L 8
#define BW_L 7
#define EN_L 5

#define FW_R 9
#define BW_R 11
#define EN_R 6


#define carSlow 80
#define carSpeed 150
#define kturn .4
#define kp .4 // .4 // .4
#define kd 0 // 1 // 20
#define ki 0  // 1 // 0

int rightSpeed = 0;
int leftSpeed = 0;

float sum_hue = 0;
int avg_hue = 0;
int prev_avg_hue = 0;


float l_pos = 0;
int prev_l_pos = 0;

float integral = 0;

bool last_was_left = true;

int  n = 0;
int  q = 0;

void setup() {

  Serial.begin(9600);
  pinMode(FW_L, OUTPUT);
  pinMode(BW_L, OUTPUT);
  pinMode(EN_L, OUTPUT);
  pinMode(FW_R, OUTPUT);
  pinMode(BW_R, OUTPUT);
  pinMode(EN_R, OUTPUT);
  
  
}

void forward() {
  analogWrite(EN_R, carSpeed);
  analogWrite(EN_L, carSpeed);
  digitalWrite(FW_R, HIGH);
  digitalWrite(FW_L, HIGH);
  digitalWrite(BW_R, LOW);
  digitalWrite(BW_L, LOW);
}

void stop() {
  analogWrite(EN_R, 0);
  analogWrite(EN_L, 0);
}

void left() {
  analogWrite(EN_R, 150);
  analogWrite(EN_L, 0);
  digitalWrite(FW_R, HIGH);
  digitalWrite(FW_L, LOW);
  digitalWrite(BW_R, LOW);
  digitalWrite(BW_L, HIGH);
}

void right() {
  analogWrite(EN_R, 0);
  analogWrite(EN_L, 150);
  digitalWrite(FW_R, LOW);
  digitalWrite(FW_L, HIGH);
  digitalWrite(BW_R, HIGH);
  digitalWrite(BW_L, LOW);
}

void setRM(int s) {
  analogWrite(EN_R, min(abs(s), 140));
  if (s >= 0) {
    digitalWrite(FW_R, HIGH);
    digitalWrite(BW_R, LOW);
    rightSpeed = min(s, 140);
  } else {
    rightSpeed = max(-140, s);
    digitalWrite(FW_R, LOW);
    digitalWrite(BW_R, HIGH);
  }
}

void setLM(int s) {
  analogWrite(EN_L, min(abs(s), 140));
  if (s >= 0) {
    leftSpeed = min(s, 140);
    digitalWrite(FW_L, HIGH);
    digitalWrite(BW_L, LOW);
  } else {

    leftSpeed = max(s, -140);
    digitalWrite(FW_L, LOW);
    digitalWrite(BW_L, HIGH);
  }
}

int extreme_l_pos = 0;

void loop() {

 // REFERENCES
 // true  -> white
 // false -> black
 // 1 = prev_left;
 // 2 = prev_right;
 // 0 = prev_straight;

// PRINT DEBUGS
// ln("%d %d %d %d", analogRead(A1), analogRead(A2), analogRead(A0), analogRead(A3));
//  Serial.print(analogRead(A1)); Serial.print(' '); Serial.print(analogRead(A2)); Serial.print(' '); Serial.print(analogRead(A0)); Serial.print(' '); Serial.print(analogRead(A3)); Serial.print(' '); Serial.print(analogRead(A4)); Serial.print(' '); Serial.print(analogRead(A5)); Serial.println("");

  unsigned int s1 = analogRead(A1);
  unsigned int s2 = analogRead(A2);
  unsigned int s3 = analogRead(A0);
  unsigned int s4 = analogRead(A3);
  unsigned int s5 = analogRead(A4);
  unsigned int s6 = analogRead(A5);

  unsigned int k1 = s1;
  unsigned int k2 = s2;
  unsigned int k3 = s3;
  unsigned int k4 = s4;
  unsigned int k5 = s5;
  unsigned int k6 = s6;
  
  if (s1 <= 50) {
    s1 = 1;
  }

  if (s2 <= 50) {
    s2 = 1;
  }
  
  if (s3 <= 50) {
    s3 = 1;
  }
  
  if (s4 <= 50) {
    s4 = 1;
  }

  if (s5 <= 50) {
    s5 = 1;
  }

  if (s6 <= 50) {
    s6 = 1;
  }

  l_pos =  (float)1000 * (float)((0 * s1 + (1 * s2) + (2 * s3) + (3 * s4) + (4 * s5) + (5 * s6))/(float)(s1 + s2 + s3 + s4 + s5 + s6)) - 2500;

      if (l_pos >= 500 || l_pos <= -500) {
        extreme_l_pos = l_pos;
      }

       if (s1 > 100 && s2 > 100 && s3 > 100 && s4 > 100 && s5 > 100 && s6 > 100) {
         setLM(carSlow); 
         setRM(carSlow);
       } else if (((s1 + s2 + s3 + s4 + s5 + s6)/6) < 100) {
         setLM( kturn * (500 - (s1 + s2 + s3 + s4 + s5 + s6)/6)*(extreme_l_pos)/abs(extreme_l_pos));
         setRM( -kturn * (500 - (s1 + s2 + s3 + s4 + s5 + s6)/6)*(extreme_l_pos)/abs(extreme_l_pos));
        } else {
         setLM(max((carSlow + kp * l_pos + (l_pos - prev_l_pos) * kd - integral * ki), 0)); 
         setRM(max((carSlow - (kp * l_pos + (l_pos - prev_l_pos) * kd - integral * ki)), 0));
         integral += l_pos;
         prev_l_pos = l_pos;
       }

// PRINT DEBUGS
//       Serial.print(leftSpeed); Serial.print(' ');
//       Serial.print(rightSpeed); Serial.println("");

 
}
