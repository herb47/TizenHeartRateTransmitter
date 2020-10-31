#include "heartratetransmitter.h"
#include "bluetooth/gatt/characteristic.h"
#include "bluetooth/gatt/descriptor.h"

bt_gatt_h gatt_characteristic_handle = 0;

void gatt_characteristic_notification_sent_callback(int result, const char *remote_address, bt_gatt_server_h server, bt_gatt_h characteristic, bool completed, void *user_data);

bool create_gatt_characteristic()
{
	int retval;
	const char *GATT_CHARACTERISTIC_UUID = "00002A37-0000-1000-8000-00805F9B34FB";
	const char CHARACTERISTIC_VALUE[] = {0, 0, 0, 0, 0};

	retval = bt_gatt_characteristic_create(GATT_CHARACTERISTIC_UUID,
			BT_GATT_PERMISSION_READ|BT_GATT_PERMISSION_WRITE, BT_GATT_PROPERTY_NOTIFY,
			CHARACTERISTIC_VALUE, sizeof(CHARACTERISTIC_VALUE),
			&gatt_characteristic_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_characteristic_create() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	if(!add_gatt_descriptor_to_gatt_characteristic())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to add a descriptor to a specified characteristic.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
	{
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in adding a descriptor to a specified characteristic.", __FILE__, __func__, __LINE__);
		return true;
	}
}

bool add_gatt_descriptor_to_gatt_characteristic()
{
	int retval;
	bt_gatt_h gatt_descriptor_handle = 0;

	if (!get_gatt_descriptor_handle(&gatt_descriptor_handle))
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to get the GATT descriptor's handle.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in getting the GATT descriptor's handle.", __FILE__, __func__, __LINE__);

	retval =  bt_gatt_characteristic_add_descriptor(gatt_characteristic_handle, gatt_descriptor_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_characteristic_add_descriptor() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool set_gatt_characteristic_value(int value)
{
	int retval;
	const char GATT_CHARACTERISTIC_VALUE[] = {0, value, 0, 0, 0};

	retval = bt_gatt_set_value(gatt_characteristic_handle, GATT_CHARACTERISTIC_VALUE, sizeof(GATT_CHARACTERISTIC_VALUE));

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_set_value() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool notify_gatt_characteristic_value_changed()
{
	int retval;

	retval = bt_gatt_server_notify_characteristic_changed_value(gatt_characteristic_handle, gatt_characteristic_notification_sent_callback, NULL, NULL);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_server_notify_characteristic_changed_value() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

void gatt_characteristic_notification_sent_callback(int result, const char *remote_address, bt_gatt_server_h server, bt_gatt_h characteristic, bool completed, void *user_data)
{
	if(!completed)
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to send a GATT characteristic value changed notification.", __FILE__, __func__, __LINE__);
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in sending a GATT characteristic value changed notification.", __FILE__, __func__, __LINE__);
}

bool get_gatt_characteristic_handle(bt_gatt_h *gatt_handle)
{
	if(gatt_characteristic_handle != 0)
	{
		*gatt_handle = gatt_characteristic_handle;
		return true;
	}
	else
		return false;
}
