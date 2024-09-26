# COMP7005_Assignment1 Repository Guide

Welcome to the `COMP7005_Assignment1` repository. This guide will help you set up and run the provided scripts.

## **Table of Contents**

1. [Cloning the Repository](#cloning-the-repository)
2. [Prerequisites](#Prerequisites)
3. [Building the programs](#building-the-programs)
4. [Running the programs](#running-the-programs)

## **Cloning the Repository**

Clone the repository using the following command:

```bash
git clone https://github.com/iamBillyNguyen/COMP7005_Assignment1.git
```

Navigate to the cloned directory:

```bash
cd COMP7005_Assignment1
```

Ensure the scripts are executable:

```bash
chmod +x *.sh
```

## **Prerequisites**

- to ensure you have all of the required tools installed, run:
```bash
./check-env.sh
```

If you are missing tools follow these [instructions](https://docs.google.com/document/d/1ZPqlPD1mie5iwJ2XAcNGz7WeA86dTLerFXs9sAuwCco/edit?usp=drive_link).

## **Building the programs**

To build the program run:

```bash
make clean
make all
```

## **Running the programs**

To create a new project from the template, run:

### **Server**
```bash
./server <socket path>
```

### **Client**
```bash
./client <socket path> <path to and name of file to read>
```
