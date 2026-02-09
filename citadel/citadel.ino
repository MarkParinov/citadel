/* CITADEL server software */
#include <HardwareSerial.h>
/* wifi */
#include <WiFi.h>
#include <esp_wifi.h>
/* servo */
#include <ESP32Servo.h>

const char* SSID = "CITADEL CONTROL";
const char* PASSWORD = "_operator_";

#define X_MOTOR_PIN 16
#define Z_MOTOR_PIN 17
#define LASER_PIN 18

#define X_MOTOR_OFFSET 0
#define Z_MOTOR_OFFSET -4

uint8_t LASER_STATE = 0;
uint8_t LASER_REQUEST = 0;

String data;

Servo x_servo;
Servo z_servo;

WiFiServer citadel_server(100);

void convert_data_to_byte_array(uint8_t* _arr, size_t _size) {
	for (size_t i = 0; i < _size; i++)
		_arr[i] = (uint8_t)data[i];
}

void setup() {
	Serial.begin(115200);
	
	Serial.println("[SETUP] CITADEL initiated; version checker=3");

	Serial.println("[SETUP] setting motor outputs...");

	x_servo.attach(X_MOTOR_PIN);
	z_servo.attach(Z_MOTOR_PIN);

	x_servo.write(90+X_MOTOR_OFFSET);
	z_servo.write(90+Z_MOTOR_OFFSET);

	Serial.println("[SETUP] setting laser output...");

	pinMode(LASER_PIN, OUTPUT);
	
	Serial.println("[SETUP] setting AP...");

	WiFi.softAP(SSID, PASSWORD);

	IPAddress IP = WiFi.softAPIP();
	Serial.print("[SETUP] AP IP address: ");
	Serial.println(IP);

	Serial.println("[SETUP] starting server...");
	citadel_server.begin();

	Serial.println("[SETUP] setup sequence completed");
	for (int i = 0; i < 4; i++) {
		digitalWrite(LASER_PIN, HIGH);
		delay(100);
		digitalWrite(LASER_PIN, LOW);
		delay(100);
	}
}

void handle_data(uint8_t* _data, size_t _size) {
	if (_data[0] != 0xcd) {
		Serial.println("[RUNTIME PACKET HANDLE] invalid magic number; ignoring packet");
		return;
	}

	LASER_REQUEST = _data[1] & 1;

	if (LASER_REQUEST != LASER_STATE) {
		digitalWrite(LASER_PIN, LASER_REQUEST);
		LASER_STATE = LASER_REQUEST;
	}

	int8_t x1 = (int8_t)_data[2];
	int8_t z1 = (int8_t)_data[3];

	int8_t x2 = (int8_t)_data[4];
	int8_t z2 = (int8_t)_data[5];

	int8_t x3 = (int8_t)_data[6];
	int8_t z3 = (int8_t)_data[7];

	int8_t x4 = (int8_t)_data[8];
	int8_t z4 = (int8_t)_data[9];

	Serial.printf("[RUNTIME PACKET HANDLE] 1: x=%d; z=%d\n", x1, z1);
	Serial.printf("[RUNTIME PACKET HANDLE] 2: x=%d; z=%d\n", x2, z2);
	Serial.printf("[RUNTIME PACKET HANDLE] 3: x=%d; z=%d\n", x3, z3);
	Serial.printf("[RUNTIME PACKET HANDLE] 4: x=%d; z=%d\n", x4, z4);

	/* ANGLE 1 */
	Serial.println("[RUNTIME PACKET HANDLE] positioning at angle 1...");
	x_servo.write(180-(90+x1)+X_MOTOR_OFFSET);
	z_servo.write(180-(90+z1)+Z_MOTOR_OFFSET);
	delay(3000);
	/* ANGLE 2 */
	Serial.println("[RUNTIME PACKET HANDLE] positioning at angle 2...");
	x_servo.write(180-(90+x2)+X_MOTOR_OFFSET);
	z_servo.write(180-(90+z2)+Z_MOTOR_OFFSET);
	delay(3000);
	/* ANGLE 3 */
	Serial.println("[RUNTIME PACKET HANDLE] positioning at angle 3...");
	x_servo.write(180-(90+x3)+X_MOTOR_OFFSET);
	z_servo.write(180-(90+z3)+Z_MOTOR_OFFSET);
	delay(3000);
	/* ANGLE 4 */
	Serial.println("[RUNTIME PACKET HANDLE] positioning at angle 4...");
	x_servo.write(180-(90+x4)+X_MOTOR_OFFSET);
	z_servo.write(180-(90+z4)+Z_MOTOR_OFFSET);
	delay(3000);

	/* FINISH */
	Serial.println("[RUNTIME PACKET HANDLE] sequence complete; defaulting to (90;90)");
	x_servo.write(90+X_MOTOR_OFFSET);
	z_servo.write(90+Z_MOTOR_OFFSET);
}

void loop() {
    WiFiClient client = citadel_server.available();
    client.setTimeout(100);
    uint8_t payload[128]; payload[0] = 0;
	
	if (client) {
		Serial.printf("[RUNTIME CONNECTION] new client connection; IP = ");
    	Serial.println(client.remoteIP());

		while (client.connected()) {
			if (client.available()) {
				Serial.println("[RUNTIME CONNECTION] reading data...");
	    		data = client.readStringUntil('\r');
	    		Serial.println("[RUNTIME CONNECTION] converting data...");
				convert_data_to_byte_array(payload, data.length());
    			Serial.println("[RUNTIME CONNECTION] received data packet:");
    			for (size_t i = 0; i < data.length(); i++)
    				Serial.printf("0x%x [%d] ", payload[i], payload[i]);
    			Serial.printf("\n[RUNTIME CONNECTION] total of %zu bytes\n", data.length());
    			data = "";
	    	}
		}
		
		client.stop();
	    Serial.println("[RUNTIME CONNECTION] client session closed");
    }
    
	if (payload[0])
    	handle_data(payload, data.length());
}
