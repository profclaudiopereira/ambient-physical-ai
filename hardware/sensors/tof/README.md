# ToF Sensor

## Presence Layer

---

# Hardware

M5Stack Unit Mini ToF-90°

Sensor:

VL53L0X

---

# Architectural Role

This sensor belongs to the:

## Presence Layer

Conceptual flow:

```text
Presence
→ Identity
→ Cognition
→ Ambient Transformation
→ Experience
```

Its purpose is to detect the existence and proximity of people or objects within the environment.

---

# Characteristics

* Time-of-Flight (ToF)
* I2C communication
* Range: 3 cm ~ 200 cm
* Resolution: 1 mm
* Fast response
* 90° forward-facing orientation

---

# Usage in Ambient Physical AI

Initial responsibilities:

* presence detection;
* distance measurement;
* environmental awareness;
* contextual triggering.

Future responsibilities:

* occupancy estimation;
* approach detection;
* contextual activation;
* ambient interaction triggers.

---

# Integration Target

Initial node:

```text
presence-node
```

Platform:

```text
CoreS3 Lite
```

---

# Status

Hardware acquired.

Integration planned for:

Presence Node v0.2
