# FreeRTOS Multi-Task Synchronization and Scheduling
This repository contains an embedded software project demonstrating real-time task scheduling and resource synchronization using FreeRTOS. The application models a concurrent data-processing environment typical in intelligent transport systems (ITS) and vehicle sensor networks.

## Project Overview
This repository contains an embedded software project demonstrating real-time task scheduling and resource synchronization using FreeRTOS. The application models a concurrent data-processing environment typical in intelligent transport systems (ITS) and vehicle sensor networks.

## Technical Stack
* **Language:** C
* **Framework:** FreeRTOS
* **Core Concepts:** Multi-threading, Mutexes, Critical Sections, Periodic Scheduling

## System Architecture
The software schedules three distinct tasks with varying priorities and execution frequencies:
* **Task 1 (1000ms) & Task 2 (500ms):** Act as sensor data generators, safely incrementing a shared variable using mutex locks to prevent race conditions. Task 2 also records these values into a fixed-size circular buffer.
* **Task 3 (5000ms - Highest Priority):** Acts as the data processor. It periodically preempts the lower-priority tasks, safely locks the data array, computes a moving average of the recent samples, and outputs the formatted results.

## Skills Demonstrated
This project highlights proficiency in embedded C programming, memory protection using semaphores, and managing deterministic execution in a Real-Time Operating System environment.
