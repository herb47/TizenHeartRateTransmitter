#ifndef SENSOR_PRIVILEGE_H_
#define SENSOR_PRIVILEGE_H_

bool check_and_request_permission();
bool request_permission();
void request_response_callback(ppm_call_cause_e cause, ppm_request_result_e result, const char *privilege, void *user_data);

#endif /* SENSOR_PRIVILEGE_H_ */
