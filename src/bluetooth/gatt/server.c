#include "heartratetransmitter.h"
#include "bluetooth/gatt/server.h"
#include "bluetooth/gatt/service.h"

static bt_gatt_server_h server = 0;

bool create_gatt_server()
{
	int retval;

	retval = bt_gatt_server_create(&server);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_server_create() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}

bool register_service_to_server()
{
	bt_gatt_h service = 0;
	int retval;

	if (!get_service_handle(&service))
	{
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to get the GATT service's handle.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in getting the GATT service's handle.", __FILE__, __func__, __LINE__);

	retval = bt_gatt_server_register_service(server, service);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_server_register_service() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		dlog_print(DLOG_ERROR, LOG_TAG, "%s/%s/%d: Failed to register a specified service to the specified GATT server that the local device is hosting.", __FILE__, __func__, __LINE__);
		return false;
	}
	else
	{
		dlog_print(DLOG_INFO, LOG_TAG, "%s/%s/%d: Succeeded in registering a specified service to the specified GATT server that the local device is hosting.", __FILE__, __func__, __LINE__);
		return true;
	}
}

bool destroy_gatt_server()
{
	int retval;

	retval = bt_gatt_server_destroy(server);

	if(retval != BT_ERROR_NONE)
	{
		dlog_print(DLOG_DEBUG, LOG_TAG, "%s/%s/%d: Function bt_gatt_server_destroy() return value = %s", __FILE__, __func__, __LINE__, get_error_message(retval));
		return false;
	}
	else
		return true;
}
