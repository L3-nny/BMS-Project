// FILE:    include/BMS_Foundation.h
// PROJECT: Lithium-Ion Battery Management System (BMS)
// AUTHOR:  Lenny Mubea
// STATUS:  READ-ONLY after architecture review
#pragma once
 
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;
 
// SECTION 1: SAFETY CONSTANTS
// These thresholds are derived from the IEC 62133
// standard for portable lithium-ion cells. Do NOT change without
// a formal architecture review. Violations trigger Task 2 (Safety Operation).
 
// Under-voltage protection — below this, irreversible capacity loss begins
const float V_MIN     = 2.5f;   // Volts
 
// Over-voltage protection — above this, thermal runaway risk increases sharply
const float V_MAX     = 4.2f;   // Volts
 
// Over-temperature cutoff — SEI layer breakdown threshold
const float T_MAX     = 70.0f;  // Celsius
 
// Target operating temperature — optimal for Li-Ion chemistry
const float T_OPTIMAL = 25.0f;  // Celsius
 
// Resting OCV at ~50% State of Charge — used for reset and recovery
const float V_NOMINAL = 3.7f;   // Volts
 
// SECTION 2: BatteryCell STRUCT
// Declared as 'struct' (not 'class') so all members
// default to public access. This is intentional to enforce our
// "Direct Member Access" policy, making physics formulas readable.
// A default "Factory Constructor" initialises every cell to a safe
// nominal state so no cell ever starts uninitialised.

struct BatteryCell {
 
    // ── Identity ─────────────────────────────────────────────
    string  cellID;              // e.g. "CELL_01", 
 
    // ── Electrochemical State ────────────────────────────────
    float   voltage;             // in Volts (V) — valid range: [V_MIN, V_MAX]
    float   temperature;         // in Celsius (°C) — valid range: [-20, T_MAX]
    float   stateOfCharge;       // 0.0 = empty → 100.0 = full (percent)
    float   internalResistance;  // in Ohms (Ω) — increases with age & temp
    float   capacityAh;          // in Amp-hours (Ah) — degrades with cycles
 
    // ── Lifecycle Tracking ───────────────────────────────────
    int     cycleCount;          // full charge/discharge cycles completed
 
    // ── Safety Gate ──────────────────────────────────────────
    // CRITICAL: Every function in this system must check isConnected == true
    // before performing any operation. Setting this to false is the
    // software equivalent of blowing a fuse. Only Task 7 (Recovery Lead)
    // may set this back to true after a supervised fault clearance.
    bool    isConnected;
 
    // ── Factory Constructor ──────────────────────────────────
    // Initializes every new BatteryCell to a safe,
    // known nominal state. Mirrors a freshly charged cell at room temp.
    // Always call with a unique cellID: BatteryCell myCell("CELL_01");
    BatteryCell(string id = "CELL_XX")
        : cellID(id),
          voltage(V_NOMINAL),      // 3.7V resting OCV
          temperature(T_OPTIMAL),  // 25°C room temperature
          stateOfCharge(50.0f),    // 50% — mid-range, safe default
          internalResistance(0.05f),// 50mΩ — healthy new cell typical value
          capacityAh(50.0f),       // 50 Ah — nominal pack capacity
          cycleCount(0),
          isConnected(true)
    {}
 
};  // end struct BatteryCell
 
// SECTION 3: FUNCTION PROTOTYPES
// These are the ONLY public interfaces for the BMS.
// Each prototypis implemented in exactly one .cpp file in src/.
// Contributors: do NOT add new prototypes without architecture review.
 
//  Task 1: Physics Engine ──────────────────────────────
// Simulates real-world discharge: voltage sag (V = IR) and
// Peukert capacity loss. Also handles voltage recovery at rest.
void simulateDischarge(BatteryCell& cell, float loadAmps, float hours);
 
//  Task 2: Safe Operation ──────────────────────────────
// Digital fuse. Monitors V_MIN/V_MAX and T_MAX using try-throw-catch.
// Sets isConnected = false on any fault. Called after every cycle.
void checkSafetyLimits(BatteryCell& cell);
 
//  Task 3: Data Logger ──────────────────────────────────────
// Appends a timestamped CSV row to logs/bms_report.csv using ofstream.
// Format: Timestamp, CellID, Voltage, Temperature, SOC
void logSystemStatus(const BatteryCell& cell);
 
//  Task 4: Health Analysis───────────────────────────────
// Tracks cycleCount and applies 1% capacity fade per 100 cycles.
// Models real SEI layer growth and lithium plating degradation.
void calculateHealth(BatteryCell& cell, int newCycles);
 
//  Task 5: Thermal Management ─────────────────────────────────
// Bang-bang thermal controller: fan (−2°C) above 45°C,
// heater (+1°C) below 15°C. Dead band: [15°C, 45°C].
void manageThermalSystem(BatteryCell& cell);
 
//  Task 6: Passive Shunt Balancing──────────────────────
// Operates on the heap-allocated pack array. Finds min voltage,
// bleeds any cell > 0.05V above min by 0.01V per iteration.
void balanceCells(BatteryCell* pack, int size);
 
//  Task 7: System Recovery  ────────────────────────────────
// Supervised reset to safe state: voltage→V_NOMINAL, temp→T_OPTIMAL,
// isConnected→true. Does NOT reset cycleCount or capacityAh (audit trail).
void resetSystem(BatteryCell& cell);
 
//  Task 8: The SOC Mapping Engine ──────────────────────────
// Template function: accepts float or double ADC readings.
// OCV-to-SOC map: 4.2V=100%, 3.7V=50%, 3.0V=0%. Linear interpolation.
// Explicit instantiations required in task8_soc.cpp for the linker.
template <typename T>
T estimateSOC(T voltage);