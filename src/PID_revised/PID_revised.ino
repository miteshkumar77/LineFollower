
#define FW_L 7
#define BW_L 8
#define EN_L 3

#define FW_R 12
#define BW_R 13
#define EN_R 11

#define k_recover .2
#define kp 5
#define kd 10
#define ki 0
#define carSlow 80

int rightSpeed = 0;
int leftSpeed = 0;

float l_pos = 0;
float prev_l_pos = 0;

long integral = 0;

float extreme_l_pos = 1;
int n = 0;
int q = 0;

int last_l_pos = 0;




unsigned int* s = new unsigned int[6](0);
unsigned int* s_max = new unsigned int[6](0);
unsigned int* s_min = new unsigned int[6](2000);
unsigned int* s_scaled = new unsigned int [6](2000);



void setup() {
	Serial.begin(9600);
	pinMode(FW_L, OUTPUT);
	pinMode(BW_L, OUTPUT);
	pinMode(EN_L, OUTPUT);
	pinMode(FW_R, OUTPUT);
	pinMode(BW_R, OUTPUT);
	pinMode(EN_R, OUTPUT);

	calibrate_sensors();
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



void read_sensors_calib() {
    s[0] = analogRead(A5);
    s[1] = analogRead(A4);
    s[2] = analogRead(A3);
    s[3] = analogRead(A2);
    s[4] = analogRead(A1);
    s[5] = analogRead(A0);
}

void read_sensors() {
    s[0] = analogRead(A5);
    s[1] = analogRead(A4);
    s[2] = analogRead(A3);
    s[3] = analogRead(A2);
    s[4] = analogRead(A1);
    s[5] = analogRead(A0);

	s_scaled[0] = 1000 * min(max(((float)s[0] - (float)s_min[0]), 0), s_max[0] - s_min[0])/((float)(s_max[0] - s_min[0]));
    s_scaled[1] = 1000 * min(max(((float)s[1] - (float)s_min[1]), 0), s_max[1] - s_min[1])/((float)(s_max[1] - s_min[1]));
    s_scaled[2] = 1000 * min(max(((float)s[2] - (float)s_min[2]), 0), s_max[2] - s_min[2])/((float)(s_max[2] - s_min[2]));
    s_scaled[3] = 1000 * min(max(((float)s[3] - (float)s_min[3]), 0), s_max[3] - s_min[3])/((float)(s_max[3] - s_min[3]));
    s_scaled[4] = 1000 * min(max(((float)s[4] - (float)s_min[4]), 0), s_max[4] - s_min[4])/((float)(s_max[4] - s_min[4]));
    s_scaled[5] = 1000 * min(max(((float)s[5] - (float)s_min[5]), 0), s_max[5] - s_min[5])/((float)(s_max[5] - s_min[5]));
	
}

void print_s_scaled() {
	for (int i = 0; i < 6; ++i) {
		Serial.print(s_scaled[i]); Serial.print(' ');
	}
	Serial.println(' ');
}
void calibrate_sensors() {
	int i = 0;
	n = 0;
	setRM(0);
	setLM(0);

	while(n < 500){
		read_sensors_calib();
		for (i = 0; i < 6; i++) {
			if (s[i] > s_max[i]) {
				s_max[i] = s[i];
			}
			if (s[i] < s_min[i]) {
				s_min[i] = s[i];
			}
		}
		delay(10);
		++n; 
	}
	delay(3000);
}

bool check_3() {
	int j = 0;
	for (int i = 0; i < 6; ++i) {
		if (s_scaled[i] > 400) {
			++j;
		}
	}

	return (j >= 5);
}

bool check_white() {
	for (int i = 0; i < 6; ++i) {
		if (s_scaled[i] > 100) {
			return false;
		}
	}
	return true;
}

int max_arr() {
	unsigned int m = s_scaled[0];
	for (int i = 0; i < 6; i++) {
		if (s_scaled[i] > m) {
			m = s_scaled[i];
		}
	}

	return m;
}

void loop() {
	
	++q;
	read_sensors();
	l_pos =  (float)1000 * (float)(((0 * s_scaled[0]) + 
	                                (1 * s_scaled[1]) + 
									(2 * s_scaled[2]) + 
									(3 * s_scaled[3]) + 
									(4 * s_scaled[4]) + 
									(5 * s_scaled[5]))/
									(float)(s_scaled[0] + 
									        s_scaled[1] + 
											s_scaled[2] + 
											s_scaled[3] + 
											s_scaled[4] + 
											s_scaled[5])) - 2500;

	if (q > 1) { 	
		q = 0;
		// check_white()
		if (check_white()) {
			unsigned int m = max_arr();
			setLM( k_recover * (500 - m)*(extreme_l_pos)/abs(extreme_l_pos));
			setRM( -k_recover * (500 - m)*(extreme_l_pos)/abs(extreme_l_pos));
		} else {
		
			if (l_pos >= 7 || l_pos <= -7) {
				extreme_l_pos = l_pos;
			}
			// check_3()
			if (check_3()) {
				setLM(carSlow);
				setRM(carSlow);
			} else {
				setLM(max((carSlow + kp * l_pos + (l_pos - prev_l_pos) * kd - integral * ki), 30)); 
				setRM(max((carSlow - (kp * l_pos + (l_pos - prev_l_pos) * kd - integral * ki)), 30));
				integral += l_pos;
				prev_l_pos = l_pos;
			}
		}

		


		// if (l_pos >= 7 || l_pos <= -7) {
		// 	extreme_l_pos = l_pos;
		// }

		// if (check_3()) {
		// 	setLM(carSlow);
		// 	setRM(carSlow);
		// } else if (check_white()) {
		// 	setLM( k_recover * (5 - (s_scaled[0] + s_scaled[1] + s_scaled[2] + s_scaled[3] + s_scaled[4] + s_scaled[5])/6)*(extreme_l_pos)/abs(extreme_l_pos));
		// 	setRM( -k_recover * (5 - (s_scaled[0] + s_scaled[1] + s_scaled[2] + s_scaled[3] + s_scaled[4] + s_scaled[5])/6)*(extreme_l_pos)/abs(extreme_l_pos));
		// } else {
		// 	setLM(max((carSlow + kp * l_pos + (l_pos - prev_l_pos) * kd - integral * ki), 0)); 
		// 	setRM(max((carSlow - (kp * l_pos + (l_pos - prev_l_pos) * kd - integral * ki)), 0));
		// 	integral += l_pos;
		// 	prev_l_pos = l_pos;
		// }

		Serial.print(leftSpeed); Serial.print(' '); Serial.print(rightSpeed); Serial.print(' '); Serial.print(l_pos); Serial.print(' '); Serial.print(extreme_l_pos); Serial.print(" On white: "); Serial.println(check_white());
		// print_s_scaled();
	}
	// setLM(0);
	// setRM(0);

	

	
  
}
