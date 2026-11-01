compile-and-upload:
	arduino-cli compile --fqbn arduino:avr:uno -u -p /dev/ttyACM0 .

compile:
	arduino-cli compile --fqbn arduino:avr:uno .

upload:
	arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:uno .

monitor:
	arduino-cli monitor -p /dev/ttyACM0 -c baudrate=9600
