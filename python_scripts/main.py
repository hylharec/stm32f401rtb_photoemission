"""
Simple script to send commands to nucleo-f446RE developpment board.
The microC's DMA listens on the UART2 and waits for 64 bytes before
raising an interrupt that handles the received command.
"""

import time
from NucleoF446RE import NucleoF446RE

if __name__ == "__main__":
    # ################################################
    # Beginning of program

    dut = NucleoF446RE(
        "/dev/ttyACM0",
        9600,
        timeout=500.0,
        rtscts=False,
        APB1_freqMHz=42_000_000
        )


    ############
    def sram_test():
        # Test function to write/read in SRAM
        print("")
        print("=========================================")
        print("========= START OF SRAM TEST ============")

        # Read 2 consecutive SRAM words (32 bits)
        dut.sram_read("2000C1F0", True)
        dut.sram_read("2000C1F4", False)

        # Write in previous addresses
        dut.sram_write("2000C1F0", hex(0x12345678)[2:].zfill(8), False)
        dut.sram_write("2000C1F4", hex(0x87654321)[2:].zfill(8), True)

        # Read again to verify that a different value was written
        dut.sram_read("2000C1F0", False)
        dut.sram_read("2000C1F4", False)

        # Write 2 consecutive addresses in one command (nops and trigger applied for each)
        dut.sram_write("2000C1F0", "A000000B", False, 2, 10)

        # Read again to verify that a different value was written
        dut.sram_read("2000C1F0", False)
        dut.sram_read("2000C1F4", False)

        # Choose continuous mode frequency (updates timer prescaler)
        dut.update_timer(200)

        # Write 2 consecutive addresses continuously at set frequency during 4 seconds
        dut.sram_write("2000C1F0", "BEBACAFE", True, 2, 1, True)
        input("Press to continue...")
        dut.stop_continuous()

        # Choose new frequency
        dut.update_timer(200)

        # Continuous mode again during 4 seconds
        dut.sram_write("2000C1F0", "0000DCD0", True, 2, 5, True)
        input("Press to continue...")
        dut.stop_continuous()

        # Read again to verify that a different value was written
        dut.sram_read("2000C1F0")
        dut.sram_read("2000C1F4")
    ############
    def flash_test():
        # Test function to write/read in Flash
        print("")
        print("=========================================")
        print("======== START OF FLASH TEST ============")

        # Read a word (32 bits)
        dut.flash_read("0801C1F0")

        # Write a word at same address
        dut.flash_write("0801C1F0", "0F0F0F0F")

        # Read again to verify that content is coherent with previous actions
        dut.flash_read("0801C1F0")

        # Erase sector that contains previously written address
        dut.flash_erase(4)

        # Read again to verify erasing
        dut.flash_read("0801C1F0")

        # Choose continuous mode frequency (updates timer prescaler)
        dut.update_timer(200)

        # Write continuously at set frequency during 4 seconds
        dut.flash_read("0801C1F0", True, 1, True)
        input("Press to continue...")
        dut.stop_continuous()

        # Choose new frequency
        dut.update_timer(200)

        # Continuous mode again during 4 seconds
        dut.flash_write("0801C1F0", "0000CAFE", True, 1, True)
        input("Press to continue...")
        dut.stop_continuous()

        # Read again to verify that a different value was written
        dut.flash_read("0801C1F0")
    ############
    def aes_test():
        print("")
        print("=========================================")
        print("======== START OF AES TEST ==============")

        dut.aes_set_key("2b7e151628aed2a6abf7158809cf4f3c")

        #dut.aes_encrypt("3243f6a8885a308d313198a2e0370734")

        #dut.aes_decrypt("3925841D02DC09FBDC118597196A0B32")

        dut.update_timer(1000, 0.5, 4000)
        dut.aes_encrypt("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", True, True)
        input("Press to continue...")
        dut.stop_continuous()
    ############

    #dut.sram_read("2000C1F0", True, 1, 5, False)

    # Choose new frequency
    #dut.update_timer(70, 0.50, 4000)

    # Continuous mode again during 4 seconds
    #dut.sram_write("2000C1F0", "0000DCD0", True, 56000, 5, True)

    #time.sleep(5)
    #dut.stop_continuous()

    #aes_test()

    dut.flash_read("08040000", word_span=3)
    dut.flash_read("08040000")
    dut.flash_read("08040004")
    dut.flash_read("08040008")

    #sram_test()
    #flash_test()
    #dut.flash_write("0801C1F0", "0F0F0F0F")
    #aes_test()

    #dut.update_timer(10, 0.50, 4000)

    #dut.flash_erase(4, True)
    #dut.flash_write("08040000", "00000000", False, 2048, True)

    #dut.sram_read("20000F00", True, 1, 0, False)
    #dut.sram_write("20000F00", "12345678", True, 1, 0, False)
    #dut.sram_write("20000F04", "9ABCDEF0", True, 1, 0, False)
    #dut.sram_read("20000F00", True, 1, 0, False)
    #dut.sram_read("20000F00", True, 2, 0, False)

    #dut.sram_write(
    #    addr="2000903C",
    #    data="00000000",
    #    trig=True,
    #    word_span=1,
    #    nb_nops=1,
    #    continuous=False
    #)

    #dut.sram_write("2000C1F0", "0000DCD0", True, 4, 5, True)
    #input("Press key to stop continuous...")
    #dut.stop_continuous()

    #    #for i in range(300):
    #    #    for j in range(500):
    #    #      dut.flash_write("0" + hex(0x08000000 + i * 4)[2:], "FFFFFFFF");
    #    for i in range(32):
    #      dut.flash_write("0" + hex(0x08001000 + i * 4)[2:], "00000000")

    #while True:
    #  dut.flash_erase(2)

    # End of program
    # ################################################
