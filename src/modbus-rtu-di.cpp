#include <Arduino.h>
#include <ModbusRTU.h> // De ModbusRTU bibliotheek

// Definieer de pinnen voor de digitale outputs
const int LED_PIN_1 = 2; // Bijvoorbeeld GPIO 2, pas dit aan naar jouw bord
const int LED_PIN_2 = 4; // Bijvoorbeeld GPIO 4, pas dit aan naar jouw bord

// Definieer de Modbus slave ID
const uint8_t MODBUS_SLAVE_ID = 1;

// Definieer de Modbus holding registers voor de digitale outputs
// We gebruiken 2 holding registers, elk vertegenwoordigt de status van een output.
// Register 0 (0x0000) voor LED_PIN_1
// Register 1 (0x0001) voor LED_PIN_2
// De waarden in deze registers bepalen of de output AAN (1) of UIT (0) is.
uint16_t holdingRegisters[2]; // Array om de waarden van de holding registers op te slaan

// Initialiseer de ModbusRTU object
// ModbusRTU modbus; // Voor ModbusRTU object met standaard seriele poort Serial
ModbusRTU modbus(&Serial2); // Gebruik Serial2 voor Modbus communicatie (aanbevolen voor RS485)

void setup() {
  // Start seriële communicatie voor debugging
  Serial.begin(115200);
  Serial.println("ESP32 Modbus RTU Slave met 2 Digitale Outputs");

  // Configureer de digitale output pinnen
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);

  // Zorg ervoor dat de outputs initieel UIT zijn
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  holdingRegisters[0] = 0; // Initialiseer het register naar 0 (UIT)
  holdingRegisters[1] = 0; // Initialiseer het register naar 0 (UIT)

  // Configureer de Modbus seriële poort (Serial2 voor ESP32)
  // De standaard RS485 pinnen op ESP32 zijn vaak:
  // RX2: GPIO 16
  // TX2: GPIO 17
  // Controleer de documentatie van jouw ESP32 bord voor de juiste pinnen.
  Serial2.begin(9600, SERIAL_8N1, 16, 17); // Baudrate, Data/Parity/Stop bits, RX_PIN, TX_PIN

  // Begin de Modbus slave
  modbus.begin(&Serial2, MODBUS_SLAVE_ID); // Koppel Modbus aan Serial2 en stel de slave ID in

  // Voeg de Modbus registers toe die we willen gebruiken.
  // We definiëren hier een bereik van 2 holding registers, beginnend bij adres 0.
  // Deze registers zullen de status van onze digitale outputs weerspiegelen en controleren.
  modbus.addHreg(0, holdingRegisters, 2); // Startadres 0, array holdingRegisters, aantal 2
}

void loop() {
  // Verwerk inkomende Modbus verzoeken
  modbus.task();

  // Controleer de waarden in de holding registers en update de digitale outputs
  // Als holdingRegisters[0] 1 is, zet LED_PIN_1 AAN, anders UIT
  if (holdingRegisters[0] == 1) {
    digitalWrite(LED_PIN_1, HIGH);
  } else {
    digitalWrite(LED_PIN_1, LOW);
  }

  // Als holdingRegisters[1] 1 is, zet LED_PIN_2 AAN, anders UIT
  if (holdingRegisters[1] == 1) {
    digitalWrite(LED_PIN_2, HIGH);
  } else {
    digitalWrite(LED_PIN_2, LOW);
  }

  // Optioneel: Voeg een kleine vertraging toe om CPU-gebruik te verminderen
  // delay(10);
}

