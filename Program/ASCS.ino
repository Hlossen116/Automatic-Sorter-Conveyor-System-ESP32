#include <Wire.h>
#include <ESP32Servo.h>
#include <ESP32PWM.h>
#include <LiquidCrystal_I2C.h> // Tambahkan pustaka LCD


LiquidCrystal_I2C lcd(0x27, 16, 2);
// Definisi Pin
const int PIN_TRIG = 26;  
const int PIN_ECHO = 27;  
const int PIN_SERVO = 25; 
const int MOTOR_MASUK1 = 32; 
const int MOTOR_MASUK2 = 33; 
const int MOTOR_EN = 23;

// Pin untuk sensor penuh pada wadah
const int PIN_TRIG_PENUH_LEBIH = 14;  // untuk wadah > 5 cm
const int PIN_ECHO_PENUH_LEBIH = 12;  // untuk wadah > 5 cm
const int PIN_TRIG_PENUH_SAMA = 18;   // untuk wadah <= 5 cm
const int PIN_ECHO_PENUH_SAMA = 19;   // untuk wadah <= 5 cm

// Objek Servo
Servo servoPenyortir;

// Konstanta
const float BATAS_TINGGI = 5.0; // (cm)
const float JARAK_PENUH = 3.0;  // Jarak untuk mendeteksi apakah wadah penuh (cm)

// Fungsi untuk mengukur jarak dengan sensor ultrasonik
float ukurJarak(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long durasi = pulseIn(echoPin, HIGH);
  float jarak = durasi * 0.034 / 2; // Konversi ke cm
  return jarak;
}

// Fungsi untuk mengontrol motor ban berjalan
void kontrolBanBerjalan(bool status) {
  if (status) {
    digitalWrite(MOTOR_MASUK1, HIGH);
    digitalWrite(MOTOR_MASUK2, LOW);
    analogWrite(MOTOR_EN, 150); // Kecepatan motor
  } else {
    digitalWrite(MOTOR_MASUK1, LOW);
    digitalWrite(MOTOR_MASUK2, LOW);
    analogWrite(MOTOR_EN, 0);
  }
}

// Fungsi untuk menyortir objek berdasarkan tinggi
void sortirObjek(float tinggi) {
  if (tinggi > BATAS_TINGGI) {
    servoPenyortir.write(30); // untuk objek > 5 cm
  } else {
    servoPenyortir.write(150); // untuk objek <= 5 cm
  }
  delay(1000);
  servoPenyortir.write(90); // Kembali ke posisi netral
  delay(500);
}

void setup() {
  // Inisialisasi komunikasi serial
  Serial.begin(115200);
lcd.init();           // Inisialisasi LCD
  lcd.backlight();      // Menghidupkan lampu latar LCD
  // Inisialisasi pin
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(MOTOR_MASUK1, OUTPUT);
  pinMode(MOTOR_MASUK2, OUTPUT);
  pinMode(MOTOR_EN, OUTPUT);

  // Pin untuk sensor penuh
  pinMode(PIN_TRIG_PENUH_LEBIH, OUTPUT);
  pinMode(PIN_ECHO_PENUH_LEBIH, INPUT);
  pinMode(PIN_TRIG_PENUH_SAMA, OUTPUT);
  pinMode(PIN_ECHO_PENUH_SAMA, INPUT);

  // Inisialisasi Servo
  servoPenyortir.attach(PIN_SERVO);
  servoPenyortir.write(90); // Posisi netral

  // Inisialisasi LCD
  lcd.begin(16, 2); // LCD 16x2
  lcd.print("Sistem Sorting");

  // Mulai ban berjalan
  kontrolBanBerjalan(true);
}

void loop() {
  // Mengukur tinggi objek
  float tinggiObjek = ukurJarak(PIN_TRIG, PIN_ECHO);
  Serial.print("Tinggi Terdeteksi: ");
  Serial.print(tinggiObjek);
  Serial.println(" cm");

  // Tampilkan tinggi objek ke LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tinggi: ");
  lcd.print(tinggiObjek);
  lcd.print(" cm");

  // Mengukur apakah wadah penuh
  float jarakPenuhLebih = ukurJarak(PIN_TRIG_PENUH_LEBIH, PIN_ECHO_PENUH_LEBIH);
  float jarakPenuhSama = ukurJarak(PIN_TRIG_PENUH_SAMA, PIN_ECHO_PENUH_SAMA);

  Serial.print("Jarak Wadah Lebih > 5cm: ");
  Serial.print(jarakPenuhLebih);
  Serial.println(" cm");
  Serial.print("Jarak Wadah Sama <= 5cm: ");
  Serial.print(jarakPenuhSama);
  Serial.println(" cm");

  // Tampilkan status ke LCD
  lcd.setCursor(0, 1);
  if (jarakPenuhLebih <= JARAK_PENUH) {
    lcd.print(">5cm Penuh");
    kontrolBanBerjalan(false);
    return; // Tidak melanjutkan proses jika wadah penuh
  }

  if (jarakPenuhSama <= JARAK_PENUH) {
    lcd.print("<=5cm Penuh");
    kontrolBanBerjalan(false);
    return; 
  }

  // Jika objek terdeteksi, lakukan penyortiran
  if (tinggiObjek > 0) { // Abaikan jika tidak ada objek
    kontrolBanBerjalan(false); // Hentikan ban berjalan
    sortirObjek(tinggiObjek);  // Sortir objek berdasarkan tinggi
    kontrolBanBerjalan(true);  // Lanjutkan ban berjalan
  }

  delay(500);
}
