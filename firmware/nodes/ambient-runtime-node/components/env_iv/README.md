# ENV-IV Sensor Driver

## Ambient Physical AI

### Native ESP-IDF Driver for the M5Stack ENV-IV Unit

---

# Overview

The `env_iv` component provides the low-level sensor interface for the M5Stack ENV-IV unit used by the Ambient Runtime.

The ENV-IV unit combines two independent environmental sensors:

- SHT40;
- BMP280.

The component exposes separate public APIs for each sensor and returns typed measurement structures containing the converted physical values.

It is implemented using native ESP-IDF I²C APIs and does not depend on Arduino-based frameworks or higher-level sensor libraries.

---

# Purpose

The purpose of the `env_iv` component is to provide deterministic and reusable access to environmental measurements required by the Ambient Runtime.

Current responsibilities include:

- communicating with the SHT40 through I²C;
- acquiring temperature and relative humidity;
- converting SHT40 raw measurements into physical units;
- limiting relative humidity to the valid range;
- communicating with the BMP280 through I²C;
- validating the BMP280 chip identifier;
- reading factory calibration coefficients;
- acquiring raw pressure and temperature data;
- applying BMP280 compensation algorithms;
- returning compensated temperature and atmospheric pressure.

The component performs no application-level interpretation of the measurements.

---

# Position within the Ambient Runtime

The ENV-IV driver belongs to the device layer of the Ambient Runtime.

```text
Ambient Runtime Application
        │
        ▼
env_iv
        │
        ▼
ESP-IDF I²C Master Driver
        │
        ▼
M5Stack ENV-IV
        │
        ├── SHT40
        │   ├── Temperature
        │   └── Relative Humidity
        │
        └── BMP280
            ├── Temperature
            └── Atmospheric Pressure
```

Higher-level runtime components consume only the converted measurement structures.

They remain independent from sensor registers, calibration coefficients and raw measurement formats.

---

# Supported Sensors

## SHT40

The SHT40 provides:

- temperature;
- relative humidity.

The current implementation communicates with the sensor at I²C address:

```text
0x44
```

The driver sends the measurement command:

```text
0xFD
```

After the conversion delay, the component reads the raw temperature and humidity values and converts them into physical units.

---

## BMP280

The BMP280 provides:

- temperature;
- atmospheric pressure.

The current implementation communicates with the sensor at I²C address:

```text
0x76
```

Before performing a full measurement, the driver validates the sensor identifier.

Expected chip identifier:

```text
0x58
```

The component also reads the device calibration registers before applying the compensation algorithms required by the BMP280.

---

# Responsibilities

The `env_iv` component is responsible only for environmental sensor acquisition and conversion.

## SHT40 Acquisition

Perform a complete SHT40 measurement cycle, including:

- temporary I²C device registration;
- measurement command transmission;
- conversion delay;
- raw data acquisition;
- temperature conversion;
- humidity conversion;
- humidity range validation;
- temporary device removal.

---

## BMP280 Identification

Read the BMP280 chip identifier through the dedicated diagnostic API.

This operation allows higher-level code or engineering tests to verify that the expected sensor is present before relying on measurement data.

---

## BMP280 Calibration

Read the factory calibration coefficients stored in the BMP280 calibration register block.

These coefficients are required by the temperature and pressure compensation algorithms.

The component keeps the calibration representation private to the implementation.

---

## BMP280 Acquisition

Perform a complete BMP280 measurement cycle, including:

- temporary I²C device registration;
- chip identifier validation;
- calibration data acquisition;
- sensor control-register configuration;
- conversion delay;
- raw pressure and temperature acquisition;
- compensated temperature calculation;
- compensated pressure calculation;
- temporary device removal.

---

## Typed Measurement Output

Return converted measurements through explicit data structures.

The component does not expose raw sensor frames to higher application layers.

---

# Component Architecture

