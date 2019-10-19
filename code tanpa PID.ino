// IR Sensors
int sensor1 = 2;      // Sensor Paling Kiri
int sensor2 = 3;
int sensor3 = 4;
int sensor4 = 5;      // Sensor Paling Kanan

// inisial Nilai Sensor
int sensor[4] = {0, 0, 0, 0};

// Pin Motor dan PWM
int ENA = 6;
int motorInput1 = 7;
int motorInput2 = 8;
int motorInput3 = 9;
int motorInput4 = 10;
int ENB = 11;

//Inisial Kecepatan motor
int initial_motor_speed = 140; // kalibrasi kecepatan motor

// Indikator Output Pin LED
int ledPin1 = A3;
int ledPin2 = A4;

int flag = 0;


float error = 0, PID_value = 100;

void setup()
{
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);

  pinMode(motorInput1, OUTPUT);
  pinMode(motorInput2, OUTPUT);
  pinMode(motorInput3, OUTPUT);
  pinMode(motorInput4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);

  Serial.begin(9600);
  delay(500);
  Serial.println("Started !!");
  delay(1000);
}

void loop()
{
  read_sensor_values();
  Serial.print(error);
  if (error == 100) {               // Belok kiri sampai mendeteksi jalan lurus
    //Serial.print("\t");
    //Serial.println("Left");
    do {
      read_sensor_values();
      analogWrite(ENA, 110); // Kecepatan Motor Kiri
      analogWrite(ENB, 90); // Kecepatan Motor Kanan
      belok_kiri_tajam();
    } while (error != 0);

  } else if (error == 101) {          // Belok kanan jika mendeteksi hanya jalan yang benar (itu akan menuju ke arah depan jika lurus dan kanan "| -")
                                      // sampai mendeteksi jalan lurus.
    //Serial.print("\t");
    //Serial.println("Right");
    analogWrite(ENA, 110); // Kecepatan Motor Kiri
    analogWrite(ENB, 90); // Kecepatan Motor Kanan
    maju();
    delay(200);
    berhenti();
    read_sensor_values();
    if (error == 102) {
      do {
        analogWrite(ENA, 110); // Kecepatan Motor Kiri
        analogWrite(ENB, 90); // Kecepatan Motor Kanan
        belok_kanan_tajam();
        read_sensor_values();
      } while (error != 0);
    }
  } else if (error == 102) {        // Belok kiri sampai mendeteksi jalan lurus
    //Serial.print("\t");
    //Serial.println("Belok tajam ke kiri");
    do {
      analogWrite(ENA, 110); // Kecepatan Motor Kiri
      analogWrite(ENB, 90); // Kecepatan Motor Kanan
      belok_kiri_tajam();
      read_sensor_values();
      if (error == 0) {
        berhenti();
        delay(200);
      }
    } while (error != 0);
  } else if (error == 103) {        // Belok kiri sampai mendeteksi jalan lurus atau berhenti jika jalan buntu tercapai.
    if (flag == 0) {
      analogWrite(ENA, 110); // Kecepatan Motor Kiri
      analogWrite(ENB, 90); // Kecepatan Motor Kanan
      maju();
      delay(200);
      berhenti();
      read_sensor_values();
      if (error == 103) {     /**** Jalan Buntu Tercapai, Berhenti! ****/
        berhenti();
        digitalWrite(ledPin1, HIGH);
        digitalWrite(ledPin2, HIGH);
        flag = 1;
      } else {        /**** Bergerak ke kiri ****/
        analogWrite(ENA, 110); // Kecepatan Motor Kiri
        analogWrite(ENB, 90); // Kecepatan Motor Kanan
        belok_kiri_tajam();
        delay(200);
        do {
          //Serial.print("\t");
          //Serial.println("Left Here");
          read_sensor_values();
          analogWrite(ENA, 110); // Kecepatan Motor Kiri
          analogWrite(ENB, 90); // Kecepatan Motor Kanan
          belok_kiri_tajam();
        } while (error != 0);
      }
    }
  } else {
    motor_control();
  }
}

void read_sensor_values()
{
  sensor[0] = !digitalRead(sensor1);
  sensor[1] = !digitalRead(sensor2);
  sensor[2] = !digitalRead(sensor3);
  sensor[3] = !digitalRead(sensor4);

  /*
    Serial.print(sensor[0]);
    Serial.print("\t");
    Serial.print(sensor[1]);
    Serial.print("\t");
    Serial.print(sensor[2]);
    Serial.print("\t");
    Serial.println(sensor[3]);*/

  if ((sensor[0] == 1) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 0))
    error = 3;
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 0) && (sensor[3] == 0))
    error = 2;
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[2] == 0) && (sensor[3] == 0))
    error = 1;
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 0))
    error = 0;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 1) && (sensor[3] == 0))
    error = -1;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 1) && (sensor[3] == 1))
    error = -2;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 1))
    error = -3;
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 0)) // Putar robot ke kiri
    error = 100;
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 1)) // Putar robot ke kanan
    error = 101;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 0)) // Putar Balik
    error = 102;
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 1)) // Belok kiri atau berhenti
    error = 103;
}

void motor_control()
{
  // Menghitung kecepatan motor efektif:
  int kecepatan_motor_kiri = initial_motor_speed;
  int kecepatan_motor_kanan = initial_motor_speed;

  // Kecepatan motor tidak boleh melebihi nilai PWM maks
  kecepatan_motor_kiri = constrain(kecepatan_motor_kiri, 0, 255);
  kecepatan_motor_kanan = constrain(kecepatan_motor_kanan, 0, 255);

  /*Serial.print(PID_value);
    Serial.print("\t");
    Serial.print(kecepatan_motor_kiri);
    Serial.print("\t");
    Serial.println(kecepatan_motor_kanan);*/

  analogWrite(ENA, kecepatan_motor_kiri); //Kecepatan Motor Kiri
  analogWrite(ENB, kecepatan_motor_kanan - 30); //Kecepatan Motor Kanan

  // Robot Maju
  maju();
}

void maju()
{
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, HIGH);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, HIGH);
}

void mundur()
{
  digitalWrite(motorInput1, HIGH);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, HIGH);
  digitalWrite(motorInput4, LOW);
}

void belok_kanan()
{
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, HIGH);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, LOW);
}

void belok_kiri()
{
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, HIGH);
}

void belok_kanan_tajam() {
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, HIGH);
  digitalWrite(motorInput3, HIGH);
  digitalWrite(motorInput4, LOW);
}

void belok_kiri_tajam() {
  digitalWrite(motorInput1, HIGH);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, HIGH);
}

void berhenti()
{
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, LOW);
}
