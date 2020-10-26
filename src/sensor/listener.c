#include "heartratetransmitter.h"
#include "sensor/listener.h"

static sensor_type_e type = SENSOR_HRM;
static sensor_h sensor;

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
