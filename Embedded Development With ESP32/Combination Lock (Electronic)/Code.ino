#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Wire.h>
#include <EEPROM.h> 

// --- DONANIM AYARLARI ---
const int servoPin    = 13;
const int redLedPin   = 7;
const int greenLedPin = 6;
const int buzzerPin   = 8;

// --- GÜVENLİK AYARLARI ---
const char DEFAULT_PASS[] = "1234"; // Varsayılan ve reset sonrası şifre

// --- NESNELER ---
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo lockServo;

// --- KEYPAD AYARLARI ---
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'A', '3', '2', '1'},
  {'B', '6', '5', '4'},
  {'C', '9', '8', '7'},
  {'D', '#', '0', '*'}
};

byte rowPins[ROWS] = {9, 10, 11, 12};
byte colPins[COLS] = {2, 3, 4, 5};

Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- DEĞİŞKENLER ---
char password[5]; // EEPROM'dan okunan güncel şifre
char enteredPassword[5];
byte charIndex = 0;

void setup() {
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  lockServo.attach(servoPin);
  lcd.init();
  lcd.backlight();

  // İlk çalıştırmada hafıza boşsa varsayılan şifreyi yükle
  if (EEPROM.read(0) == 255) {
    savePasswordToEEPROM((char*)DEFAULT_PASS);
  }

  // Hafızadan şifreyi oku
  readPasswordFromEEPROM();
  
  resetSystem();
}

void loop() {
  char customKey = customKeypad.getKey();

  if (customKey) {
    playTone(50); // Tuş sesi

    // --- A: Şifre Değiştir ---
    if (customKey == 'A') {
      changePasswordRoutine();
      return;
    }

    // --- B: BACKSPACE (Tek Karakter Silme) ---
    if (customKey == 'B') {
      if (charIndex > 0) {
        charIndex--;
        enteredPassword[charIndex] = 0; // Diziden sil
        lcd.setCursor(charIndex, 1);
        lcd.print(" "); // Ekrandan sil
        lcd.setCursor(charIndex, 1); // İmleci geri al
      }
      return;
    }

    // --- C: TEMİZLE (Başa Dön/Reset) ---
    if (customKey == 'C') {
      lcd.clear();
      lcd.print("TEMIZLENDI"); 
      playTone(100);
      delay(500);
      resetSystem(); // Tüm girdileri sil ve ana ekrana dön
      return;
    }

    // --- D: FABRİKA AYARLARI (DİREKT RESET) ---
    if (customKey == 'D') {
      factoryResetRoutine();
      return;
    }

    // --- *: İptal ---
    if (customKey == '*') {
      resetSystem();
      return;
    }
    
    // --- Rakam Girişi ---
    if (charIndex < 4 && customKey != '#' && customKey != 'A' && customKey != 'B' && customKey != 'C' && customKey != 'D') {
      enteredPassword[charIndex] = customKey;
      lcd.setCursor(charIndex, 1);
      lcd.print("*");
      charIndex++;
    }

    if (charIndex == 4) {
      delay(200);
      checkPassword();
    }
  }
}

// --- ANA FONKSİYONLAR ---

void checkPassword() {
  if (strcmp(enteredPassword, password) == 0) {
    accessGranted();
  } else {
    accessDenied();
  }
}

void accessGranted() {
  lcd.clear();
  lcd.print("HOSGELDINIZ");
  digitalWrite(greenLedPin, HIGH);
  lockServo.write(90); 
  
  // Onay sesi
  digitalWrite(buzzerPin, HIGH); delay(100); digitalWrite(buzzerPin, LOW);
  delay(100);
  digitalWrite(buzzerPin, HIGH); delay(100); digitalWrite(buzzerPin, LOW);
  
  delay(4000);
  resetSystem();
}

void accessDenied() {
  lcd.clear();
  lcd.print("HATALI SIFRE!");
  digitalWrite(redLedPin, HIGH);
  
  // Hata sesi (Uzun)
  digitalWrite(buzzerPin, HIGH); delay(1000); digitalWrite(buzzerPin, LOW);
  
  resetSystem();
}

void resetSystem() {
  memset(enteredPassword, 0, sizeof(enteredPassword));
  charIndex = 0;
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  lockServo.write(0); 
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sifre Giriniz:");
}

void playTone(int duration) {
  digitalWrite(buzzerPin, HIGH);
  delay(duration);
  digitalWrite(buzzerPin, LOW);
}

// --- ŞİFRE İŞLEMLERİ ---

// A Tuşu: Mevcut şifre girilerek yeni şifre atanır
void changePasswordRoutine() {
  lcd.clear();
  lcd.print("Eski Sifre?");
  
  if (getPinInputAndCheck(password)) { // Mevcut şifre kontrolü
    lcd.clear();
    lcd.print("Yeni Sifre Gir:");
    
    char newPass[5];
    getPinInput(newPass); // Yeni şifreyi al
    
    savePasswordToEEPROM(newPass);
    strcpy(password, newPass);
    
    lcd.clear();
    lcd.print("Sifre Degisti!");
    delay(2000);
  } else {
    lcd.clear();
    lcd.print("Yanlis Sifre!");
    delay(2000);
  }
  resetSystem();
}

// D Tuşu: Şifre sormadan direkt fabrika ayarlarına döner (1234)
void factoryResetRoutine() {
  lcd.clear();
  lcd.print("SIFIRLANIYOR...");
  delay(1000); // Kullanıcı görsün diye bekleme
  
  savePasswordToEEPROM((char*)DEFAULT_PASS); // Hafızaya 1234 yaz
  strcpy(password, DEFAULT_PASS); // Aktif şifreyi güncelle
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FABRIKA AYARINA"); 
  lcd.setCursor(0,1);
  lcd.print("DONULDU");
  
  // Onay sesi
  for(int i=0; i<3; i++) {
    digitalWrite(buzzerPin, HIGH); delay(100); digitalWrite(buzzerPin, LOW); delay(100);
  }
  delay(2000);
  resetSystem();
}

// --- YARDIMCI GİRİŞ FONKSİYONLARI ---

// Kullanıcıdan 4 hane şifre alır, 'target' ile eşleşiyor mu bakar
bool getPinInputAndCheck(char* target) {
  char tempPass[5];
  getPinInput(tempPass);
  return (strcmp(tempPass, target) == 0);
}

// Sadece giriş alır ve diziye kaydeder
void getPinInput(char* outBuffer) {
  byte idx = 0;
  memset(outBuffer, 0, 5);
  
  while (idx < 4) {
    char key = customKeypad.getKey();
    if (key) {
      playTone(50);
      if (key == '*') { // İptal
        resetSystem(); 
        outBuffer[0] = '\0'; 
        return; 
      }
      
      if (key >= '0' && key <= '9') { // Sadece rakamlar
         outBuffer[idx] = key;
         lcd.setCursor(idx, 1);
         lcd.print("*"); // Gizli gösterim
         idx++;
      }
    }
  }
}

// EEPROM Fonksiyonları
void savePasswordToEEPROM(char* newPass) {
  for (int i = 0; i < 4; i++) {
    EEPROM.write(i, newPass[i]);
  }
}

void readPasswordFromEEPROM() {
  for (int i = 0; i < 4; i++) {
    password[i] = EEPROM.read(i);
  }
  password[4] = '\0';
}