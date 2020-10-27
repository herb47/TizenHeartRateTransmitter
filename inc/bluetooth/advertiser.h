#ifndef BLUETOOTH_ADVERTISER_H_
#define BLUETOOTH_ADVERTISER_H_

bool create_advertiser();
bool set_advertising_device_name();
bool set_advertising_appearance();
bool set_advertising_service_uuid();
bool start_advertising();
bool clear_advertising_data();
bool destroy_advertiser();

#endif /* BLUETOOTH_ADVERTISER_H_ */
