#include <SPI.h>
#include <MFRC522.h>

// Пины подключения RFID модуля
#define RST_PIN 9
#define SS_PIN 10

// Пины выходов
#define MAIN_OUTPUT_PIN 7     // Основной выход (если UID прочитан и не 00000000)
#define ZERO_OUTPUT_PIN 8     // Альтернативный выход (если UID = 00000000 или карта не считана)

MFRC522 mfrc522(SS_PIN, RST_PIN); // Создаем объект для работы с RFID

void setup() {
  Serial.begin(9600);      // Инициализация Serial для вывода в монитор
  SPI.begin();             // Инициализация SPI
  mfrc522.PCD_Init();      // Инициализация RFID модуля

  // Настройка выходов
  pinMode(MAIN_OUTPUT_PIN, OUTPUT);
  pinMode(ZERO_OUTPUT_PIN, OUTPUT);

  digitalWrite(MAIN_OUTPUT_PIN, LOW); // Начальное состояние - выключено
  digitalWrite(ZERO_OUTPUT_PIN, LOW);

  Serial.println("Устройство чтения готово. Поднесите карту.");
}

void loop() {
  // Проверяем, есть ли новая карта
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Карта не считана или отсутствует.");
    activateZeroOutput();
    return;
  }

  // Читаем UID карты
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX); // Формируем строку UID
  }
  uid.toUpperCase();

  Serial.print("Считан UID: ");
  Serial.println(uid);

  // Проверяем UID
  if (uid != "00000000") {
    Serial.println("UID корректный. Активируем основной выход на 120 секунд.");
    activateMainOutput();
  } else {
    Serial.println("UID некорректный (00000000). Активируем альтернативный выход на 6 секунд.");
    activateZeroOutput();
  }

  // Завершаем работу с картой
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

// Функция активации основного выхода
void activateMainOutput() {
  digitalWrite(MAIN_OUTPUT_PIN, HIGH);
  delay(120000); // 120 секунд
  digitalWrite(MAIN_OUTPUT_PIN, LOW);
}

// Функция активации альтернативного выхода
void activateZeroOutput() {
  digitalWrite(ZERO_OUTPUT_PIN, HIGH);
  delay(6000); // 6 секунд
  digitalWrite(ZERO_OUTPUT_PIN, LOW);
}
