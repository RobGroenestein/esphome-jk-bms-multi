#include "esphome.h"

class JkBmsMulti : public Component, public UARTDevice {
 public:
  Sensor *soc_sensors[3];
  Sensor *voltage_sensors[3];

  JkBmsMulti(UARTComponent *parent) : UARTDevice(parent) {
    for (int i = 0; i < 3; i++) {
      soc_sensors[i] = nullptr;
      voltage_sensors[i] = nullptr;
    }
  }

  void loop() override {
    while (available()) {
      uint8_t byte = read();
      buffer_.push_back(byte);

      if (buffer_.size() >= 3 &&
          !(buffer_[0] == 0x55 && buffer_[1] == 0xAA && buffer_[2] == 0xEB)) {
        buffer_.erase(buffer_.begin());
        continue;
      }

      if (buffer_.size() > 120) {
        parse_frame();
        buffer_.clear();
      }
    }
  }

 private:
  std::vector<uint8_t> buffer_;

  void parse_frame() {
    if (buffer_.size() < 30) return;

    uint8_t bms_id = buffer_[3];  // 0x00, 0x01, 0x02
    if (bms_id > 2) return;

    uint16_t raw_volt = (buffer_[0x10] << 8) | buffer_[0x11];
    float voltage = raw_volt / 100.0f;

    uint16_t raw_soc = (buffer_[0x12] << 8) | buffer_[0x13];
    float soc = raw_soc / 100.0f;

    if (voltage_sensors[bms_id]) voltage_sensors[bms_id]->publish_state(voltage);
    if (soc_sensors[bms_id]) soc_sensors[bms_id]->publish_state(soc);
  }
};
