#include "heartratetransmitter.h"
#include <privacy_privilege_manager.h>

#include "bluetooth/gatt/server.h"
#include "bluetooth/gatt/service.h"
#include "bluetooth/gatt/characteristic.h"
#include "bluetooth/gatt/descriptor.h"
#include "bluetooth/le/advertiser.h"

#include "sensor/listener.h"

const char *sensor_privilege = "http://tizen.org/privilege/healthinfo";

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *button;
} appdata_s;

sensor_type_e sensor_type = SENSOR_HRM;
sensor_h sensor_handle = 0;

bool check_hrm_sensor_is_supported();
bool initialize_hrm_sensor();

bool check_and_request_sensor_permission();
bool request_sensor_permission();
void request_sensor_permission_response_callback(ppm_call_cause_e cause, ppm_request_result_e result, const char *privilege, void *user_data);

static void
win_delete_request_cb(void *data, Evas_Object *obj, void *event_info)
{
	ui_app_exit();
}

static void
win_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	appdata_s *ad = data;
	/* Let window go to hide state. */
	elm_win_lower(ad->win);
}

/* Callback for the "clicked" signal */
/* Called when the button is clicked by the user */
void button_clicked_callback(void *data, Evas_Object *obj, void *event_info)
{
	dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Shut down button is clicked.", __FILE__, __func__, __LINE__);
	ui_app_exit();
}

static void
create_base_gui(appdata_s *ad)
{
	/* Window */
	/* Create and initialize elm_win.
	   elm_win is mandatory to manipulate window. */
	ad->win = elm_win_util_standard_add(PACKAGE, PACKAGE);
	elm_win_autodel_set(ad->win, EINA_TRUE);

	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win, (const int *)(&rots), 4);
	}

	evas_object_smart_callback_add(ad->win, "delete,request", win_delete_request_cb, NULL);
	eext_object_event_callback_add(ad->win, EEXT_CALLBACK_BACK, win_back_cb, ad);

	/* Conformant */
	/* Create and initialize elm_conformant.
	   elm_conformant is mandatory for base gui to have proper size
	   when indicator or virtual keypad is visible. */
	ad->conform = elm_conformant_add(ad->win);
	elm_win_indicator_mode_set(ad->win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_size_hint_weight_set(ad->conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->win, ad->conform);
	evas_object_show(ad->conform);

	ad->button = elm_button_add(ad->conform);
	elm_object_text_set(ad->button, "Shut down the transmitter");
	elm_object_content_set(ad->conform, ad->button);
	evas_object_smart_callback_add(ad->button, "clicked", button_clicked_callback, NULL);

	/* Show window after base gui is set up */
	evas_object_show(ad->win);
}

static bool
app_create(void *data)
{
	/* Hook to take necessary actions before main event loop starts
		Initialize UI resources and application's data
		If this function returns true, the main loop of application starts
		If this function returns false, the application is terminated */
	int retval;
	appdata_s *ad = data;

	create_base_gui(ad);

	if(!check_hrm_sensor_is_supported())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: A HRM sensor is not supported.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: A HRM sensor is supported.", __FILE__, __func__, __LINE__);

	retval = bt_initialize();

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_initialize() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to initialize the Bluetooth API.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in initializing the Bluetooth API.", __FILE__, __func__, __LINE__);

	retval = bt_gatt_server_initialize();

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_server_initialize() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to initialize the GATT server.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in initializing the GATT server.", __FILE__, __func__, __LINE__);

	if(!create_gatt_descriptor())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create the GATT characteristic descriptor.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating the GATT characteristic descriptor.", __FILE__, __func__, __LINE__);

	if(!create_gatt_characteristic())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create the GATT characteristic.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating the GATT characteristic.", __FILE__, __func__, __LINE__);

	if(!create_gatt_service())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create the GATT service.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating the GATT service.", __FILE__, __func__, __LINE__);

	if(!create_gatt_server())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create the GATT server's handle.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating the GATT server's handle.", __FILE__, __func__, __LINE__);

	if(!start_gatt_server())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to register the application along with the GATT services of the application it is hosting.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in registering the application along with the GATT services of the application it is hosting.", __FILE__, __func__, __LINE__);

	if(!create_bluetooth_le_advertiser())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create advertiser to advertise device's existence.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating advertiser to advertise device's existence.", __FILE__, __func__, __LINE__);

	if(!start_bluetooth_le_advertising())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to start advertising with passed advertiser and advertising parameters.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in starting advertising with passed advertiser and advertising parameters.", __FILE__, __func__, __LINE__);

//	if(!)
//	{
//		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to", __FILE__, __func__, __LINE__);
//		return false;
//	}
//	else
//		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in", __FILE__, __func__, __LINE__);

	return true;
}

