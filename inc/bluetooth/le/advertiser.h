#ifndef BLUETOOTH_ADVERTISER_H_
#define BLUETOOTH_ADVERTISER_H_

bool create_bluetooth_le_advertiser();
bool set_bluetooth_le_advertising_device_name();
bool set_bluetooth_le_advertising_gatt_service_uuid();
bool start_bluetooth_le_advertising();
bool stop_bluetooth_le_advertising();
bool set_bluetooth_le_advertising_mode();

#endif /* BLUETOOTH_ADVERTISER_H_ */
