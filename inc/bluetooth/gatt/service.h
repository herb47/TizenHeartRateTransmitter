#ifndef BLUETOOTH_GATT_SERVICE_H_
#define BLUETOOTH_GATT_SERVICE_H_

bool create_gatt_service();
bool add_gatt_characteristic_to_gatt_service();
bool get_gatt_service_handle(bt_gatt_h *handle);
bool destroy_gatt_service();
bool set_bluetooth_le_advertising_gatt_service_uuid(bt_advertiser_h bluetooth_le_advertiser_handle);

#endif /* BLUETOOTH_GATT_SERVICE_H_ */
