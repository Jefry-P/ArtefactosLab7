#include <SPI.h>
#include <Wire.h>
#include <LoRa.h>

// ======================================================
// IDENTIFICACION
// ======================================================

const int MI_ID = 3;
const int DESTINO_ID = 4;

String mensajeOriginal = "Hola este es un mensaje";

// ======================================================
// PINES
// ======================================================

#define LORA_SCK   18
#define LORA_MISO  19
#define LORA_MOSI  23
#define LORA_NSS   5

#define TOUCH_PIN  T0
const int UMBRAL_TOUCH = 500;
bool touchPresionado = false;

// ======================================================
// ATBASH
// ======================================================

char convertirAtbash(char letra) {

  if (letra >= 'A' && letra <= 'Z') {
    return 'Z' - (letra - 'A');
  }

  if (letra >= 'a' && letra <= 'z') {
    return 'z' - (letra - 'a');
  }

  return letra;
}

String cifrarAtbash(String mensaje) {

  String mensajeCifrado = "";

  for (int i = 0; i < mensaje.length(); i++) {
    mensajeCifrado += convertirAtbash(mensaje.charAt(i));
  }

  return mensajeCifrado;
}

// ======================================================
// ENVIAR MENSAJE
// ======================================================

void enviarMensaje() {

  String mensajeCifrado = cifrarAtbash(mensajeOriginal);

  Serial.println();
  Serial.println("TOUCH DETECTADO");

  Serial.print("Mensaje original: ");
  Serial.println(mensajeOriginal);

  Serial.print("Mensaje cifrado: ");
  Serial.println(mensajeCifrado);

  // Formato del paquete: ORIGEN|DESTINO|MENSAJE
  if (!LoRa.beginPacket()) {
    Serial.println("ERROR: No se pudo preparar el paquete");
    return;
  }

  LoRa.print(MI_ID);
  LoRa.print("|");
  LoRa.print(DESTINO_ID);
  LoRa.print("|");
  LoRa.print(mensajeCifrado);

  if (!LoRa.endPacket()) {
    Serial.println("ERROR: No se pudo enviar el paquete");
    return;
  }

  Serial.print("Paquete enviado: ");
  Serial.print(MI_ID);
  Serial.print("|");
  Serial.print(DESTINO_ID);
  Serial.print("|");
  Serial.println(mensajeCifrado);

  Serial.println("----------------------------");
}

// ======================================================
// SETUP
// ======================================================

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("TRANSMISOR LORA");

  Serial.print("Mi ID: ");
  Serial.println(MI_ID);

  Serial.print("Destino: ");
  Serial.println(DESTINO_ID);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_NSS);

  // RESET y DIO0 omitidos
  LoRa.setPins(LORA_NSS, -1, -1);

  if (!LoRa.begin(433E6)) {
    Serial.println("ERROR: No se pudo iniciar LoRa");
    while (true);
  }

  Serial.println("LoRa iniciado correctamente");
  Serial.println("Esperando toque...");
}

// ======================================================
// LOOP
// ======================================================

void loop() {

  int valorTouch = touchRead(TOUCH_PIN);

  // Descomenta para medir y ajustar UMBRAL_TOUCH:
  Serial.println(valorTouch);

  bool toqueActual = valorTouch < UMBRAL_TOUCH;

  if (toqueActual && !touchPresionado) {
    touchPresionado = true;
    enviarMensaje();
  }

  if (!toqueActual) {
    touchPresionado = false;
  }

  delay(100);
}