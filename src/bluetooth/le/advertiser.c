#include "heartratetransmitter.h"
#include "bluetooth/le/advertiser.h"

static bt_advertiser_h advertiser = 0;
static const char *SERVICE_UUID = "0000180D-0000-1000-8000-00805F9B34FB";

static void advertising_state_changed_callback(int result, bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e adv_state, void *user_data);

bool create_advertiser()
{
	int retval;

	retval = bt_adapter_le_create_advertiser(&advertiser);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_create_advertiser() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool set_advertising_device_name()
{
	int retval;

	retval = bt_adapter_le_set_advertising_device_name(advertiser, BT_ADAPTER_LE_PACKET_ADVERTISING, true);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_set_advertising_device_name() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool set_advertising_service_uuid()
{
	int retval;

	retval = bt_adapter_le_add_advertising_service_uuid(advertiser, BT_ADAPTER_LE_PACKET_ADVERTISING, SERVICE_UUID);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_add_advertising_service_uuid() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool start_advertising()
{
	int retval;

	retval = bt_adapter_le_start_advertising_new(advertiser, advertising_state_changed_callback, NULL);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_start_advertising() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

void advertising_state_changed_callback(int result, bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e adv_state, void *user_data)
{
	dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Function advertising_state_changed_callback() is invoked.", __FILE__, __func__, __LINE__);
}

bool clear_advertising_data()
{
	int retval;

	retval = bt_adapter_le_clear_advertising_data(advertiser, BT_ADAPTER_LE_PACKET_ADVERTISING);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_clear_advertising_data() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool destroy_advertiser()
{
	int retval;

	retval = bt_adapter_le_destroy_advertiser(advertiser);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_adapter_le_destroy_advertiser() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}