static void
app_control(app_control_h app_control, void *data)
{
	/* Handle the launch request. */
}

static void
app_pause(void *data)
{
	/* Take necessary actions when application becomes invisible. */
}

static void
app_resume(void *data)
{
	/* Take necessary actions when application becomes visible. */
	if (!check_and_request_sensor_permission())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to check if an application has permission to use the sensor privilege.", __FILE__, __func__, __LINE__);
		ui_app_exit();
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in checking if an application has permission to use the sensor privilege.", __FILE__, __func__, __LINE__);
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
	int retval;

	if(check_hrm_sensor_listener_is_created())
	{
		if(!destroy_hrm_sensor_listener())
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to release all the resources allocated for a HRM sensor listener.", __FILE__, __func__, __LINE__);
		else
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in releasing all the resources allocated for a HRM sensor listener.", __FILE__, __func__, __LINE__);
	}

	if(!destroy_gatt_service())
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to destroy the GATT handle of service.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in destroying the GATT handle of service.", __FILE__, __func__, __LINE__);

	if(!destroy_gatt_server())
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to destroy the GATT server's handle.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in destroying the GATT server's handle.", __FILE__, __func__, __LINE__);

	retval = bt_deinitialize();

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_deinitialize() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to release all resources of the Bluetooth API.", __FILE__, __func__, __LINE__);
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in releasing all resources of the Bluetooth API.", __FILE__, __func__, __LINE__);
}

static void
ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LANGUAGE_CHANGED*/
	char *locale = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &locale);
	elm_language_set(locale);
	free(locale);
	return;
}

static void
ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_DEVICE_ORIENTATION_CHANGED*/
	return;
}

static void
ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_REGION_FORMAT_CHANGED*/
}

static void
ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_BATTERY*/
}

static void
ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	/*APP_EVENT_LOW_MEMORY*/
}

int
main(int argc, char *argv[])
{
	appdata_s ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		dlog_print(DLOG_ERROR, LOG_TAG, "app_main() is failed. err = %d", ret);
	}

	return ret;
}

bool check_hrm_sensor_is_supported()
{
	int retval;
	bool supported = false;

	retval = sensor_is_supported(sensor_type, &supported);

	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_is_supported() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to checks whether a HRM sensor is supported in the current device.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in checking whether a HRM sensor is supported in the current device.", __FILE__, __func__, __LINE__);

	if(!supported)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_is_supported() output supported = %d", __FILE__, __func__, __LINE__, supported);
		return false;
	}
	else
		return true;
}

