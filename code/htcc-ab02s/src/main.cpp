#include "Arduino.h"
#include <Wire.h>
#include "GPS_Air530Z.h"
#include "HT_SSD1306Wire.h"
#include <LoRa_APP.h>
#include <AESLib.h>

SSD1306Wire display(0x3c, 500000, SDA, SCL, GEOMETRY_128_64, GPIO10);
Air530ZClass GPS;
RadioEvents_t radioEvents;
size_t frequency = 868000000;
AESLib aesLib;

byte aesKey[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F};

void vextOn() {//GPS and display on
	pinMode(Vext, OUTPUT);
	digitalWrite(Vext, LOW);
}

void vextOff() {//GPS and display off
	pinMode(Vext, OUTPUT);
	digitalWrite(Vext, HIGH);
}

void txDone() {
	Serial.println("Tx done");
	Radio.Rx(0);
}

void rxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
	String buff = "";
	for (int i = 0; i < size; i++) {
		char c = (char)payload[i];
		buff += c;
	}

	Serial.println("Received buff: " + buff);
	Serial.println("\nrssi: ");
	Serial.print(rssi);
	Serial.println("snr: ");
	Serial.print(snr);

	Radio.Rx(0);
}

void rxTimeout() {
	Serial.println("rx timeout");
	Radio.Rx(0);
}

void rxError() {
	Serial.println("rx error");
	Radio.Rx(0);
}

String encrypt(const char *msg) {
	byte plain[32];
	byte encrypted[32];
	int len = strlen(msg);

	memcpy(plain, msg, len);

	int encLen = aesLib.encrypt(plain, encrypted, len, aesKey);
}

void setup() {	
	vextOn();
	delay(10);

	display.init();
	display.clear();
	display.setTextAlignment(TEXT_ALIGN_CENTER);
	display.setFont(ArialMT_Plain_16);
	display.drawString(64, 32 - 8, "Testing on Serial");
	display.display();

	Serial.begin(921600);
	GPS.begin(115200);

	radioEvents.TxDone = txDone;
	radioEvents.RxDone = rxDone;
	radioEvents.RxTimeout = rxTimeout;
	radioEvents.RxError = rxError;

	Radio.Init(&radioEvents);
	Radio.SetChannel(frequency);

	Radio.SetRxConfig(
		MODEM_LORA,         // RadioModems_t modem
		0,                  // uint32_t bandwidth (0=125 kHz)
		7,                  // uint32_t datarate (SF7)
		1,                  // uint8_t coderate (4/5)
		0,                  // uint32_t bandwidthAfc (usually 0 for LoRa)
		8,                  // uint16_t preambleLen
		0,                  // uint16_t symbTimeout (0 = infinite)
		false,              // bool fixLen
		0,                  // uint8_t payloadLen (0 = variable length)
		true,               // bool crcOn
		false,              // bool freqHopOn
		0,                  // uint8_t hopPeriod
		false,              // bool iqInverted
		true                // bool rxContinuous
	);

	String okMsg = "LoRa ready!";

	Serial.println(okMsg);

	display.clear();
	display.drawString(64, 32, okMsg);
	display.display();
}

void loop() {
	const char *msg = "Hello P2P";
	Radio.Send((uint8_t *)msg, strlen(msg));
	Serial.println("Sent packet");
	delay(5000);
}