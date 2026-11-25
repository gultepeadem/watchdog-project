# Watchdog - POSIX Signal-Based Process Monitoring and Management System

Sakarya University - Department of Software Engineering
SWE-301 System Programming (2026-2027 Fall) - Term Project 1

## Overview
watchdog is a POSIX-compliant process supervision program written in C. It spawns and monitors child worker processes, restarts unexpectedly terminated workers up to a maximum limit of 3 attempts, reports live child states on SIGUSR1, performs periodic health checks via SIGALRM, and executes a graceful shutdown sequence on SIGINT or SIGTERM.

## Project Structure
- Makefile
- README.md
- include/ (signal_manager.h, process_manager.h, logger.h)
- src/ (main.c, signal_manager.c, process_manager.c, logger.c)
- workers/ (worker.c)
- tests/ (test_scenarios.sh)
- docs/ (REPORT.pdf)

## Installation & Build
Ensure gcc and make are installed on a Linux environment:
    make

To clean compiled binaries and logs:
    make clean

## Usage
Run the supervisor from the project root directory (required so the relative path `./workers/worker` resolves correctly):
    ./watchdog

From another terminal, send signals to the running watchdog process:
    kill -SIGUSR1 <WATCHDOG_PID>
    kill -SIGTERM <WATCHDOG_PID>

## Running Automated Tests
    ./tests/test_scenarios.sh

## Group Members & Responsibilities
- Adem Gultepe (B231202300, gultepeadem) - Student 1: Signal Management & Main Control Loop (signal_manager.h/.c, main.c)
- Irem Ekmekci (B231202055, iremekmekci) - Student 2: Child Process Management & Worker (process_manager.h/.c, worker.c)
- Zeynep Pektas (B211202054, Zeynosh) - Student 3: Logging, Build System, Testing & Documentation (logger.h/.c, Makefile, tests/test_scenarios.sh, README.md)
