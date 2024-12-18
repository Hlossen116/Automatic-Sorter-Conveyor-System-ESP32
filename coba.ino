#include <LiquidCrystal_I2C.h>

#include <Wire.h>
#include <ESP32Servo.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin untuk sensor ultrasonik 1 (untuk jarak kendaraan)
#define TRIG_PIN_1 14
#define ECHO_PIN_1 12

// Pin untuk sensor ultrasonik 2 (untuk tinggi kendaraan)
#define TRIG_PIN_2 27
#define ECHO_PIN_2 26

// Pin untuk LED
#define LED_PIN 25  // LED pada GPIO 25

// Pin untuk servo
#define SERVO_PIN 13

// Membuat objek LCD


// Membuat objek servo
Servo myServo;

// Variabel untuk sensor ultrasonik 1
long duration_1;
int distance_1;

// Variabel untuk sensor ultrasonik 2
long duration_2;
int distance_2;

// Posisi servo
int posisiTertutup = 0;
int posisiTerbuka = 90;

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
  
  // Jika tinggi kendaraan lebih dari 15cm, tidak boleh masuk
  if (distance_2 > 15) {
    lcd.clear();
    lcd.print("Kendaraan Tidak");
    lcd.setCursor(0, 0);
    lcd.print("Bisa Masuk");
    myServo.write(posisiTertutup); // Menutup palang
    digitalWrite(LED_PIN, HIGH); // Menyalakan LED
  }
  // Jika tinggi kendaraan dibawah 15cm, cek jarak kendaraan
  else {
    lcd.clear();
    lcd.print("Kendaraan Bisa");
    lcd.setCursor(0, 0);
    lcd.print("Masuk");
    
    // Membaca jarak kendaraan dari sensor ultrasonik 1
    distance_1 = getDistance(TRIG_PIN_1, ECHO_PIN_1);
    
    // Jika kendaraan mendekat (jarak <= 5cm), buka palang
    if (distance_1 <= 5) {
      myServo.write(posisiTerbuka); // Membuka palang
      digitalWrite(LED_PIN, HIGH); // Menyalakan LED
    } else {
      myServo.write(posisiTertutup); // Menutup palang jika tidak ada kendaraan dekat
      digitalWrite(LED_PIN, LOW); // Mematikan LED
    }
  }
  
  delay(500); // Delay agar sensor tidak terlalu sering membaca
  digitalWrite(LED_PIN, LOW);  // Matikan LED setelah proses
}

// Fungsi untuk menghitung jarak dari sensor ultrasonik
int getDistance(int trigPin, int echoPin) {
  // Menghantar pulsa pada trig pin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Menghitung waktu echo kembali
  duration_1 = pulseIn(echoPin, HIGH);
  
  // Menghitung jarak (cm)
  int distance = duration_1 * 0.034 / 2;
  
  return distance;
}
