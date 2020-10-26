#ifndef BLUETOOTH_GATT_CHARACTERISTIC_H_
#define BLUETOOTH_GATT_CHARACTERISTIC_H_

bool create_characteristic();
bool add_descriptor_to_characteristic();
bool set_characteristic_value(int value);
bool notify_characteristic_value_changed();
bool destroy_characteristic();
bool get_characteristic_handle(bt_gatt_h *handle);

#endif /* BLUETOOTH_GATT_CHARACTERISTIC_H_ */
