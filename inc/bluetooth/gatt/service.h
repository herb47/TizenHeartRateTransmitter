#ifndef BLUETOOTH_GATT_SERVICE_H_
#define BLUETOOTH_GATT_SERVICE_H_

bool create_service();
bool add_characteristic_to_service();
bool destroy_service();
bool get_service_handle(bt_gatt_h *handle);

#endif /* BLUETOOTH_GATT_SERVICE_H_ */