```text
Application
        │
        ▼
env_iv Public API
        │
        ├───────────────────────────┐
        │                           │
        ▼                           ▼
SHT40 Read Path              BMP280 Read Path
        │                           │
        ▼                           ▼
Raw Measurement              Chip Validation
        │                           │
        ▼                           ▼
Physical Conversion          Calibration Read
        │                           │
        ▼                           ▼
Typed SHT40 Data             Raw Measurement
                                    │
                                    ▼
                           Compensation Algorithms
                                    │
                                    ▼
                            Typed BMP280 Data
```

The implementation maintains separate acquisition paths for the two sensors while exposing them through the same ESP-IDF component.

---

# Measurement Data Model

The component exposes two independent measurement structures.

## SHT40 Data

```c
typedef struct {
    float temperature_c;
    float humidity_percent;
} env_iv_sht40_data_t;
```

Fields:

| Field | Description | Unit |
|---|---|---|
| `temperature_c` | Converted SHT40 temperature | Degrees Celsius |
| `humidity_percent` | Converted relative humidity | Percent |

The humidity result is constrained to the range:

```text
0.0% to 100.0%
```

---

## BMP280 Data

```c
typedef struct {
    float temperature_c;
    float pressure_hpa;
} env_iv_bmp280_data_t;
```

Fields:

| Field | Description | Unit |
|---|---|---|
| `temperature_c` | Compensated BMP280 temperature | Degrees Celsius |
| `pressure_hpa` | Compensated atmospheric pressure | Hectopascals |

The pressure value returned by the driver is already converted to `hPa`.

---

# Public API

The component exposes three public functions.

## SHT40 Measurement

```c
esp_err_t env_iv_sht40_read(
    i2c_master_bus_handle_t bus,
    env_iv_sht40_data_t *data
);
```

Performs a complete SHT40 measurement and stores the converted temperature and humidity values in the supplied structure.

Parameters:

| Parameter | Description |
|---|---|
| `bus` | I²C bus used to communicate with the ENV-IV unit. |
| `data` | Output structure receiving the converted SHT40 measurements. |

The function returns `ESP_ERR_INVALID_ARG` when the output pointer is null.

---

## BMP280 Identification

```c
esp_err_t env_iv_bmp280_read_id(
    i2c_master_bus_handle_t bus,
    uint8_t *chip_id
);
```

Reads the BMP280 chip identifier.

Parameters:

| Parameter | Description |
|---|---|
| `bus` | I²C bus used to communicate with the sensor. |
| `chip_id` | Output parameter receiving the device identifier. |

This function is useful for diagnostics and hardware validation.

---

## BMP280 Measurement

```c
esp_err_t env_iv_bmp280_read(
    i2c_master_bus_handle_t bus,
    env_iv_bmp280_data_t *data
);
```

Performs the complete BMP280 measurement sequence.

The function:

- validates the chip identifier;
- reads calibration coefficients;
- configures the measurement control register;
- waits for conversion;
- reads raw data;
- applies compensation;
- returns temperature and pressure.

If the chip identifier differs from `0x58`, the function returns:

```text
ESP_ERR_INVALID_RESPONSE
```

---

# Dependencies

The component depends on:

- ESP-IDF I²C Master Driver;
- FreeRTOS timing services.

Its ESP-IDF component registration is:

```cmake
idf_component_register(
    SRCS "env_iv.c"
    INCLUDE_DIRS "include"
    REQUIRES driver
)
```

The component has no direct dependency on:

- `ambient_console`;
- `pahub`;
- `tab5_platform`;
- network services;
- semantic communication components.

The I²C bus is supplied by the caller.

---

# Measurement Flow

The `env_iv` component performs independent acquisition cycles for the SHT40 and BMP280 sensors.

Each acquisition creates a temporary I²C device handle, performs the complete measurement sequence and releases the device before returning.

The component maintains no persistent communication state.

---

# SHT40 Measurement Flow

The SHT40 acquisition sequence follows the workflow below.

```text
Application
        │
        ▼
Create Temporary I²C Device
        │
        ▼
Send Measurement Command
        │
        ▼
Wait for Conversion
        │
        ▼
Read Raw Measurement
        │
        ▼
Convert Temperature
        │
        ▼
Convert Relative Humidity
        │
        ▼
Limit Humidity Range
        │
        ▼
Release I²C Device
        │
        ▼
Return Measurement
```

