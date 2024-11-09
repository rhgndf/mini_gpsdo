
void tud_cdc_rx_cb(uint8_t itf) {
  if (itf == 0) {
    uint8_t buffer[64];
    while (tud_cdc_n_available(itf)) {
      uint32_t len = tud_cdc_n_read(itf, buffer, 64);
      GPSSend(buffer, len);
    }
  }
}

void tud_cdc_line_state_cb(uint8_t instance, bool dtr, bool rts) {
  (void)dtr;
  (void)rts;
  if (instance == 0) {
      cdc_line_coding_t coding;
      tud_cdc_get_line_coding(&coding);
      //GPSSetBaudRate(coding.bit_rate);
  }
}