# CoreS3 Lite Grove I2C Investigation

Date: 31/05/2026

The Presence Node firmware successfully initialized I2C on GPIO2/GPIO1, matching the CoreS3 Grove Port A mapping.

However, probing the Unit Mini ToF-90 at address 0x29 resulted in ESP_ERR_TIMEOUT.

This suggests that the issue is not the ESP-IDF build system or application runtime, but likely the external Grove bus power/pull-up enable path on CoreS3/CoreS3 Lite.

Further investigation is required regarding:
- Grove external power enable;
- AW9523B / BUS_OUT_EN behavior;
- CoreS3 Lite ESP-IDF initialization sequence;
- M5Stack power initialization equivalent.

Decision:
Do not block Presence Node development on this issue.
Validate the ToF sensor using another compatible M5Stack device, then return to CoreS3 Lite external Grove initialization later.