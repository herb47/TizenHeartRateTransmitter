#include "heartratetransmitter.h"
#include "bluetooth/gatt/service.h"

static const char *GATT_SERVICE_UUID = "0000180D-0000-1000-8000-00805F9B34FB";

static bt_gatt_h service = 0;

bool create_gatt_service()
{
	int retval;

	retval = bt_gatt_service_create(GATT_SERVICE_UUID, BT_GATT_SERVICE_TYPE_PRIMARY, &service);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_service_create() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool get_service_handle(bt_gatt_h *handle)
{
	if(service != 0)
	{
		*handle = service;
		return true;
	}
	else
		return false;
}

bool destroy_gatt_service()
{
	int retval;

	retval = bt_gatt_service_destroy(service);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_service_destroy() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}