bool initialize_hrm_sensor()
{
	int retval;

	retval = sensor_get_default_sensor(sensor_type, &sensor_handle);

	if(retval != SENSOR_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function sensor_get_default_sensor() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool check_and_request_sensor_permission() {
	int retval;
	ppm_check_result_e result;

	retval = ppm_check_permission(sensor_privilege, &result);

	if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE)
	{
		switch (result)
		{
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ALLOW:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: The application has permission to use a sensor privilege.", __FILE__, __func__, __LINE__);

			if(!check_hrm_sensor_listener_is_created())
			{
				if(!initialize_hrm_sensor())
				{
					dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to get the handle for the default sensor of a HRM sensor.", __FILE__, __func__, __LINE__);
					ui_app_exit();
				}
				else
					dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in getting the handle for the default sensor of a HRM sensor.", __FILE__, __func__, __LINE__);

				if(!create_hrm_sensor_listener(sensor_handle))
				{
					dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create a HRM sensor listener.", __FILE__, __func__, __LINE__);
					return false;
				}
				else
					dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating a HRM sensor listener.", __FILE__, __func__, __LINE__);

//				if(!start_hrm_sensor_listener())
//				{
//					dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to start observing the sensor events regarding a HRM sensor listener.", __FILE__, __func__, __LINE__);
//					return false;
//				}
//				else
//				{
//					dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in starting observing the sensor events regarding a HRM sensor listener.", __FILE__, __func__, __LINE__);
//					return true;
//				}

				return true;
			}
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY:
			/* Show a message and terminate the application */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_check_permission() output result = PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_DENY", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: The application doesn't have permission to use a sensor privilege.", __FILE__, __func__, __LINE__);
			return false;
		case PRIVACY_PRIVILEGE_MANAGER_CHECK_RESULT_ASK:
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: The user has to be asked whether to grant permission to use a sensor privilege.", __FILE__, __func__, __LINE__);

			if(!request_sensor_permission())
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to request a user's response to obtain permission for using the sensor privilege.", __FILE__, __func__, __LINE__);
				return false;
			}
			else
			{
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in requesting a user's response to obtain permission for using the sensor privilege.", __FILE__, __func__, __LINE__);
				return true;
			}
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

bool request_sensor_permission()
{
	int retval;

	retval = ppm_request_permission(sensor_privilege, request_sensor_permission_response_callback, NULL);

	/* Log and handle errors */
	if (retval == PRIVACY_PRIVILEGE_MANAGER_ERROR_NONE)
		return true;
	else
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function ppm_request_permission() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
}

void request_sensor_permission_response_callback(ppm_call_cause_e cause, ppm_request_result_e result, const char *privilege, void *user_data) {
	if (cause == PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR)
	{
		/* Log and handle errors */
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function request_sensor_permission_response_callback() output cause = PRIVACY_PRIVILEGE_MANAGER_CALL_CAUSE_ERROR", __FILE__, __func__, __LINE__);
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Function request_sensor_permission_response_callback() was called because of an error.", __FILE__, __func__, __LINE__);
	}
	else
	{
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Function request_sensor_permission_response_callback() was called with a valid answer.", __FILE__, __func__, __LINE__);

		switch (result) {
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_ALLOW_FOREVER:
			/* Update UI and start accessing protected functionality */
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: The user granted permission to use a sensor privilege for an indefinite period of time.", __FILE__, __func__, __LINE__);

			if(!initialize_hrm_sensor())
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to get the handle for the default sensor of a HRM sensor.", __FILE__, __func__, __LINE__);
				ui_app_exit();
			}
			else
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in getting the handle for the default sensor of a HRM sensor.", __FILE__, __func__, __LINE__);

			if(!create_hrm_sensor_listener(sensor_handle))
			{
				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create a HRM sensor listener.", __FILE__, __func__, __LINE__);
				ui_app_exit();
			}
			else
				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating a HRM sensor listener.", __FILE__, __func__, __LINE__);

//			if(!start_hrm_sensor_listener())
//			{
//				dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to start observing the sensor events regarding a HRM sensor listener.", __FILE__, __func__, __LINE__);
//				ui_app_exit();
//			}
//			else
//				dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in starting observing the sensor events regarding a HRM sensor listener.", __FILE__, __func__, __LINE__);
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER:
			/* Show a message and terminate the application */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_FOREVER", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: The user denied granting permission to use a sensor privilege for an indefinite period of time.", __FILE__, __func__, __LINE__);
			ui_app_exit();
			break;
		case PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE:
			/* Show a message with explanation */
			dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function request_sensor_permission_response_callback() output result = PRIVACY_PRIVILEGE_MANAGER_REQUEST_RESULT_DENY_ONCE", __FILE__, __func__, __LINE__);
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: The user denied granting permission to use a sensor privilege once.", __FILE__, __func__, __LINE__);
			ui_app_exit();
			break;
		}
	}
}
