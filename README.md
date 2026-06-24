# FreeRTOS Task Synchronization & Queue Management ⏱️⚙️

## Overview
This repository contains an embedded C application demonstrating fundamental Real-Time Operating System (RTOS) concepts using **FreeRTOS**. It serves as a foundational implementation of preemptive task scheduling and Inter-Task Communication (ITC). 

Rather than relying on hardware interrupts or messy global variables, this project utilizes a FreeRTOS **Message Queue** to safely pass data between decoupled, concurrent tasks running on the microcontroller.

## Architecture & Data Flow
The application architecture is built around two primary tasks that run concurrently:

1. **The Transmitter Task (Tx):** * Executes periodically using `vTaskDelay` to ensure deterministic execution.
   * Generates a predefined value (e.g., `100UL`).
   * Safely posts this value to the back of the shared FreeRTOS queue using `xQueueSend`.

2. **The Receiver Task (Rx):**
   * Blocks efficiently on the shared queue using `xQueueReceive` (yielding CPU time while waiting).
   * Automatically wakes up when data arrives in the queue.
   * Processes the received data and toggles a hardware state (e.g., blinking an LED) to confirm successful transmission.

## Key RTOS Concepts Demonstrated
* **Preemptive Scheduling:** Both tasks are registered with the FreeRTOS kernel scheduler (`vTaskStartScheduler`), which manages CPU allocation based on task priority.
* **Thread-Safe Communication:** Eliminates race conditions by avoiding global variables. Data is passed strictly through the kernel-managed `xQueueCreate` structure.
* **Blocking vs. Polling:** The Rx task demonstrates efficient power management by blocking indefinitely (`portMAX_DELAY`) until data is available, rather than actively polling via CPU-intensive while-loops.

## Tech Stack
* **Kernel:** FreeRTOS
* **Language:** C (Standard Embedded)
* **Concepts:** Task Synchronization, Message Queues, Context Switching
