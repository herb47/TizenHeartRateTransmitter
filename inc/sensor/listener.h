#ifndef SENSOR_LISTENER_H_
#define SENSOR_LISTENER_H_

bool create_hrm_sensor_listener(sensor_h sensor);
bool set_hrm_sensor_listener_attribute();
bool set_hrm_sensor_listener_event_callback();
bool start_hrm_sensor_listener();
bool stop_hrm_sensor_listener();
bool destroy_hrm_sensor_listener();
bool check_hrm_sensor_listener_is_created();

#endif /* SENSOR_LISTENER_H_ */
