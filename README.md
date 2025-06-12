# Monkey Kernel

_A tiny, educational 32-bit RISC-V kernel built in the jungle of Costa Rica._

Monkey Kernel fits in just a handful of C files yet walks you through the whole stack—from boot-strapping QEMU, into a minimal kernel, all the way to a user-space “shell”. It is **not** meant for production devices; its goal is to be an approachable playground for anyone who wants to learn how an OS really works.

---

## Features

| Area | What’s implemented |
|------|--------------------|
| **Boot** | SBI boot stub that jumps straight to `kernel_main` |
| **Kernel** | Cooperative scheduler with up to 8 processes (`struct process procs[8]`) |
| **Syscalls** | `SYS_PUTCHAR`, `SYS_GETCHAR`, `SYS_EXIT` (3 total) |
| **Drivers** | VirtIO-block driver (MMIO) + 512 B sector I/O |
| **File system** | Super‑block + inode table on `disk.img`; commands: `touch`, `ls` |
| **Userland** | Statically‑linked shell compiled to `shell.bin` and loaded by the kernel. Supports:<br>`> touch <file>` — create empty file<br>`> exit` — terminate shell |
| **Build & run** | One‑liner `./run.sh` that cross‑compiles with Clang/LLVM and boots QEMU (`qemu-system-riscv32`) |

---

## Quick start

```bash
# 1. Install deps (macOS shown; similar on Linux)
brew install llvm qemu riscv-gnu-toolchain   # or apt/yum equivalents

# 2. Clone and run
git clone https://github.com/ed-alvarez/monkeyos.git
cd monkeyos
./run.sh            # builds kernel + shell and boots QEMU
```

`run.sh` sets sensible defaults, but you can override paths via environment variables:

```bash
CC=/usr/bin/clang OBJCOPY=/usr/bin/llvm-objcopy QEMU=qemu-system-riscv32 ./run.sh
```

---

## Project layout

```
.
├── kernel.c / .h      – core scheduler, trap handler, VirtIO driver
├── fs.c / .h          – minimal block-based filesystem
├── shell.c            – user-space shell (compiled to shell.bin)
├── common.*           – ultra-thin libc: memcpy, printf, ...
├── string.*           – strlen/strncmp/strncpy
├── kernel.ld          – kernel linker script (loads at 0x8020_0000)
├── user.ld            – user ELF linker script (base 0x0100_0000)
├── disk.img           – blank 16 MB disk image (formatted at first boot)
└── run.sh             – build & QEMU launch helper
```

---

## How it works (high level)

1. **Boot** – QEMU’s OpenSBI ROM loads the ELF at `0x80200000`, then jumps to `boot()` (in `.text.boot`).
2. **Kernel init** – `kernel_main` zeroes BSS, sets up paging (`Sv32`) and a single VirtIO‑block virtqueue.
3. **File‑system** – On first boot `fs_mkfs()` writes a super‑block at sector 0 and an empty inode table (sectors 1‑9).
4. **User process** – The embedded `shell.bin` is mapped at `0x01000000`; a trap gateway handles syscalls.
5. **Scheduling** – The shell is the only runnable process, but the infrastructure can hold up to eight.
6. **I/O** – `putchar/getchar` delegate to the SBI console; block I/O goes through VirtIO MMIO registers.

---

## Memory map

| Address          | Purpose                          |
|------------------|----------------------------------|
| `0x8020_0000`    | Kernel text (see `kernel.ld`)    |
| `0x8022_0000`    | 128 KB kernel stack top          |
| `0x1000_1000`    | VirtIO‑block MMIO base           |
| `0x0100_0000`    | User process text (see `user.ld`)|

---

## Road‑map / ideas

* Round‑robin pre‑emptive scheduler (timer interrupts)  
* `ls`, `cat`, and basic pipe in the shell  
* ELF loader (instead of embedding binaries)  
* Paging‑based memory protection & dynamic allocation  
* More drivers: UART, CLINT (timer), PLIC (interrupts)

Contributions & suggestions are welcome—feel free to open issues or PRs!

---

## License

This project is released under the MIT license. See [LICENSE](LICENSE) for details.
