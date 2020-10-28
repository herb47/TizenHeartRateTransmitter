#include "heartratetransmitter.h"
#include "sensor/privilege.h"
#include "sensor/listener.h"

static const char *privilege = "http://tizen.org/privilege/healthinfo";

static void request_response_callback(ppm_call_cause_e cause, ppm_request_result_e result, const char *privilege, void *user_data);

bool check_and_request_permission() {
	ppm_check_result_e result;
	int retval;

	retval = ppm_check_permission(privilege, &result);

	if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE) {
		switch (result) {
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: The application has permission to use a privilege.", __FILE__, __func__, __LINE__);

			if(is_listener_created())
			{
				if(!initialize_sensor())
				{
					dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to get the handle for the default sensor.", __FILE__, __func__, __LINE__);
					return false;
				}
				else
					dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in getting the handle for the default sensor.", __FILE__, __func__, __LINE__);

				if(!create_listener())
				{
					dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create a sensor listener.", __FILE__, __func__, __LINE__);
					return false;
				}
				else
					dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating a sensor listener.", __FILE__, __func__, __LINE__);

				if(!set_listener_attribute())
				{
					dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to set an attribute to control the behavior of a sensor listener.", __FILE__, __func__, __LINE__);
					return false;
				}
				else
					dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in setting an attribute to control the behavior of a sensor listener.", __FILE__, __func__, __LINE__);

				if(!set_listener_event_callback())
				{
					dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a sensor listener.", __FILE__, __func__, __LINE__);
					return false;
				}
				else
					dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a sensor listener.", __FILE__, __func__, __LINE__);

				if(!start_listener())
				{
					dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to starts observing the sensor events regarding a given sensor listener.", __FILE__, __func__, __LINE__);
					return false;
				}
				else
					dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in starting observing the sensor events regarding a given sensor listener.", __FILE__, __func__, __LINE__);

			}

			return true;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY:
			/* Show a message and terminate the application */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: The application doesn't have permission to use a privilege.", __FILE__, __func__, __LINE__);
			return false;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK:
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: The user has to be asked whether to grant permission to use a privilege.", __FILE__, __func__, __LINE__);

			if(!request_permission())
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to request a user's response to obtain permission for using the given privilege.", __FILE__, __func__, __LINE__);
				return false;
			}
			else
			{
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in requesting a user's response to obtain permission for using the given privilege.", __FILE__, __func__, __LINE__);
				return true;
			}
		default:
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_check_permission() output result = %d", __FILE__, __func__, __LINE__, result);
			return false;
		}
	}
	else
	{
		/* retval != PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE */
		/* Handle errors */
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_check_permission() return %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
}

bool request_permission()
{
	int retval;

	retval = ppm_request_permission(privilege, request_response_callback, NULL);

	/* Log and handle errors */
	if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE)
		return true;
	else
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_request_permission() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
}

void request_response_callback(ppm_call_cause_e cause, ppm_request_result_e result, const char *privilege, void *user_data) {
	if (cause == PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR) {
		/* Log and handle errors */
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function privilege_request_response_cb() output cause = PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR", __FILE__, __func__, __LINE__);
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Callback was called because of an error.", __FILE__, __func__, __LINE__);
	} else {
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Callback was called with a valid answer.", __FILE__, __func__, __LINE__);

		switch (result) {
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: The user granted permission to use a privilege for an indefinite period of time.", __FILE__, __func__, __LINE__);

			if(!initialize_sensor())
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to get the handle for the default sensor.", __FILE__, __func__, __LINE__);
				return;
			}
			else
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in getting the handle for the default sensor.", __FILE__, __func__, __LINE__);

			if(!create_listener())
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create a sensor listener.", __FILE__, __func__, __LINE__);
				return;
			}
			else
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating a sensor listener.", __FILE__, __func__, __LINE__);

			if(!set_listener_attribute())
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to set an attribute to control the behavior of a sensor listener.", __FILE__, __func__, __LINE__);
				return;
			}
			else
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in setting an attribute to control the behavior of a sensor listener.", __FILE__, __func__, __LINE__);

			if(!set_listener_event_callback())
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to register the callback function to be invoked when sensor events are delivered via a sensor listener.", __FILE__, __func__, __LINE__);
				return;
			}
			else
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in registering the callback function to be invoked when sensor events are delivered via a sensor listener.", __FILE__, __func__, __LINE__);

			if(!start_listener())
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to starts observing the sensor events regarding a given sensor listener.", __FILE__, __func__, __LINE__);
				return;
			}
			else
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in starting observing the sensor events regarding a given sensor listener.", __FILE__, __func__, __LINE__);

			return;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER:
			/* Show a message and terminate the application */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function privilege_request_response_cb() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: The user denied granting permission to use a privilege for an indefinite period of time.", __FILE__, __func__, __LINE__);
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE:
			/* Show a message with explanation */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function privilege_request_response_cb() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: The user denied granting permission to use a privilege once.", __FILE__, __func__, __LINE__);
			break;
		}
	}
}
