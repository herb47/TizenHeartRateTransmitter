#include "heartratetransmitter.h"
#include "bluetooth/gatt/descriptor.h"

static const char *DESCRIPTOR_UUID = "00002902-0000-1000-8000-00805F9B34FB";
static const char DESCRIPTOR_VALUE[] = {1, 0};

bt_gatt_h descriptor = 0;

bool create_descriptor()
{
	int retval;

	retval = bt_gatt_descriptor_create(DESCRIPTOR_UUID,
			BT_GATT_PERMISSION_READ|BT_GATT_PERMISSION_WRITE,
			DESCRIPTOR_VALUE, sizeof(DESCRIPTOR_VALUE), &descriptor);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_descriptor_create() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool destroy_descriptor()
{
	int retval;

	retval = bt_gatt_descriptor_destroy(descriptor);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_descriptor_destroy() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool get_descriptor_handle(bt_gatt_h *handle)
{
	if(descriptor != 0)
	{
		*handle = descriptor;
		return true;
	}
	else
		return false;
}
