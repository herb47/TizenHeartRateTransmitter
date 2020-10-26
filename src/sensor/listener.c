#include "heartratetransmitter.h"
#include "sensor/listener.h"

static sensor_type_e type = SENSOR_HRM;
static sensor_h sensor = 0;
static sensor_listener_h listener = 0;

static void event_callback(sensor_h sensor, sensor_event_s events[], void *user_data);

bool initialize_sensor()
{
	int retval;
	bool supported = false;

	retval = sensor_is_supported(type, &supported);

	if(retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_is_supported() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to checks whether a given sensor type is supported in the current device.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in checking whether a given sensor type is supported in the current device.", __FILE__, __func__, __LINE__);

	if(!supported) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_is_supported() output supported = %d", __FILE__, __func__, __LINE__, supported);
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Heart rate monitor sensor is not supported.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Heart rate monitor sensor is supported.", __FILE__, __func__, __LINE__);

	retval = sensor_get_default_sensor(type, &sensor);

	if(retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_get_default_sensor() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));

		return false;
	} else
		return true;
}

bool create_listener()
{
	int retval;

	retval = sensor_create_listener(sensor, &listener);

	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_create_listener() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool set_listener_attribute()
{
	int retval;

	/*
	* By default, the listener automatically stops listening for the sensor data,
	* if the display is switched off or the device goes to the power-save mode. You can override such behavior:
	*/

	retval = sensor_listener_set_attribute_int(listener, SENSOR_ATTRIBUTE_PAUSE_POLICY, SENSOR_PAUSE_NONE);

	/*
	* The above function makes the listener listen for the sensor data regardless of the display state and the power-save mode.
	* However, it does not prevent the device from going to the sleep mode.
	* To listen for the sensor data, the device must be awake anyway.
	*/

	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_listener_set_attribute_int() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool set_listener_event_callback()
{
	/* Register callback */
	unsigned int interval_ms = 10000;
	int retval;

	retval = sensor_listener_set_event_cb(listener, interval_ms, event_callback, NULL);

	if(retval != SENSOR_ERROR_NONE) {
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_listener_set_event_cb() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

void event_callback(sensor_h sensor, sensor_event_s events[], void *user_data) {
	//unsigned long long timestamp = events[0].timestamp;
	int value = (int)events[0].values[0];
	//dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Function sensor_events_callback() output timestamp = %ld", __FILE__, __func__, __LINE__, timestamp);
	dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Function sensor_events_callback() output value = %d", __FILE__, __func__, __LINE__, value);
}

bool start_listener()
{
	int retval;

	retval = sensor_listener_start(listener);

	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_listener_start() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool destroy_listener()
{
	int retval;

	retval = sensor_destroy_listener(listener);

	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_destroy_listener() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}
