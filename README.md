# ASCII Video Call in C

A proof-of-concept ASCII-based video calling application using FFmpeg, ncurses, and TCP sockets.

## Features
- Real-time camera capture and ASCII conversion.
- Terminal rendering with `ncurses`.
- Bidirectional TCP networking (P2P).

## Requirements
- FFmpeg (libavcodec, libavformat, libswscale)
- ncurses
- GCC and Make

## 🛠️ Setup

This project requires several dependencies to compile and run. You can install them automatically using the provided script:

### 🐧 On Linux (Ubuntu or Arch):

```bash
## Build Instructions
git clone https://github.com/courecta/cvc
cd cvc
chmod +x setup.sh
./setup.sh
make
```