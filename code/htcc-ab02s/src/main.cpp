#include "Arduino.h"
#include <Wire.h>
#include "GPS_Air530Z.h"
#include "HT_SSD1306Wire.h"
#include <LoRa_APP.h>

SSD1306Wire display(0x3c, 500000, SDA, SCL, GEOMETRY_128_64, GPIO10);
Air530ZClass GPS;
RadioEvents_t radioEvents;
size_t frequency = 868000000;

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

	Serial.println(buff);

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
	if (Serial.available()) {
		String msgStr = Serial.readStringUntil('|');
		const char *msg = msgStr.c_str();
		Radio.Send((uint8_t*)msg, strlen(msg));
	}
	delay(100);
}