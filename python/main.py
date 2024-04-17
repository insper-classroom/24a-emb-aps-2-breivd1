import serial
import uinput
import time

# Open serial port
ser = serial.Serial('/dev/ttyACM0', 115200)

# Setup uinput device
device = uinput.Device([
    uinput.KEY_D,
    uinput.KEY_A,
    uinput.KEY_W,
    uinput.KEY_S,
    uinput.KEY_E,
    uinput.KEY_Q,
])

def parse_data(data):
    axis = data[0]  # Key index
    value = int.from_bytes(data[1:3], byteorder='little', signed=True)  # Press or release value
    return axis, value

def press_key(axis, value):
    if value == 0 and axis != 2:
        return  # Don't do anything if value is 0 and it's not the third axis

    key_map = {
        0: (uinput.KEY_D, uinput.KEY_A),  # X-axis for 'd' and 'a'
        1: (uinput.KEY_W, uinput.KEY_S),  # Y-axis for 'w' and 's'
        2: {11: uinput.KEY_E, 10: uinput.KEY_E, 21: uinput.KEY_Q, 20: uinput.KEY_Q}  # Special keys with press/release actions
    }

    if axis in (0, 1):  # Handle movement keys with quick tap
        key = key_map[axis][0] if value > 0 else key_map[axis][1]
        device.emit_click(key)

    elif axis == 2:  # Handle hold and release keys
        key_actions = key_map[axis]
        if value in key_actions:
            key = key_actions[value]
            if value % 10 == 1:  # Press command
                device.emit(key, 1)  # Key down
            elif value % 10 == 0:  # Release command
                device.emit(key, 0)  # Key up

try:
    # Pacote de sync
    while True:
        print('Waiting for sync package...')
        while True:
            data = ser.read(1)
            if data == b'\xff':
                break

        # Lendo 4 bytes da uart
        data = ser.read(3)
        axis, value = parse_data(data)
        press_key(axis, value)


except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()
