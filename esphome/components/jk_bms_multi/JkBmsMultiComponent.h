// JkBmsMultiComponent.h
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace jk_bms_multi {

class JkBmsMultiComponent : public esphome::Component, public uart::UARTDevice {
 public:
  void set_soc_sensors(const std::vector<sensor::Sensor*>& sensors) {
    this->soc_sensors_ = sensors;
  }
  void set_voltage_sensors(const std::vector<sensor::Sensor*>& sensors) {
    this->voltage_sensors_ = sensors;
  }

 protected:
  std::vector<sensor::Sensor*> soc_sensors_;
  std::vector<sensor::Sensor*> voltage_sensors_;
};

} // namespace jk_bms_multi
} // namespace esphome
