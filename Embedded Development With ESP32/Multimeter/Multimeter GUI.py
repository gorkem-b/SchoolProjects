import sys
import serial
import serial.tools.list_ports
from PyQt5.QtWidgets import (QApplication, QWidget, QVBoxLayout, QHBoxLayout, 
                             QPushButton, QLabel, QFrame, QMainWindow)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QObject
from PyQt5.QtGui import QFont

ESP32_PORT = 'AUTO' 
ESP32_BAUD_RATE = 115200


def find_esp32_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if "CP210" in port.description or "CH340" in port.description or "USB-SERIAL" in port.description:
            print(f"ESP32 found: {port.device}")
            return port.device
    
    if ports:
        print(f"ESP32 not found, trying first available port ({ports[0].device}).")
        return ports[0].device # Fallback to first port
    print("No serial ports found.")
    return None

class SerialWorker(QObject):
    """
    Listens to the serial port on a separate thread.
    This prevents the GUI from freezing.
    """
    # Signal to send received data to the main GUI
    data_received = pyqtSignal(str)
    # Signal for connection status (errors, success)
    connection_status = pyqtSignal(str)

    def __init__(self, port, baud_rate):
        super().__init__()
        self.port = port
        self.baud_rate = baud_rate
        self.ser = None
        self.running = True

    def run(self):
        """This function runs when the thread starts."""
        try:
            self.ser = serial.Serial(self.port, self.baud_rate, timeout=1)
            self.connection_status.emit(f"Connected: {self.port}")
        except serial.SerialException as e:
            print(f"Error: Could not open port {self.port}. {e}")
            self.connection_status.emit(f"Error: Port unavailable - {self.port}")
            return # Stop thread if port fails

        while self.running:
            try:
                # Read one line from the ESP32 (e.g., "V:3.25")
                line = self.ser.readline().decode('utf-8').strip()
                if line:
                    # If data exists, emit it to the main GUI
                    self.data_received.emit(line)
            except (serial.SerialException, UnicodeDecodeError):
                # Handle disconnects or errors
                if self.running:
                    self.connection_status.emit("Error: Connection lost")
                    self.running = False
            
        if self.ser and self.ser.is_open:
            self.ser.close()
        print("Serial port thread stopped.")

    def send_command(self, command):
        """Sends a command to the ESP32 (e.g., "SET_MODE:V")"""
        if self.ser and self.ser.is_open:
            try:
                self.ser.write(f"{command}\n".encode('utf-8'))
                print(f"Sent: {command}")
            except serial.SerialException as e:
                self.connection_status.emit(f"Error: Write failed - {e}")

    def stop(self):
        """Signals the thread to stop."""
        self.running = False


