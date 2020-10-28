#ifndef SENSOR_LISTENER_H_
#define SENSOR_LISTENER_H_

bool initialize_sensor();
bool create_listener();
bool set_listener_attribute();
bool set_listener_event_callback();
bool start_listener();
bool destroy_listener();
bool is_listener_created();

#endif /* SENSOR_LISTENER_H_ */
