#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>

// Pines para ESP32 clasico (ESP32-WROOM / DevKit).
constexpr uint8_t RFID_SS_PIN   = 40; // SDA
constexpr uint8_t RFID_RST_PIN  = 1;
constexpr uint8_t RFID_SCK_PIN  = 41;
constexpr uint8_t RFID_MISO_PIN = 2;
constexpr uint8_t RFID_MOSI_PIN = 42;

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("\n===== LECTOR RFID RC522 =====");

    // Inicializar SPI con los pines elegidos.
    SPI.begin(
        RFID_SCK_PIN,
        RFID_MISO_PIN,
        RFID_MOSI_PIN,
        RFID_SS_PIN
    );

    rfid.PCD_Init();
    delay(50);

    // Comprobar que el lector responde.
    byte version = rfid.PCD_ReadRegister(MFRC522::VersionReg);

    Serial.print("Version del lector: 0x");
    Serial.println(version, HEX);

    if (version == 0x00 || version == 0xFF) {
        Serial.println("ERROR: el RC522 no responde.");
        Serial.println("Revisa alimentacion, conexiones y soldaduras.");

        while (true) {
            delay(1000);
        }
    }

    Serial.println("Acerca una tarjeta o llavero RFID...");
}

void loop() {
    // Esperar una tarjeta nueva.
    if (!rfid.PICC_IsNewCardPresent()) {
        delay(10);
        return;
    }

    // Leer su identificador.
    if (!rfid.PICC_ReadCardSerial()) {
        return;
    }

    Serial.println("\n--- Tarjeta detectada ---");

    Serial.print("UID: ");

    for (byte i = 0; i < rfid.uid.size; i++) {
        if (i > 0) {
            Serial.print(':');
        }

        if (rfid.uid.uidByte[i] < 0x10) {
            Serial.print('0');
        }

        Serial.print(rfid.uid.uidByte[i], HEX);
    }

    Serial.println();

    MFRC522::PICC_Type tipo = rfid.PICC_GetType(rfid.uid.sak);

    Serial.print("Tipo: ");
    Serial.println(rfid.PICC_GetTypeName(tipo));

    // Terminar la comunicacion con esta tarjeta.
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    Serial.println("Retira la tarjeta y acerca otra.");
}