The returned values are already converted into engineering units.

No additional conversion is required by the caller.

---

# BMP280 Measurement Flow

The BMP280 acquisition sequence is more elaborate because it requires sensor validation and compensation.

```text
Application
        │
        ▼
Create Temporary I²C Device
        │
        ▼
Read Chip Identifier
        │
        ▼
Validate Device
        │
        ▼
Read Calibration Registers
        │
        ▼
Configure Measurement
        │
        ▼
Wait for Conversion
        │
        ▼
Read Raw Temperature
        │
        ▼
Read Raw Pressure
        │
        ▼
Temperature Compensation
        │
        ▼
Pressure Compensation
        │
        ▼
Release I²C Device
        │
        ▼
Return Measurement
```

Only compensated values are exposed through the public API.

---

# Calibration and Compensation

Unlike the SHT40, the BMP280 requires factory calibration coefficients to produce accurate measurements.

The component automatically:

- reads the factory calibration registers;
- stores the coefficients internally;
- applies the Bosch compensation algorithms;
- converts the compensated pressure to hectopascals (hPa).

These implementation details remain private to the component.

Higher-level runtime modules receive only the final engineering values.

---

# Engineering Principles

The implementation follows the architectural principles adopted throughout the Ambient Physical AI project.

## Sensor Independence

Although both sensors belong to the same ENV-IV unit, each acquisition path is implemented independently.

This separation simplifies maintenance and allows each sensor to evolve independently.

---

## Stateless Operation

The component maintains no persistent sensor context.

Every measurement performs a complete acquisition cycle from device registration to device removal.

This approach minimizes internal state and simplifies reuse.

---

## Hardware Encapsulation

Sensor registers, calibration coefficients and compensation algorithms remain completely encapsulated.

Application components interact only with typed measurement structures.

---

## Explicit Data Contracts

The public API returns strongly typed structures rather than generic buffers.

This improves readability while preventing ambiguity between measurements produced by different sensors.

---

# Current Status

The current implementation provides the following validated functionality.

| Capability | Status |
|------------|--------|
| SHT40 communication | Validated |
| SHT40 temperature measurement | Validated |
| SHT40 humidity measurement | Validated |
| BMP280 identification | Validated |
| BMP280 calibration loading | Validated |
| BMP280 temperature compensation | Validated |
| BMP280 pressure compensation | Validated |
| Environmental measurement integration | Validated |

The component is considered stable for the current Ambient Runtime baseline.

---

# Limitations

The component intentionally focuses on environmental measurement only.

It does not implement:

- continuous sampling;
- background acquisition;
- measurement history;
- averaging;
- filtering;
- threshold detection;
- alarm generation;
- sensor fusion.

Such responsibilities belong to higher-level runtime services.

---

# Related Components

The ENV-IV driver is commonly used together with the following Ambient Runtime components.

| Component | Relationship |
|-----------|--------------|
| `tab5_platform` | Provides the I²C bus used by the ENV-IV unit. |
| `pahub` | Selects the external I²C channel connected to the ENV-IV unit. |
| `ambient_console` | Displays the environmental measurements returned by the driver. |

---

# Related Documentation

| Document | Description |
|----------|-------------|
| `firmware/nodes/ambient-runtime-node/README.md` | Ambient Runtime architectural overview. |
| `firmware/nodes/ambient-runtime-node/components/README.md` | Runtime component architecture. |
| `firmware/nodes/ambient-runtime-node/components/pahub/README.md` | External I²C multiplexer. |
| `firmware/nodes/ambient-runtime-node/components/ambient_console/README.md` | Runtime Console. |

---

# Conclusion

The `env_iv` component provides a complete native ESP-IDF interface for the M5Stack ENV-IV environmental sensing unit.

By encapsulating the acquisition, validation, calibration and compensation procedures required by the SHT40 and BMP280 sensors, the component exposes a simple, strongly typed and reusable API to the Ambient Runtime.

This separation allows higher-level runtime modules to consume accurate environmental measurements without requiring knowledge of sensor registers, calibration data or compensation algorithms, preserving the modular architecture adopted throughout the Ambient Physical AI project.