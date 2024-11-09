
#include <stdint.h>

#include "tusb.h"
#include "usb_descriptors.h"

#include "ms5351.h"
#include "settings.h"

#define REQUEST_5351_LOAD_SETTINGS 0x10
#define REQUEST_5351_I2C_READ 0x11
#define REQUEST_5351_I2C_WRITE 0x12

#define REQUEST_COMMIT_SETTINGS 0x20
#define REQUEST_RESET_SETTINGS 0x21
#define REQUEST_READ_SETTINGS 0x22
#define REQUEST_WRITE_SETTINGS 0x23

#define REQUEST_READ_SETTINGS_U32 0x30
#define REQUEST_WRITE_SETTINGS_U32 0x31
#define REQUEST_READ_SETTINGS_U16 0x32
#define REQUEST_WRITE_SETTINGS_U16 0x33
#define REQUEST_READ_SETTINGS_U8 0x34
#define REQUEST_WRITE_SETTINGS_U8 0x35
#define REQUEST_READ_SETTINGS_U64 0x36
#define REQUEST_WRITE_SETTINGS_U64 0x37

#define URL  "example.tinyusb.org/webusb-serial/index.html"

const tusb_desc_webusb_url_t desc_url = {
  .bLength         = 3 + sizeof(URL) - 1,
  .bDescriptorType = 3, // WEBUSB URL type
  .bScheme         = 1, // 0: http, 1: https
  .url             = URL
};

union {
    uint32_t u32;
    uint16_t u16;
    uint8_t u8;
    uint8_t buf[64];
} buf = {0};
//--------------------------------------------------------------------+
// WebUSB use vendor class
//--------------------------------------------------------------------+

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request) {
  // nothing to with DATA & ACK stage
  //printf("Control transfer: %d %d %d %d %d %d\n", stage, request->  , request->bRequest, request->wValue, request->wIndex, request->wLength);
  if (stage == CONTROL_STAGE_SETUP) {
    switch (request->bmRequestType_bit.type) {
        case TUSB_REQ_TYPE_VENDOR:
        switch (request->bRequest) {
            case VENDOR_REQUEST_WEBUSB:
            // match vendor request in BOS descriptor
            // Get landing page url
            return tud_control_xfer(rhport, request, (void*)(uintptr_t)&desc_url, desc_url.bLength);

            case VENDOR_REQUEST_MICROSOFT:
            if (request->wIndex == 7) {
                // Get Microsoft OS 2.0 compatible descriptor
                uint16_t total_len;
                memcpy(&total_len, desc_ms_os_20 + 8, 2);

                return tud_control_xfer(rhport, request, (void*)(uintptr_t)desc_ms_os_20, total_len);
            } else {
                return false;
            }

            default: break;
        }
        break;

        case TUSB_REQ_TYPE_CLASS:
        switch (request->bRequest) {
            // response with status OK
            case REQUEST_5351_LOAD_SETTINGS:
                MS5351LoadSettings();
                return tud_control_status(rhport, request);
            case REQUEST_5351_I2C_READ:
                if (request->wLength > 64) {
                    return false;
                }
                MS5351I2CRead(request->wValue, buf.buf, request->wLength);
                return tud_control_xfer(rhport, request, buf.buf, request->wLength);
            case REQUEST_COMMIT_SETTINGS:
                SettingsCommit();
                return tud_control_status(rhport, request);
            case REQUEST_RESET_SETTINGS:
                SettingsReset();
                return tud_control_status(rhport, request);
            case REQUEST_READ_SETTINGS:
                uint32_t offset = request->wValue;
                uint32_t len = request->wLength > 64 ? 64 : request->wLength;
                len = SettingsGetRaw(offset, buf.buf, len);
                return tud_control_xfer(rhport, request, buf.buf, len);
            case REQUEST_READ_SETTINGS_U32:
                uint32_t value = SettingsGetU32(request->wValue);
                return tud_control_xfer(rhport, request, &value, sizeof(value));
            case REQUEST_READ_SETTINGS_U16:
                uint16_t value16 = SettingsGetU16(request->wValue);
                return tud_control_xfer(rhport, request, &value16, sizeof(value16));
            case REQUEST_READ_SETTINGS_U8:
                uint8_t value8 = SettingsGetU8(request->wValue);
                return tud_control_xfer(rhport, request, &value8, sizeof(value8));
            case REQUEST_WRITE_SETTINGS_U32:
            case REQUEST_WRITE_SETTINGS_U16:
            case REQUEST_WRITE_SETTINGS_U8:
            case REQUEST_WRITE_SETTINGS_U64:
                return tud_control_xfer(rhport, request, buf.buf, request->wLength);

            default: break;
        }
        break;

        default: break;
    }
    return false;
  } if (stage == CONTROL_STAGE_ACK) {
    switch (request->bmRequestType_bit.type) {
      case TUSB_REQ_TYPE_CLASS:
        switch (request->bRequest) {
            case REQUEST_WRITE_SETTINGS_U32:
                SettingsSetU32(request->wValue, buf.u32);
                break;
            case REQUEST_WRITE_SETTINGS_U16:
                SettingsSetU16(request->wValue, buf.u16);
                break;
            case REQUEST_WRITE_SETTINGS_U8:
                SettingsSetU8(request->wValue, buf.u8);
                break;
            default: break;
        }
      default: break;
    }
  }

  // stall unknown request
  return true;
}

void tud_vendor_rx_cb(uint8_t itf, uint8_t const* buffer, uint16_t bufsize) {
  tud_vendor_read_flush();
}