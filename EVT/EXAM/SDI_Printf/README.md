# SDI_Printf

Demonstrates **SDI-Print** on the CH32H417: `printf()` output is streamed over the
1/2-wire debug link and read back through the WCH-Link's USB-CDC serial port, with
**no USART peripheral and no extra wiring**.

## How it works

`printf()` → `_write()` (in `SRC/Debug/debug.c`, `SDI_PRINT == SDI_PR_OPEN` branch)
writes characters into the RISC-V Debug Module data registers `DATA0`/`DATA1`.
These are memory-mapped into the hart's address space at:

```
address(DATA0) = 0xE0000000 + hartinfo.dataaddr
```

`hartinfo` is the standard RISC-V debug register (DMI `0x12`). On the CH32H417 it
reads `0x00212340` → `dataaddr = 0x340`, `dataaccess = 1` (memory-mapped), so:

```
DATA0 @ 0xE0000340
DATA1 @ 0xE0000344
```

> Note: the CH32V-series parts use `0xE0000380` (their `dataaddr` is `0x380`).
> That value does **not** apply to the CH32H417 — writes would go nowhere and no
> output would appear. Always derive the address from `hartinfo` for a new part.

Once the WCH-Link is told to enable SDI print, it polls `DATA0` over the debug
transport, drains the bytes, and forwards them to its serial port.

## Enabling SDI print in the project

This example sets `SDI_PRINT` to `SDI_PR_OPEN` (`1`) in `V3F/User/ch32h417_conf.h`.
Remove that line to fall back to normal USART `printf`. The transport itself lives
in the shared `SRC/Debug/debug.c` / `SRC/Debug/debug.h`.

## Run

1. Build and download `V3F` (WCH-Link, 1-wire serial).
2. Enable SDI print on the probe. With [`wlink`](https://github.com/ch32-rs/wlink):
   ```
   wlink sdi-print enable --chip CH32H41X
   ```
   (MounRiver Studio: tick **Enable SDI Printf** in the download dialog.)
3. Open the WCH-Link serial port (e.g. `/dev/ttyACM*` @ 115200 8N1).

Expected output:

```
SystemClk:400000000
V3F SystemCoreClk:100000000
SDI print test 0
SDI print test 1
SDI print test 2
...
```

## Notes

- Verified on `CH32H417QEU` (ChipID `0x4170052d`), WCH-LinkE firmware v2.18.
- Only the V3F core variant is provided here; the V5F core uses the same shared
  Debug Module data-register mapping (`0xE0000340`), so a V5F variant is analogous.
