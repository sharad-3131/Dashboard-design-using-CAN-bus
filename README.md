# Dashboard-design-using-CAN-bus

# 🚗 CAN-Based Automotive Dashboard System (LPC2129)

This project implements a multi-node **CAN-based embedded system** using the **LPC2129 microcontroller**, simulating a real-time automotive dashboard. It includes three major nodes: **Main Node**, **Fuel Node**, and **Indicator Node**, communicating over the **Classical CAN protocol**. The system is designed to monitor fuel level, display real-time status on an LCD, and control turn signal indicators based on user input via CAN messages.

---

## 🧠 Project Architecture

### 1. **Main Node (LPC2129)**
- Acts as the **central controller** of the system.
- Interfaces with:
  - **RTC** (Real Time Clock)
  - **LCD Display**
  - **Temperature Sensor**
- **CAN Transmission**:
  - Sends `"L"` or `"R"` commands to control left/right turn indicators.
  - Periodically sends system status or dummy commands.
- **Interrupt Handling**:
  - Sends messages via CAN on external interrupt trigger.

### 2. **Fuel Node**
- Measures **fuel level percentage** (simulated or via analog sensor).
- Continuously transmits **fuel level via CAN**.
- ID used: `1`
- Data Format: Integer percentage (0–100)

### 3. **Indicator Node**
- Receives `"L"` or `"R"` CAN messages with ID `2`.
- **Controls 8 LEDs** (P0.0 to P0.7) to simulate turn indicators.
  - `"L"`: Left pattern (0 → 7)
  - `"R"`: Right pattern (7 → 0)
- Toggling mechanism: same command toggles ON/OFF the pattern.
- Fast response loop checks for new commands during blinking.

---

## 🔧 Hardware Requirements

- **LPC2129 development boards** (minimum 3)
- **CAN transceivers** (e.g., MCP2551 or SN65HVD230)
- **8 LEDs + Resistors** (for indicator node)
- **16x2 LCD** (for main node display)
- **RTC Module** (I2C-based like DS1307)
- **DHT11 / LM35** (for temperature simulation)
- Power supply, jumpers, connectors

---

## 🔌 Communication Protocol

### CAN Bus Configuration:
- **Baud Rate**: Configured via CAN1/CAN2 control registers
- **Message Format**: Standard 11-bit ID
- **Message IDs**:
  - `0x01` – Fuel level from Fuel Node
  - `0x02` – Indicator control from Main Node

### CAN Frame Structure:
| Field     | Value Example | Description                   |
|-----------|---------------|-------------------------------|
| ID        | 0x01 / 0x02   | Sender type (Fuel / Main)     |
| Data1     | `'L'` / `'R'` | Command or value              |
| Data2     | (unused)      | Reserved for future expansion |

---

## 📁 File Structure

