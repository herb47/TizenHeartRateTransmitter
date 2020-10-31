#ifndef BLUETOOTH_GATT_SERVER_H_
#define BLUETOOTH_GATT_SERVER_H_

bool set_gett_server_connection_state_changed_callback();
bool create_gatt_server();
bool register_gatt_service_to_gatt_server();
bool destroy_gatt_server();
bool start_gatt_server();

#endif /* BLUETOOTH_GATT_SERVER_H_ */
