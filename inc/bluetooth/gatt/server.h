#ifndef BLUETOOTH_GATT_SERVER_H_
#define BLUETOOTH_GATT_SERVER_H_

bool create_server();
bool register_service_to_server();
bool destroy_server();
bool set_connection_state_changed_callback();

#endif /* BLUETOOTH_GATT_SERVER_H_ */
