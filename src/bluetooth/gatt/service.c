#include "heartratetransmitter.h"
#include "bluetooth/gatt/service.h"
#include "bluetooth/gatt/characteristic.h"

const char *GATT_SERVICE_UUID = "0000180D-0000-1000-8000-00805F9B34FB";

bt_gatt_h gatt_service_handle = 0;

bool create_gatt_service()
{
	int retval;

	retval = bt_gatt_service_create(GATT_SERVICE_UUID, BT_GATT_SERVICE_TYPE_PRIMARY, &gatt_service_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_service_create() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	if(!add_gatt_characteristic_to_gatt_service())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to add a characteristic to a specified service.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
	{
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in adding a characteristic to a specified service.", __FILE__, __func__, __LINE__);
		return true;
	}
}

bool add_gatt_characteristic_to_gatt_service()
{
	int retval;
	bt_gatt_h gatt_characteristic_handle = 0;

	if (!get_gatt_characteristic_handle(&gatt_characteristic_handle))
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to get the GATT characteristic's handle.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in getting the GATT characteristic's handle.", __FILE__, __func__, __LINE__);

	retval = bt_gatt_service_add_characteristic(gatt_service_handle, gatt_characteristic_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_service_add_characteristic() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool get_gatt_service_handle(bt_gatt_h *gatt_handle)
{
	if(gatt_service_handle != 0)
	{
		*gatt_handle = gatt_service_handle;
		return true;
	}
	else
		return false;
}

bool set_bluetooth_le_advertising_gatt_service_uuid(bt_advertiser_h bluetooth_le_advertiser_handle)
{
	int retval;

	retval = bt_adapter_le_add_advertising_service_uuid(bluetooth_le_advertiser_handle, BT_ADAPTER_LE_PACKET_ADVERTISING, GATT_SERVICE_UUID);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_add_advertising_service_uuid() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool destroy_gatt_service()
{
	int retval;

	retval = bt_gatt_service_destroy(gatt_service_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_service_destroy() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
	{
		gatt_service_handle = 0;
		return true;
	}
}
