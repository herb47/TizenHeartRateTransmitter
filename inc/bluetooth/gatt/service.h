#ifndef BLUETOOTH_GATT_SERVICE_H_
#define BLUETOOTH_GATT_SERVICE_H_

bool create_gatt_service();
bool get_service_handle(bt_gatt_h *handle);
bool destroy_gatt_service();

#endif /* BLUETOOTH_GATT_SERVICE_H_ */
