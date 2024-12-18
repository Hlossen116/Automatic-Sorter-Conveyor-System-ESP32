#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Servo.h>  // Gunakan library Servo bawaan Arduino

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin untuk sensor ultrasonik 1 (untuk jarak kendaraan)
#define TRIG_PIN_1 30
#define ECHO_PIN_1 32

// Pin untuk sensor ultrasonik 2 (untuk tinggi kendaraan)
#define TRIG_PIN_2 34
#define ECHO_PIN_2 36

// Pin untuk LED
#define LED_PIN 38  // Pin digital biasa pada Mega 2560 Pro

// Pin untuk servo
#define SERVO_PIN 40

// Membuat objek servo
Servo myServo;

// Variabel untuk sensor ultrasonik
int distance_1;
int distance_2;

// Posisi servo
int posisiTertutup = 0;
int posisiTerbuka = 90;
int currentServoPos = posisiTertutup; // Menyimpan posisi servo saat ini

// Variabel untuk menyimpan teks terakhir
String lastLine1 = "";
String lastLine2 = "";

void setup() {
  // Memulai komunikasi serial
  Serial.begin(115200);
  lcd.init();           // Inisialisasi LCD
  lcd.backlight();
  lcd.print("Sistem Parkir");

  // Inisialisasi servo
  myServo.attach(SERVO_PIN);
  myServo.write(posisiTertutup);  // Posisi awal servo tertutup

  // Inisialisasi LED
  pinMode(LED_PIN, OUTPUT);

  // Inisialisasi pin sensor ultrasonik
  pinMode(TRIG_PIN_1, OUTPUT);
  pinMode(ECHO_PIN_1, INPUT);
  pinMode(TRIG_PIN_2, OUTPUT);
  pinMode(ECHO_PIN_2, INPUT);

  delay(2000);
  lcd.clear();
  lcd.print("Sistem siap.");
  delay(1000);
}

void loop() {
  // Membaca tinggi kendaraan dari sensor ultrasonik 2
  distance_2 = getDistance(TRIG_PIN_2, ECHO_PIN_2);

  String line1, line2;

  if (distance_2 > 15) {
    line1 = "Kendaraan Tidak";
    line2 = "Bisa Masuk";
    if (currentServoPos != posisiTertutup) {
      myServo.write(posisiTertutup); // Menutup palang
      currentServoPos = posisiTertutup;
    }
    digitalWrite(LED_PIN, HIGH); // Menyalakan LED
  } else {
    line1 = "Kendaraan Bisa";
    line2 = "Masuk";
    distance_1 = getDistance(TRIG_PIN_1, ECHO_PIN_1);
    if (distance_1 <= 5) {
      if (currentServoPos != posisiTerbuka) {
        myServo.write(posisiTerbuka); // Membuka palang
        currentServoPos = posisiTerbuka;
      }
      digitalWrite(LED_PIN, HIGH); // Menyalakan LED
    } else {
      if (currentServoPos != posisiTertutup) {
        myServo.write(posisiTertutup); // Menutup palang
        currentServoPos = posisiTertutup;
      }
      digitalWrite(LED_PIN, LOW); // Mematikan LED
    }
  }

  // Hanya perbarui LCD jika teks berubah
  if (line1 != lastLine1 || line2 != lastLine2) {
    lcd.clear();
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    lastLine1 = line1;
    lastLine2 = line2;
  }

  delay(500); // Delay agar sensor tidak terlalu sering membaca
}

// Fungsi untuk menghitung jarak dari sensor ultrasonik
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);

  int distance = duration * 0.034 / 2;
  return distance;
}
