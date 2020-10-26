#include "heartratetransmitter.h"
#include "bluetooth/gatt/characteristic.h"
#include "bluetooth/gatt/descriptor.h"

static const char *CHARACTERISTIC_UUID = "00002A37-0000-1000-8000-00805F9B34FB";

bt_gatt_h characteristic = 0;

static void notification_sent_callback(int result, const char *remote_address, bt_gatt_server_h server, bt_gatt_h characteristic, bool completed, void *user_data);

bool create_characteristic()
{
	const char CHARACTERISTIC_VALUE[] = {0, 0, 0, 0, 0};
	int retval;

	retval = bt_gatt_characteristic_create(CHARACTERISTIC_UUID,
			BT_GATT_PERMISSION_READ|BT_GATT_PERMISSION_WRITE, BT_GATT_PROPERTY_NOTIFY,
			CHARACTERISTIC_VALUE, sizeof(CHARACTERISTIC_VALUE), &characteristic);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_characteristic_create() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool add_descriptor_to_characteristic()
{
	bt_gatt_h descriptor = 0;
	int retval;

	if (!get_descriptor_handle(&descriptor))
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to get the GATT descriptor's handle.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in getting the GATT descriptor's handle.", __FILE__, __func__, __LINE__);

	retval =  bt_gatt_characteristic_add_descriptor(characteristic, descriptor);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_characteristic_add_descriptor() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool set_characteristic_value(int value)
{
	const char CHARACTERISTIC_VALUE[] = {0, value, 0, 0, 0};
	int retval;

	retval = bt_gatt_set_value(characteristic, CHARACTERISTIC_VALUE, sizeof(CHARACTERISTIC_VALUE));

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_set_value() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool notify_characteristic_value_changed()
{
	int retval;

	retval = bt_gatt_server_notify_characteristic_changed_value(characteristic, notification_sent_callback, NULL, NULL);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_server_notify_characteristic_changed_value() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

void notification_sent_callback(int result, const char *remote_address, bt_gatt_server_h server, bt_gatt_h characteristic, bool completed, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Function notification_sent_callback() is invoked.", __FILE__, __func__, __LINE__);
}

bool destroy_characteristic()
{
	int retval;

	retval = bt_gatt_characteristic_destroy(characteristic);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_characteristic_destroy() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool get_characteristic_handle(bt_gatt_h *handle)
{
	if(characteristic != 0)
	{
		*handle = characteristic;
		return true;
	}
	else
		return false;
}
