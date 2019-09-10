#include <Wire.h> //Biblioteca para I2C
#include <SparkFunMLX90614.h> //Biblioteca SparkFunMLX90614 (sensor IR)
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
 
//objeto responsável pela comunicação com o sensor de temperatura infravermelho
IRTherm sensor;
 
BLECharacteristic *characteristicTX; //através desse objeto iremos enviar dados para o client
 
bool deviceConnected = false; //controle de dispositivo conectado
 
const int Motor1 = 2; // Nome da váriavel e pino
const int Motor2 = 23; //Nome da variavel e pino
 
#define SERVICE_UUID   "ab0828b1-198e-4351-b779-901fa0e0371e”
#define CHARACTERISTIC_UUID_RX  "4ac8a682-9736-4e5d-932b-e9b31405049c"
#define CHARACTERISTIC_UUID_TX  "0972EF8C-7613-4075-AD52-756F33D4DA91”

void setup() {
    Serial.begin(115200);
 
    pinMode(Motor1, OUTPUT);
    pinMode(Motor2, OUTPUT);
 
    // Create the BLE Device
    BLEDevice::init("ESP32-BLE"); // nome do dispositivo bluetooth
 
    // Create the BLE Server
    BLEServer *server = BLEDevice::createServer(); //cria um BLE server 
 
    server->setCallbacks(new ServerCallbacks()); //seta o callback do server
 
    // Create the BLE Service
    BLEService *service = server->createService(SERVICE_UUID);
 
    // Create a BLE Characteristic para envio de dados
    characteristicTX = service->createCharacteristic(
                       CHARACTERISTIC_UUID_TX,
                       BLECharacteristic::PROPERTY_NOTIFY
                     );
 
    characteristicTX->addDescriptor(new BLE2902());

     // Create a BLE Characteristic para recebimento de dados
    BLECharacteristic *characteristic = service->createCharacteristic(
                                                      CHARACTERISTIC_UUID_RX,
                                                      BLECharacteristic::PROPERTY_WRITE
                                                    );
 
    characteristic->setCallbacks(new CharacteristicCallbacks());
 
    // Start the service
    service->start();
 
    // Start advertising (descoberta do ESP32)
    server->getAdvertising()->start();
 
    //Inicializa sensor de temperatura infravermelho 
    sensor.begin(); 
 
    //Seleciona temperatura em Celsius 
    sensor.setUnit(TEMP_C);//podemos ainda utilizar TEMP_F para Fahrenheit ou TEMP_K para Kelvin 
    Serial.println(“Aguardando algum dispositivo conectar...");
 
} //fim setup

//callback para eventos das características
class CharacteristicCallbacks: public BLECharacteristicCallbacks {
     void onWrite(BLECharacteristic *characteristic) {
          //retorna ponteiro para o registrador contendo o valor atual da caracteristica
          std::string rxValue = characteristic->getValue(); 
          //verifica se existe dados (tamanho maior que zero)
          if (rxValue.length() > 0) {
 
               for (int i = 0; i < rxValue.length(); i++) {
             Serial.print(rxValue[i]);
               }
               Serial.println();
               //dado dentro do find move o motor 1 para direita
               if (rxValue.find("") != -1) { 
             digitalWrite(LED, HIGH);
               }//dado dentro do find move o motor 1 para esquerda
               else if (rxValue.find("") != -1) {
             digitalWrite(LED, LOW);
               }
               //dado dentro do fin move o motor 2 para direita
               else if (rxValue.find("") != -1) { 
             digitalWrite(BUZZER, HIGH);
               }//dado dentro do find move o motor 2 para esquerda
               else if (rxValue.find("") != -1) {
             digitalWrite(BUZZER, LOW);
               }
          }
     }//onWrite
};
//callback para receber os eventos de conexão de dispositivos
class ServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
 deviceConnected = true;
    };
 
    void onDisconnect(BLEServer* pServer) {
 deviceConnected = false;
    }
};
