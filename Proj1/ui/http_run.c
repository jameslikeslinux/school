#include "http.h"

#define RETRY_SLEEP 5

void http_run(void *http_t_as_void) {
	http_t *http = (http_t*) http_t_as_void;
	int ret, connect_try, request_try, disconnect_try;

	connect_try = 0;
	request_try = 0;

CONNECT:
	connect_try++;
	ret = http_connect(http);

	if (ret == FAIL || connect_try >= http->num_retry)
		return;
	else if (ret == RETRY) {
		log_printf(http->log, INFO, "http_run", "Retrying http_connect %d of %d times in %d seconds...", connect_try, http->num_retry, RETRY_SLEEP);
		sleep(5);
		goto CONNECT;
	} else
		connect_try = 0;

SEND_REQUEST:
	request_try++;
	ret = http_send_request(http);

	if (ret == FAIL || request_try >= http->num_retry)
		return;
	else if (ret == RETRY) {
		http_disconnect(http);
		log_printf(http->log, INFO, "http_run", "Retrying http_send_request %d of %d times in %d seconds...", request_try, http->num_retry, RETRY_SLEEP);
		sleep(5);
		goto CONNECT;
	} else
		request_try = 0;

	http_disconnect(http);

	draw_commands();
	draw_url_input();
	select_input_form();
}
