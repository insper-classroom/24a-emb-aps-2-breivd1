import pyautogui
import serial
import time

# Open serial port
ser = serial.Serial('/dev/ttyACM0', 115200)

# Last pressed key state for movement axes
last_pressed_keys = {0: None, 1: None}

def parse_data(data):
    axis = data[0]  # Key index
    value = int.from_bytes(data[1:3], byteorder='little', signed=True)  # Press or release value
    return axis, value

def press_key(axis, value):
    global last_pressed_keys
    key_map = {
        0: ('d', 'a'),  # X-axis for 'd' (right) and 'a' (left)
        1: ('w', 's'),  # Y-axis for 'w' (up) and 's' (down)
        2: {11: 'e', 10: 'e', 21: 'q', 20: 'q', 31: '1', 41: '2', 30: '1', 40: '2'},  # Special keys with press/release actions
    }

    if axis in (0, 1):  # Handle movement keys
        if value != 0:
            current_key = key_map[axis][0] if value > 0 else key_map[axis][1]
            if current_key != last_pressed_keys[axis]:
                if last_pressed_keys[axis] is not None:
                    pyautogui.keyUp(last_pressed_keys[axis])  # Release the previous key
                pyautogui.keyDown(current_key)  # Press the new key
                last_pressed_keys[axis] = current_key
        else:
            if last_pressed_keys[axis] is not None:
                pyautogui.keyUp(last_pressed_keys[axis])  # Release the key if the joystick returns to neutral
                last_pressed_keys[axis] = None

    elif axis == 2:  # Handle special keys
        key_actions = key_map[axis]
        if value in key_actions:
            key = key_actions[value]
            if value % 10 == 1:  # Press command
                pyautogui.keyDown(key)  # Key down
            elif value % 10 == 0:  # Release command
                pyautogui.keyUp(key)  # Key up

try:
    print('Waiting for sync package...')
    while True:
        data = ser.read(1)
        if data == b'\xff':
            data = ser.read(3)
            axis, value = parse_data(data)
            press_key(axis, value)

except KeyboardInterrupt:
    print("Program terminated by user")
except Exception as e:
    print(f"An error occurred: {e}")
finally:
    ser.close()
