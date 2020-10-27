#include "heartratetransmitter.h"
#include "bluetooth/gatt/server.h"
#include "bluetooth/gatt/service.h"
#include "bluetooth/gatt/characteristic.h"
#include "bluetooth/gatt/descriptor.h"
#include "bluetooth/advertiser.h"
#include "sensor/privilege.h"
#include "sensor/listener.h"

typedef struct appdata {
	Evas_Object *win;
	Evas_Object *conform;
	Evas_Object *label;
} appdata_s;

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

	/* Label */
	/* Create an actual view of the base gui.
	   Modify this part to change the view. */
	ad->label = elm_label_add(ad->conform);
	elm_object_text_set(ad->label, "<align=center>Hello Tizen</align>");
	evas_object_size_hint_weight_set(ad->label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(ad->conform, ad->label);

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

	if (!create_server())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create the GATT server's handle.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating the GATT server's handle.", __FILE__, __func__, __LINE__);

	if (!create_service())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create the GATT service.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating the GATT service.", __FILE__, __func__, __LINE__);

	if (!create_characteristic())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to creates the GATT characteristic.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating the GATT characteristic.", __FILE__, __func__, __LINE__);

	if (!create_descriptor())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create the GATT characteristic descriptor.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating the GATT characteristic descriptor.", __FILE__, __func__, __LINE__);

	if (!add_descriptor_to_characteristic())
		{
			dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to add a descriptor to a specified characteristic.", __FILE__, __func__, __LINE__);
			return false;
		}
		else
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in adding a descriptor to a specified characteristic.", __FILE__, __func__, __LINE__);

	if (!add_characteristic_to_service())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to add a characteristic to a specified service.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in adding a characteristic to a specified service.", __FILE__, __func__, __LINE__);

	if (!register_service_to_server())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to register a specified service to the specified GATT server that the local device is hosting.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in registering a specified service to the specified GATT server that the local device is hosting.", __FILE__, __func__, __LINE__);

	retval = bt_gatt_server_start();

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_server_start() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to register the application along with the GATT services of the application it is hosting.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in registering the application along with the GATT services of the application it is hosting.", __FILE__, __func__, __LINE__);

	if(!create_advertiser())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to create advertiser to advertise device's existence or respond to LE scanning request.", __FILE__, __func__, __LINE__);
		return false;
	}
	else {
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in creating advertiser to advertise device's existence or respond to LE scanning request.", __FILE__, __func__, __LINE__);
	}

	if(!set_advertising_appearance())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to set the external appearance of this device to advertise or scan response data.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in setting the external appearance of this device to advertise or scan response data.", __FILE__, __func__, __LINE__);

	if(!start_advertising())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to start advertising with passed advertiser and advertising parameters.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in starting advertising with passed advertiser and advertising parameters.", __FILE__, __func__, __LINE__);

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
	if (!check_and_request_permission())
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to check if an application, which calls this function, has permission to use the given privilege.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in checking if an application, which calls this function, has permission to use the given privilege.", __FILE__, __func__, __LINE__);
}

static void
app_terminate(void *data)
{
	/* Release all resources. */
	int retval;

	if(!destroy_advertiser())
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to destroy advertiser.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in destroying advertiser.", __FILE__, __func__, __LINE__);

	if(!destroy_listener())
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to release all the resources allocated for a listener.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in releasing all the resources allocated for a listener.", __FILE__, __func__, __LINE__);

	if(!destroy_descriptor())
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to destroy the GATT handle of descriptor.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in destroying the GATT handle of descriptor.", __FILE__, __func__, __LINE__);

	if(!destroy_characteristic())
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to destroy the GATT handle of characteristic.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in destroying the GATT handle of characteristic.", __FILE__, __func__, __LINE__);

	if(!destroy_service())
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to destroy the GATT handle of service.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in destroying the GATT handle of service.", __FILE__, __func__, __LINE__);

	if(!destroy_server())
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to destroy the GATT server's handle.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in destroying the GATT server's handle.", __FILE__, __func__, __LINE__);

	retval = bt_gatt_server_deinitialize();

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_server_deinitialize() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to deinitializ the GATT server.", __FILE__, __func__, __LINE__);
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in deinitializing the GATT server.", __FILE__, __func__, __LINE__);

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
