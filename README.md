# Autonomous Fire-Fighting Robot

**Result:** 2nd Place — Al-Nahrain University Graduation Projects Exhibition (May–Jun 2025)

An autonomous robot that detects flame sources using IR flame sensors and
drives toward them to extinguish the fire with an onboard water pump.
Inspired by the California wildfires as a low-cost early-response concept.

## Components
- Arduino Uno
- 3x IR (4-pin) flame sensors
- L298 motor driver + 4x BO motors (with wheels)
- Mini servo
- 5–9V water pump + tubing, water reservoir
- 2x 3.7V 18650 batteries
- TIP-122 transistor + 104pF capacitor + 1K resistor (pump switching)
- Solderless breadboard, jumper wires

## How it works
Three flame sensors (left, center, right) continuously scan for a flame.
Depending on which sensor triggers, the robot steers left, right, or
straight toward the source. Once the flame is centered and close, the robot
stops and activates the water pump via a transistor switch for a fixed
duration, then resumes scanning.

## Note on this repository
This firmware was written after the original competition build (the project
predates this GitHub account) to document the system design and preserve a
working reference implementation of the project as originally conceived and
presented.

## Possible improvements
- Replace the fixed-duration pump cycle with a flame-sensor feedback loop
  (stop pumping once the flame sensor no longer detects flame)
- Add an ultrasonic sensor for obstacle avoidance while approaching
- Log detection events over Serial for post-run analysis
