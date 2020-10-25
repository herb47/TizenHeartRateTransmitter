#include "heartratetransmitter.h"
#include "bluetooth/gatt/service.h"
#include "bluetooth/gatt/characteristic.h"

static const char *SERVICE_UUID = "0000180D-0000-1000-8000-00805F9B34FB";

static bt_gatt_h service = 0;

bool create_service()
{
	int retval;

	retval = bt_gatt_service_create(SERVICE_UUID, BT_GATT_SERVICE_TYPE_PRIMARY, &service);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_service_create() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool add_characteristic_to_service()
{
	bt_gatt_h characteristic = 0;
	int retval;

	if (!get_characteristic_handle(&characteristic))
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to get the GATT characteristic's handle.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in getting the GATT characteristic's handle.", __FILE__, __func__, __LINE__);

	retval = bt_gatt_service_add_characteristic(service, characteristic);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_service_add_characteristic() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool destroy_service()
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
