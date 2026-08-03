# nRF52840 Bare-Metal Bootloader

Minimalny, działający bootloader dla nRF52840 DK – bez SoftDevice, bez Zephyr, bez bibliotek Nordic. Tylko C i rejestry.

## Co to robi

Po resecie MCU wykonuje bootloader (LED1 świeci ~2 s), potem skacze do aplikacji głównej, która zaczyna migać LED2. Całość to ~150 linii C + dwa skrypty linkera.

## Struktura

```
nrf_bootloader/
├── bootloader/          # Program startowy (64 KB Flash)
│   ├── startup.c        # Inicjalizacja .data/.bss + tablica wektorów
│   ├── main.c           # LED1 ON → delay → jump_to_app(0x10000)
│   ├── linker.ld        # Mapa pamięci: 0x00000000, 64K
│   └── Makefile
│
└── app/                 # Aplikacja główna (960 KB Flash)
    ├── startup.c        # Identyczny jak w bootloaderze
    ├── main.c           # Miganie LED2 (P0.14)
    ├── linker.ld        # Mapa pamięci: 0x00010000, 960K
    └── Makefile
```

## Wymagania

- [Arm GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (`arm-none-eabi-gcc`)
- [nRF Command Line Tools](https://www.nordicsemi.com/Products/Development-tools/nrf-command-line-tools) (`nrfjprog`)
- nRF52840 DK (PCA10056)

## Szybki start

```bash
# 1. Kompilacja bootloadera
cd bootloader
make

# 2. Kompilacja aplikacji
cd ../app
make

# 3. Wyczyść Flash (usuwa stary SoftDevice)
nrfjprog --eraseall

# 4. Wgraj bootloader
cd ../bootloader
nrfjprog --program bootloader.hex --sectorerase --verify

# 5. Wgraj aplikację i zresetuj
cd ../app
nrfjprog --program app.hex --sectorerase --verify --reset
```

Po resecie: LED1 świeci 2 s → LED2 miga co ~1 s.

## Jak to działa (w jednym zdaniu)

Bootloader startuje pod `0x00000000`, ustawia `VTOR = 0x00010000`, ładuje MSP z tablicy wektorów aplikacji i skacze do jej `Reset_Handler` – aplikacja nie wie, że był bootloader.

## Debugowanie

| Objaw | Rozwiązanie |
|---|---|
| Nic się nie świeci | `nrfjprog --eraseall`, sprawdź kabel USB do **górnego** portu (J-Link) |
| LED1 świeci w nieskończoność | `jump_to_app` może być zinlinowany – sprawdź `__attribute__((noinline))` |
| LED1 OK, LED2 nie miga | Sprawdź czy `.isr_vector` jest pod `0x10000`: `arm-none-eabi-objdump -h app.elf \| head` |

## Następne kroki

1. Flaga update w `GPREGRET` – bootloader czeka zamiast skakać
2. Odbiór firmware przez UART (rejestry, bez bibliotek)
3. Weryfikacja CRC po flashowaniu
4. Zamiana UART na BLE (Nordic UART Service → własny protokół)

## Licencja

MIT – rób z tym co chcesz.