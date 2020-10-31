#ifndef BLUETOOTH_GATT_CHARACTERISTIC_H_
#define BLUETOOTH_GATT_CHARACTERISTIC_H_

bool create_gatt_characteristic();
bool add_gatt_descriptor_to_gatt_characteristic();
bool set_gatt_characteristic_value(int value);
bool notify_gatt_characteristic_value_changed();
bool get_gatt_characteristic_handle(bt_gatt_h *handle);

#endif /* BLUETOOTH_GATT_CHARACTERISTIC_H_ */
