# QuaQua Image Compressor (Quadtree Compression)

Welcome to **QuaQua**, a simple yet educational image compressor using the **Divide and Conquer** strategy via **Quadtree decomposition**. This project was created as part of a mini-assignment for the Algorithm Strategy course.

---

## 1. Introduction

This program compresses RGB images using the **Quadtree** algorithm. It works by recursively dividing the image into four blocks, analyzing whether each block is homogeneous enough using a selected error metric, and only subdividing further if necessary. The output includes:

- A compressed image
- An optional **GIF** showing the step-by-step formation of the quadtree structure

---

## 2. Requirements and Setup

This project is written in **C++17** and uses only a simple `Makefile`. No external dependencies are needed. The only required files are:

- `stb_image.h` and `stb_image_write.h` (included in the repo)
- `gif.h` by Charlie Tangora (included in the repo)

### You need:
- C++17-compatible compiler (`g++`)
- `make` utility

---

## 3. Compilation

To compile the program, simply run:

```bash
make
```

To run:

```bash
./bin/main.exe
```

## Author
Adinda Putri 13523071

