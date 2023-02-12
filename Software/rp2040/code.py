import board
import busio
import digitalio

import time

import usb_hid
from adafruit_hid.keyboard import Keyboard

# comment out these lines for non_US keyboards
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS as KeyboardLayout
from adafruit_hid.keycode import Keycode


kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayout(kbd)

scriptKeywords = {
    'WINDOWS': Keycode.WINDOWS, 'SHIFT': Keycode.SHIFT, 'ALT': Keycode.ALT, 'CTRL': Keycode.CONTROL,
    'DOWN': Keycode.DOWN_ARROW, 'LEFT': Keycode.LEFT_ARROW, 'RIGHT': Keycode.RIGHT_ARROW,
    'UP': Keycode.UP_ARROW, 'CAPSLOCK': Keycode.CAPS_LOCK, 'DELETE': Keycode.DELETE,
    'ESC': Keycode.ESCAPE, 'HOME': Keycode.HOME,
    'PRINTSCREEN': Keycode.PRINT_SCREEN, 'ENTER': Keycode.ENTER,
    'SPACE': Keycode.SPACE, 'TAB': Keycode.TAB, 'BACKSPACE': Keycode.BACKSPACE,
    'A': Keycode.A, 'B': Keycode.B, 'C': Keycode.C, 'D': Keycode.D, 'E': Keycode.E,
    'F': Keycode.F, 'G': Keycode.G, 'H': Keycode.H, 'I': Keycode.I, 'J': Keycode.J,
    'K': Keycode.K, 'L': Keycode.L, 'M': Keycode.M, 'N': Keycode.N, 'O': Keycode.O,
    'P': Keycode.P, 'Q': Keycode.Q, 'R': Keycode.R, 'S': Keycode.S, 'T': Keycode.T,
    'U': Keycode.U, 'V': Keycode.V, 'W': Keycode.W, 'X': Keycode.X, 'Y': Keycode.Y,
    'Z': Keycode.Z, '0': Keycode.ZERO, '1': Keycode.ONE, '2': Keycode.TWO,
    '3': Keycode.THREE, '4': Keycode.FOUR, '5': Keycode.FIVE, '6': Keycode.SIX,
    '7': Keycode.SEVEN, '8': Keycode.EIGHT, '9': Keycode.NINE}


def writeString(commandString):
    global layout
    layout.write(commandString)

def parseCommands(commandLines):
    global kbd
    
    for line in commandLines:
        commandList = line.split('+')
        for command in commandList:
            command = command.strip()
            
            keyword = scriptKeywords.get(command, None)
            if keyword:
                kbd.press(keyword)
            elif(command[0:5] == "DELAY"):
                time.sleep(float(line[6:])/1000)
            elif(command[0:6] == "STRING"):
                writeString(line[7:])
                
        kbd.release_all()




uart_esp = busio.UART(board.GP12, board.GP13, baudrate=115200)
espEnable = digitalio.DigitalInOut(board.GP2)
led = digitalio.DigitalInOut(board.LED)
led.direction = digitalio.Direction.OUTPUT
led.value = True
espEnable.direction = digitalio.Direction.OUTPUT

espEnable.value = True

while True:
    try:
    
        rxData = bytearray(255)
        barray = bytearray()
        count = uart_esp.readinto(rxData)
        

            
        if count:
            commandList = []
            dataString = rxData.decode('utf-8').splitlines()
            for line in dataString:
                if b'\x00' in line.encode():
                    print("END")
                else:
                    commandList.append(line)
            parseCommands(commandList)
    except Exception as e:
        print(e)
        continue
 