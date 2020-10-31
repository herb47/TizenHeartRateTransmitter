#include "heartratetransmitter.h"
#include "bluetooth/le/advertiser.h"
#include "bluetooth/gatt/service.h"

bt_advertiser_h bluetooth_le_advertiser_handle = 0;

void bluetooth_le_advertising_state_changed_callback(int result, bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e adv_state, void *user_data);

bool create_bluetooth_le_advertiser()
{
	int retval;

	retval = bt_adapter_le_create_advertiser(&bluetooth_le_advertiser_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_create_advertiser() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}

	if(!set_bluetooth_le_advertising_device_name())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to set whether the device name should be included in advertise data.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in setting whether the device name should be included in advertise data.", __FILE__, __func__, __LINE__);

	if(!set_bluetooth_le_advertising_gatt_service_uuid(bluetooth_le_advertiser_handle))
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to add a GATT service UUID to advertise data.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in adding a GATT service UUID to advertise data.", __FILE__, __func__, __LINE__);

	if(!set_bluetooth_le_advertising_mode())
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to set advertising mode to control the advertising power and latency.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
	{
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in setting advertising mode to control the advertising power and latency.", __FILE__, __func__, __LINE__);
		return true;
	}
}

bool set_bluetooth_le_advertising_device_name()
{
	int retval;

	retval = bt_adapter_le_set_advertising_device_name(bluetooth_le_advertiser_handle, BT_ADAPTER_LE_PACKET_ADVERTISING, true);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_set_advertising_device_name() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool start_bluetooth_le_advertising()
{
	int retval;

	retval = bt_adapter_le_start_advertising_new(bluetooth_le_advertiser_handle, bluetooth_le_advertising_state_changed_callback, NULL);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_start_advertising() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

void bluetooth_le_advertising_state_changed_callback(int result, bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e adv_state, void *user_data)
{
	switch(adv_state)
	{
		case BT_ADAPTER_LE_ADVERTISING_STARTED:
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Bluetooth advertising is started.", __FILE__, __func__, __LINE__);
			break;
		case BT_ADAPTER_LE_ADVERTISING_STOPPED:
			dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Bluetooth advertising is stopped.", __FILE__, __func__, __LINE__);
			break;
	}
}

bool stop_bluetooth_le_advertising()
{
	int retval;

	retval = bt_adapter_le_stop_advertising(bluetooth_le_advertiser_handle);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_stop_advertising() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool set_bluetooth_le_advertising_mode()
{
	int retval;

	retval = bt_adapter_le_set_advertising_mode(bluetooth_le_advertiser_handle, BT_ADAPTER_LE_ADVERTISING_MODE_LOW_ENERGY);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_set_advertising_device_name() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}
