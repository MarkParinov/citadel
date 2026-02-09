compile:
	arduino-cli compile --fqbn esp32:esp32:esp32 citadel --output-dir ../build

flash:
	arduino-cli upload -p $(ARGS) --fqbn esp32:esp32:esp32 --input-file citadel/build/esp32.esp32.esp32/citadel.ino.bin

clean:
	rm ./build/ -rf