class MultimeterApp(QMainWindow):
    """Main Multimeter GUI Application Class"""
    
    def __init__(self, port):
        super().__init__()
        self.port = port
        self.current_unit = "V" # Default unit
        self.initUI()
        self.initSerial()

    def initUI(self):
        self.setWindowTitle("ESP32 Multimeter Simulation")
        self.setGeometry(100, 100, 500, 350)

        # Main layout
        main_layout = QVBoxLayout()

        # 1. Display Screen Area
        screen_frame = QFrame()
        screen_frame.setFrameShape(QFrame.StyledPanel)
        screen_frame.setStyleSheet("background-color: #2D2D2D; border-radius: 10px;")
        screen_layout = QHBoxLayout(screen_frame)
        
        # Digital Display (for numbers)
        self.screen_label = QLabel("0.00")
        self.screen_label.setFont(QFont("DS-Digital", 80, QFont.Bold)) # Digital clock font
        self.screen_label.setStyleSheet("color: #FF4136;") # Reddish color
        self.screen_label.setAlignment(Qt.AlignRight)
        
        # Unit (V, A, R)
        self.unit_label = QLabel("V")
        self.unit_label.setFont(QFont("Arial", 40, QFont.Bold))
        self.unit_label.setStyleSheet("color: #FF4136; margin-left: 10px;")
        self.unit_label.setAlignment(Qt.AlignLeft)

        screen_layout.addWidget(self.screen_label, 4) # Give more space to numbers
        screen_layout.addWidget(self.unit_label, 1) # Less space for unit
        
        main_layout.addWidget(screen_frame, 2) # Display takes 2/3 of height

        # 2. Button Area
        button_layout = QHBoxLayout()
        
        self.btn_v = QPushButton("VOLTAGE (V)")
        self.btn_a = QPushButton("CURRENT (A)")
        self.btn_r = QPushButton("RESISTANCE (\u03A9)") # Omega symbol
        
        buttons = [self.btn_v, self.btn_a, self.btn_r]
        for btn in buttons:
            btn.setFont(QFont("Arial", 14, QFont.Bold))
            btn.setMinimumHeight(60)
            button_layout.addWidget(btn)

        main_layout.addLayout(button_layout, 1) # Buttons take 1/3 of height

        # 3. Status Bar
        self.status_label = QLabel("Initializing...")
        self.status_label.setStyleSheet("padding: 5px;")
        main_layout.addWidget(self.status_label)

        # Connect button clicks to the set_mode function
        self.btn_v.clicked.connect(lambda: self.set_mode('V'))
        self.btn_a.clicked.connect(lambda: self.set_mode('A'))
        self.btn_r.clicked.connect(lambda: self.set_mode('R'))

        # Set the central widget
        central_widget = QWidget()
        central_widget.setLayout(main_layout)
        self.setCentralWidget(central_widget)

    def initSerial(self):
        """Starts the serial worker thread."""
        if not self.port:
            self.status_label.setText("Error: ESP32 not found. Please check connection.")
            self.status_label.setStyleSheet("background-color: red; color: white; padding: 5px;")
            return

        self.thread = QThread()
        self.worker = SerialWorker(self.port, ESP32_BAUD_RATE)
        self.worker.moveToThread(self.thread)

        # Connect signals from worker to slots in the main GUI
        self.thread.started.connect(self.worker.run)
        self.worker.data_received.connect(self.update_display)
        self.worker.connection_status.connect(self.update_status)

        # Start the thread
        self.thread.start()

    def set_mode(self, mode):
        """Sends the mode command to the ESP32 when a button is pressed."""
        self.current_unit = mode
        if mode == 'R':
            # Set unit to Omega
            self.unit_label.setText("\u03A9")
        else:
            self.unit_label.setText(mode)
        
        # Send command via the worker thread
        self.worker.send_command(f"SET_MODE:{mode}")

    def update_display(self, data):
        """
        Updates the display when new data is received from the serial worker.
        (Updated to robustly filter all ESP32 boot messages).
        """
        
        # 1. CHECK: Must contain a colon
        if ':' not in data:
            # print(f"Non-format data (ignoring): {data}")
            return # Ignore

        # 2. CHECK: Must split into exactly 2 parts
        parts = data.split(':')
        if len(parts) != 2:
            # print(f"Non-format data (ignoring): {data}")
            return # Ignore
        
        prefix = parts[0]
        value_str = parts[1]

        # 3. FINAL CHECK: Prefix must be one we expect ('V', 'A', 'R')
        if prefix not in ['V', 'A', 'R']:
            # This filters out "0, SPIWP" and other boot messages
            # print(f"Unknown prefix (ignoring): {data}")
            return # Ignore

        # 4. Try to convert the value to a number
        try:
            value = float(value_str)

            # Only update the display if the prefix matches the current mode
            # (This filters out delayed data from a previous mode)
            if prefix == self.current_unit:
                if prefix == 'V':
                    formatted_value = f"{value:.2f}" # 2 decimal places for Voltage
                    self.unit_label.setText("V")
                elif prefix == 'A':
                    # Convert Amps (e.g., 0.015) to milliAmps (15.00)
                    formatted_value = f"{value * 1000:.2f}" 
                    self.unit_label.setText("mA")
                elif prefix == 'R':
                    if value > 1000000: # MegaOhms
                        formatted_value = f"{value/1000000:.2f}"
                        self.unit_label.setText("M\u03A9")
                    elif value > 1000: # KiloOhms
                        formatted_value = f"{value/1000:.2f}"
                        self.unit_label.setText("k\u03A9")
                    else: # Ohms
                        formatted_value = f"{value:.1f}"
                        self.unit_label.setText("\u03A9")
                
                self.screen_label.setText(formatted_value)
                
        except ValueError:
            # Value was not a number (e.g., "V:abc" or "V:0xee")
            print(f"Received data is not numerical (ignoring): {data}")
        except Exception as e:
            print(f"Unknown error: {e}")

    def update_status(self, message):
        """Updates the bottom status bar."""
        self.status_label.setText(message)
        if "Error" in message:
            self.status_label.setStyleSheet("background-color: red; color: white; padding: 5px;")
        else:
            self.status_label.setStyleSheet("background-color: green; color: white; padding: 5px;")

    def closeEvent(self, event):
        """Safely stops the worker thread when the window is closed."""
        if hasattr(self, 'worker'): # Check if worker exists
            self.worker.stop()
            self.thread.quit()
            self.thread.wait() # Wait for thread to finish
        event.accept()

if __name__ == '__main__':
    app = QApplication(sys.argv)    
    resolved_port = ESP32_PORT
    if ESP32_PORT == 'AUTO':
        resolved_port = find_esp32_port()

    window = MultimeterApp(port=resolved_port)
    window.show()
    sys.exit(app.exec_())