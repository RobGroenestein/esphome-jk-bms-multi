#include "jkbmsmulti.h"
#include "esphome/core/log.h"
#include <vector>

namespace esphome {
namespace jkbmsmulti {

static const char *const TAG = "jkbmsmulti";

void JkBmsMultiComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up JK BMS Multi...");
}

void JkBmsMultiComponent::loop() {
  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    this->rx_buffer_.push_back(byte);
    
    // De oude check, die vaak misgaat bij verkeerde baudrate
    // if (this->rx_buffer_.back() == 0x57) {
    //   this->parse_frame();
    //   this->rx_buffer_.clear();
    // }

    // Nieuwe, robuustere check
    // Als de buffer een start byte heeft, maar te groot wordt, wissen we hem.
    if (this->rx_buffer_.size() > 384) {
        ESP_LOGW(TAG, "UART buffer overflow, clearing.");
        this->rx_buffer_.clear();
    }
    
    // Parsen zodra er een startbyte wordt gevonden en de buffer lang genoeg is
    // Let op: deze logica is gebaseerd op de 106-byte lengte.
    // Dit is een simpele parsing methode, een meer geavanceerde zou een state machine gebruiken.
    if (this->rx_buffer_.size() >= 106 && this->rx_buffer_[0] == 0x4E) {
        this->parse_frame();
        this->rx_buffer_.clear();
    }
  }
}

void JkBmsMultiComponent::parse_frame() {
  ESP_LOGD(TAG, "Parsing received frame of size %d", this->rx_buffer_.size());

  // Check for the correct start byte
  if (this->rx_buffer_.empty() || this->rx_buffer_.front() != 0x4E) {
    ESP_LOGW(TAG, "Received frame with invalid start byte: 0x%02X", this->rx_buffer_.front());
    return;
  }

  // Check for the correct length (from your old code, the frame length is 106 bytes)
  // This is a common frame length for JK-BMS data frames.
  if (this->rx_buffer_.size() != 106) {
    ESP_LOGW(TAG, "Received frame with unexpected length: %d", this->rx_buffer_.size());
    return;
  }
  
  // Extract data based on the protocol
  uint8_t bms_id = this->rx_buffer_[1];  // Assuming the second byte is the BMS ID
  
  if (bms_id >= this->voltage_sensors_.size() || bms_id >= this->soc_sensors_.size()) {
    ESP_LOGW(TAG, "Received data for unknown BMS ID %d", bms_id);
    return;
  }

  // Calculate voltage from bytes 7-8 and SoC from byte 19
  // The old code assumed these positions. Double-check your specific protocol for accuracy.
  uint16_t voltage_raw = (this->rx_buffer_[7] << 8) | this->rx_buffer_[8];
  float voltage = voltage_raw / 1000.0f; // Convert mV to V
  uint8_t soc = this->rx_buffer_[19];

  // Publish the states
  if (this->voltage_sensors_[bms_id] != nullptr) {
    this->voltage_sensors_[bms_id]->publish_state(voltage);
    ESP_LOGD(TAG, "BMS %d - Voltage: %.2f V", bms_id, voltage);
  } else {
    ESP_LOGW(TAG, "Voltage sensor for BMS ID %d is not configured.", bms_id);
  }

  if (this->soc_sensors_[bms_id] != nullptr) {
    this->soc_sensors_[bms_id]->publish_state(soc);
    ESP_LOGD(TAG, "BMS %d - SoC: %d %%", bms_id, soc);
  } else {
    ESP_LOGW(TAG, "SoC sensor for BMS ID %d is not configured.", bms_id);
  }
}

} // namespace jkbmsmulti
} // namespace esphome
