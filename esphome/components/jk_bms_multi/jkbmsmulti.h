#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include <vector>

namespace esphome {
namespace jkbmsmulti {

class JkBmsMultiComponent : public Component, public uart::UARTDevice {
 public:
  void set_soc_sensors(const std::vector<sensor::Sensor*>& sensors) { this->soc_sensors_ = sensors; }
  void set_voltage_sensors(const std::vector<sensor::Sensor*>& sensors) { this->voltage_sensors_ = sensors; }

  void setup() override;
  void loop() override;
  float get_setup_priority() const override { return esphome::setup_priority::DATA; }

 protected:
  std::vector<sensor::Sensor*> soc_sensors_;
  std::vector<sensor::Sensor*> voltage_sensors_;

  std::vector<uint8_t> rx_buffer_;
  void parse_frame();
};

} // namespace jkbmsmulti
} // namespace esphome
