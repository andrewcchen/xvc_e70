## XVC on SAM E70

Turn a SAME70-XPLD board into a JTAG adapter for FPGAs based on Xilinx Virtual Cable protocol.

### Requirements

- linux
- arm-none-eabi-gcc
- netcat (nc)

### Build

Build with:
```
make RELEASE=1
```

Flash `Release/xvc.hex` to the device with openocd or pyocd.

Before running gpnvm need to be set to enable running from flash, and tcm to max size.

In openocd:
```
atsamv gpnvm set 1
atsamv gpnvm set 7
atsamv gpnvm set 8
```

### Usage

- Connect the target usb port to your computer
- Check which tty device the board showed up as (usually /dev/ttyACM0)
- Set read write permissions on the tty device
- Run `./socket.sh <tty>`

Tip: you can use a udev rule to automatically set permissions.

### TODO

- Reset after usb disconnect
- Set gpnvm for tcm automatically
