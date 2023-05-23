"""
The NucleoF446RE class provides a simple interface to send commands to the eponym dev board.
Serial com parameters should match those that were flashed on the microcontroller.
"""

import sys
import serial

class NucleoF446RE:
    def __init__(self, port: str, baud: int = 9600, timeout: float = 0.1, rtscts: bool = False, APB1_freqMHz: float = 16000000) -> None:
        """
        APB1_freqMHz parameter is not sent to target, it needs to be set according to configuration flashed on
        target for timer pwm frequency to be accurate.
        """
        self.ser = serial.Serial(
            port,
            baud,
            serial.EIGHTBITS,
            serial.PARITY_NONE,
            serial.STOPBITS_ONE,
            timeout=timeout,
            rtscts=rtscts)
        self.APB1_freqMHz = APB1_freqMHz

    def _cmd_fmt(self, cmd : str) -> bytes:
        """
        Parameter cmd is the user input.
        It needs to be cut or padded with \0 before being sent
        (DMA waits for 64 bytes to be received through UART).
        Lastly, it is encoded (utf-8) so as to be ready to be
        sent to the Serial.write method.
        """

        if len(cmd) >= 64:
            print("ALERT")
            cmd = cmd[:64] + "\0"
        else:
            cmd = cmd + (64 - len(cmd)) * "\0"

        return cmd.encode()

    def init(self):
        """
        The only purpose of this function is to test the serial com.
        The DUT board needs to answer "OK". It does nothing else.
        """
        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt("HANDSHAKE"))

        try:
            print("HANDSHAKE => " + self.readline().decode().rstrip())
        except TimeoutError:
            sys.stderr.print("Serial com timed out. Exit.")
            sys.exit(-1)

    def stop_continuous(self):
        """
        Sends a command to the device to stop continuous write/read experiment.
        """
        s = f"STOP"
        print(s + "\n\t\t\t\t\t\t=> ", end="")

        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

    def update_timer_raw(self, freqHz: float = 1, timer_reload: int = 10, pwm_width: int = 5):
        """
        Updates the prescaler of the timer responsible for generating
        periodic write/read + PWM sync output.
        The frequency should be between 1 and APB1 bus frequency Hz.
        """
        timer_reload = float(timer_reload)

        # Freq must be clamped depending on APB1 freq
        if freqHz < self.APB1_freqMHz / (65236.0 * timer_reload):
            freqHz = self.APB1_freqMHz / (65236.0 * timer_reload)
            print(f"Warning: freqHz was to low, clamped to {freqHz}.")
        elif freqHz > self.APB1_freqMHz / timer_reload:
            freqHz = self.APB1_freqMHz / timer_reload
            print(f"Warning: freqHz was to high, clamped to {freqHz}.")

        prescaler = round(self.APB1_freqMHz / (freqHz * timer_reload)) - 1

        # Update prescaler value
        s = f"TIMER_PSC>{prescaler}"
        print(s + f" ({self.APB1_freqMHz / ((prescaler-1) * timer_reload)} Hz)" + "\n\t\t\t\t\t\t=> ", end="")
        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))
        print(self.ser.readline().decode().rstrip())

        # Update autoreload value
        s = f"TIMER_ARR>{int(timer_reload)}"
        print(s + "\n\t\t\t\t\t\t=> ", end="")
        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))
        print(self.ser.readline().decode().rstrip())

        # Update pwm pulsewidth value
        s = f"TIMER_PWM_WIDTH>{pwm_width}"
        print(s + "\n\t\t\t\t\t\t=> ", end="")
        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))
        print(self.ser.readline().decode().rstrip())

    def update_timer(self, freqHz: float = 1, dutyCycle: float = 0.5, precision: int = 10):
        """
        Updates the timer to try to get as close to the specified parameters.
        The duty cycle is a ratio between 0.0 and 1.0; 0 is a flat pwm, 1 is always high, 0.5 is a square signal.
        Precision is the timer's auto-reload register value. Between 0 and 65535. Affects the max and min frequency that can be reached.
        Higher precision means lower max frequency.
        """

        timer_reload = precision
        pwm_width = int(float(timer_reload) * dutyCycle)

        self.update_timer_raw(freqHz, timer_reload, pwm_width)

    def flash_write(self, addr: str, data: str, trig: bool = False, word_span: int = 1, continuous: bool = False):
        """
        The device will store data in Flash mem at addr.
        The trig boolean tells the board whether it should fire a trigger signal
        on one of its outputs right before storing in RAM
        (for fault injection purpose) (only for non-continuous: pwm always on when continuous).
        """
        trig_str = "trig" if trig else "noTrig"

        if not continuous:
            # ONESHOT>FLASH>write>trig>0xaddr>
            s = f"ONESHOT>FLASH>write>{trig_str}>{addr}>{data}>{word_span}"
        else:
            # START>FLASH>write>trig>0xaddr>
            s = f"START>FLASH>write>{trig_str}>{addr}>{data}>{word_span}"

        print(s + "\n\t\t\t\t\t\t=> ", end="")

        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

    def flash_read(self, addr: str, trig: bool = False, word_span: int = 1, continuous: bool = False):
        """
        The device will read data in Flash mem at addr.
        The trig boolean tells the board whether it should fire a trigger signal
        on one of its outputs right before reading in RAM
        (for fault injection purpose) (only for non-continuous: pwm always on when continuous).
        For a span greater than 1, only the last read address is returned.
        """
        trig_str = "trig" if trig else "noTrig"

        if not continuous:
            # ONESHOT>FLASH>read>trig>0xaddr
            s = f"ONESHOT>FLASH>read>{trig_str}>{addr}>{word_span}"
        else:
            # START>FLASH>read>trig>0xaddr
            s = f"START>FLASH>read>{trig_str}>{addr}>{word_span}"

        print(s + "\n\t\t\t\t\t\t=> ", end="")

        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

    def flash_erase(self, sector: int, continuous: bool = False):
        """
        The device will erase the given page in Flash mem.
        """
        sector_clamped = max(min(sector, 7), 0)
        if sector != sector_clamped:
            print(f"Warning: page num was clamped: {sector} -> {sector_clamped}")

        if not continuous:
            s = f"ONESHOT>FLASH>erase>{sector_clamped}"
        else:
            s = f"START>FLASH>erase>{sector_clamped}"

        print(s + "\n\t\t\t\t\t\t=> ", end="")

        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

    def sram_write(self, addr: str, data: str, trig: bool = False, word_span: int = 1, nb_nops: int = 0, continuous: bool = False):
        """
        The device will store 32-bit data in SRAM mem at addr and along a certain span.
        A specified amount of nop instructions is inserted before each operation.
        The trig boolean tells the board whether it should fire a trigger signal
        on one of its outputs right before storing in RAM
        (for fault injection purpose).
        """
        trig_str = "trig" if trig else "noTrig"

        if not continuous:
            # ONESHOT>SRAM>write>trig>span>nb_nops>0xaddr>0xdata
            s = f"ONESHOT>SRAM>write>{trig_str}>{word_span}>{nb_nops}>{addr}>{data}"
        else:
            # START>SRAM>write>trig>span>nb_nops>0xaddr>0xdata
            s = f"START>SRAM>write>{trig_str}>{word_span}>{nb_nops}>{addr}>{data}"
        print(s + "\n\t\t\t\t\t\t=> ", end="")

        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

    def sram_read(self, addr: str, trig: bool = False, word_span: int = 1, nb_nops: int = 0, continuous: bool = False):
        """
        The device will read 32-bit data in SRAM mem at addr and along a certain span.
        A specified amount of nop instructions is inserted before each operation.
        The trig boolean tells the board whether it should fire a trigger signal
        on one of its outputs right before reading in RAM
        (for fault injection purpose).
        """
        trig_str = "trig" if trig else "noTrig"

        if not continuous:
            # ONESHOT>SRAM>read>trig>span>nb_nops>0xaddr
            s = f"ONESHOT>SRAM>read>{trig_str}>{word_span}>{nb_nops}>{addr}"
        else:
            # ONESHOT>SRAM>read>trig>span>nb_nops>0xaddr
            s = f"START>SRAM>read>{trig_str}>{word_span}>{nb_nops}>{addr}"

        print(s + "\n\t\t\t\t\t\t=> ", end="")

        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

    def aes_set_key(self, key: str):
        """
        The device will init and setup aes context for the specified key.
        Key string should be 32 hex characters long (ex: "1234ABCD56780909").
        """
        if len(key) != 32:
            print("AES key setup error: key should be 32 hex chars.")
            return

        # AES_SETUP>KEY>key
        s = f"AES_SETUP>KEY>{key}"
        print(s + "\n\t\t\t\t\t\t=> ", end="")
        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

    def aes_encrypt(self, plain: str, trig: bool = False, continuous: bool = False):
        """
        The device will run TinyAES C implementation and return result.
        Plaintext string should be 32 hex characters long (ex: "1234ABCD56780909").
        """
        trig_str = "trig" if trig else "noTrig"

        if len(plain) != 32:
            print("AES encrypt error: plain should be 32 hex chars.")
            return

        # Send plain text to encrypt
        s = f"AES_SETUP>PLAIN>{plain}>{trig_str}"
        print(s + "\n\t\t\t\t\t\t=> ", end="")
        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

        # Start one-shot or continuous encryption
        if not continuous:
            s = "AES_ENCRYPT"
        else:
            s = f"START>AES>{trig_str}"

        print(s + "\n\t\t\t\t\t\t=> ", end="")
        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

    def aes_decrypt(self, cipher: str, trig: bool = False, continuous: bool = False):
        """
        The device will run TinyAES C implementation and return result.
        Ciphertext string should be 32 hex characters long (ex: "1234ABCD56780909").
        """
        trig_str = "trig" if trig else "noTrig"

        if len(cipher) != 32:
            print("AES decrypt error: cipher should be 32 hex chars.")
            return

        # Send cipher text to decrypt
        s = f"AES_SETUP>CIPHER>{cipher}>{trig_str}"
        print(s + "\n\t\t\t\t\t\t=> ", end="")
        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

        # Start one-shot or continuous decryption
        if not continuous:
            s = "AES_DECRYPT"
        else:
            s = f"START>AES>{trig_str}"

        print(s + "\n\t\t\t\t\t\t=> ", end="")
        self.ser.reset_input_buffer()
        self.ser.write(self._cmd_fmt(s))

        print(self.ser.readline().decode().rstrip())

