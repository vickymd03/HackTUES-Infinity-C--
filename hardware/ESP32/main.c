/* HTTP File Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include "main.h"
#include <sys/param.h>

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "esp_http_client.h"

#include "esp_spiffs.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "uart.h"
#include "http.h"

#include "esp_log.h"
#include "esp_eth.h"
#include "esp_http_server.h"


////////////////////////////////////////////////////////////////////////////////

#define TWDT_TIMEOUT_S          3
#define TASK_RESET_PERIOD_S     2

/*
 * Macro to check the outputs of TWDT functions and trigger an abort if an
 * incorrect code is returned.
 */
#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected){                                  \
                printf("TWDT ERROR\n");                                \
                abort();                                               \
            }                                                          \
})

#define EXAMPLE_AP_WIFI_SSID		"WRover"
#define EXAMPLE_AP_WIFI_PASS		""
#define EXAMPLE_MAX_STA_CONN		4

#define EXAMPLE_SSID_Nomber_1
#if defined(EXAMPLE_SSID_Nomber_1)
	#define EXAMPLE_STA_WIFI_SSID		"Tech_D3881996"
	#define EXAMPLE_STA_WIFI_PASS		"FJHPEPJJ"
#elif defined(EXAMPLE_SSID_Nomber_2)
	#define EXAMPLE_STA_WIFI_SSID		"TiN-Incubator"
	#define EXAMPLE_STA_WIFI_PASS		""
#endif

#define EXAMPLE_STA_MAXIMUM_RETRY	5

#define MAX_EVENT_TAB 10
#define MAX_MEASUREMENT_TAB 50


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

/* This example demonstrates how to create file server
 * using esp_http_server. This file has only startup code.
 * Look in file_server.c for the implementation */

////////////////////////////////////////////////////////////////////////////////

static const char *TAG="example";

static int s_retry_num = 0;
//programData_t programs[21];
uint32_t ipaddress, ipnetmask, ipgw;


static TaskHandle_t task_handles[portNUM_PROCESSORS];

//Callback for user tasks created in app_main()
void reset_task(void *arg)
{
    //Subscribe this task to TWDT, then check if it is subscribed
    CHECK_ERROR_CODE(esp_task_wdt_add(NULL), ESP_OK);
    CHECK_ERROR_CODE(esp_task_wdt_status(NULL), ESP_OK);

    while(1){
        //reset the watchdog every 2 seconds
        CHECK_ERROR_CODE(esp_task_wdt_reset(), ESP_OK);  //Comment this line to trigger a TWDT timeout
        vTaskDelay(pdMS_TO_TICKS(TASK_RESET_PERIOD_S * 1000));
    }
}

void main_task(void *arg)
{
	char buf[200];
	uint8_t *file = NULL;

	while(1){
		if(http_file_queue != 0){
			if(xQueueReceive(http_file_queue, &file, 10)){
				ESP_LOGI(TAG, "Send_by_http");
				send_ov7725_data_to_API(file, 38480);
			}
		}
		if(0){
			send_request_for_controls((char*)&buf);
		}else if(0){
			send_request_for_controls((char*)&buf);
		}
		vTaskDelay(pdMS_TO_TICKS(10));   //Delay for 10 miliseconds
	}
}

/* Function to initialize SPIFFS */
static esp_err_t init_spiffs(void)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/spiffs",
      .partition_label = NULL,
      .max_files = 5,   // This decides the maximum number of files that can be created on the storage
      .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    return ESP_OK;
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_STA_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		ipaddress = event->ip_info.ip.addr;
		ipnetmask = event->ip_info.netmask.addr;
		ipgw = event->ip_info.gw.addr;
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/* /////// esp_wifi_types.h ///////
 * typedef union {
 *    wifi_ap_config_t  ap;
 *    wifi_sta_config_t sta;
 * } wifi_config_t; */
esp_err_t wifi_init_apsta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = EXAMPLE_AP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_AP_WIFI_SSID),
            .password = EXAMPLE_AP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
			//.channel = AP_CHANNEL
        },
    };
    if (strlen(EXAMPLE_AP_WIFI_PASS) == 0) {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    wifi_config_t sta_config = {
        .sta = {
            .ssid = EXAMPLE_STA_WIFI_SSID,
            .password = EXAMPLE_STA_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
			.threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_apsta finished. SSID:%s password:%s",
             EXAMPLE_AP_WIFI_SSID, EXAMPLE_AP_WIFI_PASS);

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_STA_WIFI_SSID, EXAMPLE_STA_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_STA_WIFI_SSID, EXAMPLE_STA_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }

    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler));
    vEventGroupDelete(s_wifi_event_group);

	return ESP_OK;
}

/* Declare the function which starts the file server.
 * Implementation of this function is to be found in
 * file_server.c */
esp_err_t start_file_server(const char *base_path);

#if(0)
void init_programs_data (programData_t *programs)
{
	programs->startHour = 0;
	programs->startMinute = 0;
	programs->separator1 = ',';
	programs->endHour = 0;
	programs->endMinute = 0;
	programs->separator2 = ',';
	*((uint8_t*)&programs->days_of_the_week) = 0x00;
	programs->separator3 = ',';
	programs->temperature = 0x2D;
	programs->separator4 = ',';
	programs->crc = programs->startHour + programs->startMinute + programs->endHour + \
					programs->endMinute + *((uint8_t*)&programs->days_of_the_week) + programs->temperature;
	programs->separator5 = '\n';
}

int make_file_data (char *buf, uint8_t mode)
{
	if (mode == 0) {
		const char *filepath = "/spiffs/program_list.hex";
		FILE *fd = NULL;
		struct stat file_stat;

		if (stat(filepath, &file_stat) == -1) {
			gpio_set_level(BLINK_GPIO, 1);					// DEL
			for (int i=0; i<21; i++) {
				init_programs_data(&programs[i]);
			}
			fd = fopen(filepath, "w");
			if (!fd) {
				gpio_set_level(BLINK_GPIO, 1);
				return 1;
			}
			fwrite(&programs[0], sizeof(programData_t), 21, fd);
			fclose(fd);
			gpio_set_level(BLINK_GPIO, 0);					// DEL
		}

		fd = fopen(filepath, "r");
		if (!fd) {
			gpio_set_level(BLINK_GPIO, 1);
			return 2;
		}
		if (fread(&programs[0], sizeof(programData_t), 21, fd) != 21) {
			gpio_set_level(BLINK_GPIO, 1);
			return 3;
		}

		for (int i=0; i<21; i++) {
			if (programs[i].crc != programs[i].startHour + programs[i].startMinute + programs[i].endHour + \
										programs[i].endMinute + *((uint8_t*)&programs[i].days_of_the_week) + programs[i].temperature) {
				init_programs_data(&programs[i]);
			}
		}
		fclose(fd);
	} else if (mode == 1){
		char *r;
		uint8_t n;
		uint8_t m;

/*		if (stat(filepath, &file_stat) == -1) {
			for (int i=0; i<21; i++) {
				if (programs[i].crc != programs[i].startHour + programs[i].startMinute + programs[i].endHour + \
											programs[i].endMinute + *((uint8_t*)&programs[i].days_of_the_week) + programs[i].temperature) {
					init_programs_data(&programs[i]);
				}
			}
		}*/
		r = strstr(buf, "program");
		if (r != NULL) {
			if(*(r+9) == '&') {
				n = (*(r+8) - 0x30);
			} else {
				n = ((*(r+8) - 0x30) * 10) + (*(r+9) - 0x30);
			}
			n--;
			r = strstr(buf, "start_time");
			if (r != NULL) {
				m = ((*(r+11) - 0x30) * 10) + (*(r+12) - 0x30);
				programs[n].startHour = m;
				m = ((*(r+16) - 0x30) * 10) + (*(r+17) - 0x30);
				programs[n].startMinute = m;
			}
			r = strstr(buf, "end_time");
			if (r != NULL) {
				m = ((*(r+9) - 0x30) * 10) + (*(r+10) - 0x30);
				programs[n].endHour = m;
				m = ((*(r+14) - 0x30) * 10) + (*(r+15) - 0x30);
				programs[n].endMinute = m;
			}

			r = strstr(buf, "mon");
			if (r != NULL) {
				*((uint8_t*)&programs[n].days_of_the_week) = 0;
				if (*(r+4) == 'y') programs[n].days_of_the_week.b_monday = 1;
				if (*(r+10) == 'y') programs[n].days_of_the_week.b_tuesday = 1;
				if (*(r+16) == 'y') programs[n].days_of_the_week.b_wednesday = 1;
				if (*(r+22) == 'y') programs[n].days_of_the_week.b_thursday = 1;
				if (*(r+28) == 'y') programs[n].days_of_the_week.b_friday = 1;
				if (*(r+34) == 'y') programs[n].days_of_the_week.b_saturday = 1;
				if (*(r+40) == 'y') programs[n].days_of_the_week.b_sunday = 1;
				if (*(r+46) == 'y') programs[n].days_of_the_week.b_hollidays = 1;
			}

			r = strstr(buf, "temperature");
			if (r != NULL) {
				m = ((*(r+12) - 0x30) * 10) + (*(r+13) - 0x30);
				programs[n].temperature = m;
			}
			programs[n].crc = programs[n].startHour + programs[n].startMinute + programs[n].endHour + \
											programs[n].endMinute + *((uint8_t*)&programs[n].days_of_the_week) + programs[n].temperature;
		}

	}

	return Data_OK;
}

void ip_file(void) {
	const char *filepath = "/spiffs/ip_data.hex";
    FILE *fd = NULL;


	fd = fopen(filepath, "w");
	fwrite(&ipaddress, sizeof(uint32_t), 1, fd);
	fwrite(&ipnetmask, sizeof(uint32_t), 1, fd);
	fwrite(&ipgw, sizeof(uint32_t), 1, fd);
	fclose(fd);
}


esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
			ESP_LOGI(TAG, "------------------------------------------");
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
			if(strstr(evt->data, "New record created successfully") != NULL){
				ESP_LOGI(TAG, "We have a new record on MySql Database!");
			}
			if (!esp_http_client_is_chunked_response(evt->client)) {
                ESP_LOGI(TAG, "Data length: %d", evt->data_len);
				ESP_LOGI(TAG, "\n%s", (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}
#endif

/* An HTTP GET handler */
static esp_err_t get_handler(httpd_req_t *req){
    char*  buf;
    size_t buf_len;

    /* Get header value string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
    if(buf_len > 1){
        buf = malloc(buf_len);
        /* Copy null terminated value string into buffer */
        if(httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK){
            ESP_LOGI(TAG, "Found header => Host: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-2") + 1;
    if(buf_len > 1){
        buf = malloc(buf_len);
        if (httpd_req_get_hdr_value_str(req, "Test-Header-2", buf, buf_len) == ESP_OK) {
            ESP_LOGI(TAG, "Found header => Test-Header-2: %s", buf);
        }
        free(buf);
    }

    buf_len = httpd_req_get_hdr_value_len(req, "Test-Header-1") + 1;
    if(buf_len > 1){
        buf = malloc(buf_len);
        if(httpd_req_get_hdr_value_str(req, "Test-Header-1", buf, buf_len) == ESP_OK){
            ESP_LOGI(TAG, "Found header => Test-Header-1: %s", buf);
        }
        free(buf);
    }

    /* Read URL query string length and allocate memory for length + 1,
     * extra byte for null termination */
    buf_len = httpd_req_get_url_query_len(req) + 1;
    if(buf_len > 1){
        buf = malloc(buf_len);
        if(httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK){
            ESP_LOGI(TAG, "Found URL query => %s", buf);
            char param[32];
            /* Get value of expected key from query string */
            if(httpd_query_key_value(buf, "query1", param, sizeof(param)) == ESP_OK){
                ESP_LOGI(TAG, "Found URL query parameter => query1=%s", param);
            }
            if(httpd_query_key_value(buf, "query3", param, sizeof(param)) == ESP_OK){
                ESP_LOGI(TAG, "Found URL query parameter => query3=%s", param);
            }
            if(httpd_query_key_value(buf, "query2", param, sizeof(param)) == ESP_OK){
                ESP_LOGI(TAG, "Found URL query parameter => query2=%s", param);
            }
        }
        free(buf);
    }

    /* Set some custom headers */
    httpd_resp_set_hdr(req, "Custom-Header-1", "Custom-Value-1");
    httpd_resp_set_hdr(req, "Custom-Header-2", "Custom-Value-2");

    /* Send response with custom headers and body set as the
     * string passed in user context*/
    const char* resp_str = (const char*) req->user_ctx;
    httpd_resp_send(req, resp_str, strlen(resp_str));

    /* After sending the HTTP response the old HTTP request
     * headers are lost. Check if HTTP request headers can be read now. */
    if(httpd_req_get_hdr_value_len(req, "Host") == 0){
        ESP_LOGI(TAG, "Request headers lost");
    }
    return ESP_OK;
}

static const httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};

/* An HTTP POST handler */
static esp_err_t post_handler(httpd_req_t *req){
    char buf[100];
    int ret, remaining = req->content_len;

    while(remaining > 0){
        /* Read the data for the request */
        if((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
            if(ret == HTTPD_SOCK_ERR_TIMEOUT){
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        httpd_resp_send_chunk(req, buf, ret);
        remaining -= ret;

        /* Log data received */
        ESP_LOGI(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGI(TAG, "====================================");
    }

    // End response
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t echo = {
    .uri       = "/echo",
    .method    = HTTP_POST,
    .handler   = post_handler,
    .user_ctx  = NULL
};

/* This handler allows the custom error handling functionality to be
 * tested from client side. For that, when a PUT request 0 is sent to
 * URI /ctrl, the /hello and /echo URIs are unregistered and following
 * custom error handler http_404_error_handler() is registered.
 * Afterwards, when /hello or /echo is requested, this custom error
 * handler is invoked which, after sending an error message to client,
 * either closes the underlying socket (when requested URI is /echo)
 * or keeps it open (when requested URI is /hello). This allows the
 * client to infer if the custom error handler is functioning as expected
 * by observing the socket state.
 */
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err){
    if(strcmp("/hello", req->uri) == 0){
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/hello URI is not available");
        /* Return ESP_OK to keep underlying socket open */
        return ESP_OK;
    }else if(strcmp("/echo", req->uri) == 0){
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "/echo URI is not available");
        /* Return ESP_FAIL to close underlying socket */
        return ESP_FAIL;
    }
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Some 404 error message");
    return ESP_FAIL;
}

/* An HTTP PUT handler. This demonstrates realtime
 * registration and deregistration of URI handlers
 */
static esp_err_t ctrl_put_handler(httpd_req_t *req){
    char buf;
    int ret;

    if((ret = httpd_req_recv(req, &buf, 1)) <= 0){
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    if(buf == '0'){
        /* URI handlers can be unregistered using the uri string */
        ESP_LOGI(TAG, "Unregistering /hello and /echo URIs");
        httpd_unregister_uri(req->handle, "/hello");
        httpd_unregister_uri(req->handle, "/echo");
        /* Register the custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, http_404_error_handler);
    }
    else{
        ESP_LOGI(TAG, "Registering /hello and /echo URIs");
        httpd_register_uri_handler(req->handle, &hello);
        httpd_register_uri_handler(req->handle, &echo);
        /* Unregister custom error handler */
        httpd_register_err_handler(req->handle, HTTPD_404_NOT_FOUND, NULL);
    }

    /* Respond with empty body */
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t ctrl = {
    .uri       = "/ctrl",
    .method    = HTTP_PUT,
    .handler   = ctrl_put_handler,
    .user_ctx  = NULL
};

static httpd_handle_t start_webserver(void){
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if(httpd_start(&server, &config) == ESP_OK){
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &echo);
        httpd_register_uri_handler(server, &ctrl);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server){
    httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if(*server){
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if(*server == NULL){
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}


void app_main(void)
{
    printf("Initialize TWDT\n");
    //Initialize or reinitialize TWDT
    CHECK_ERROR_CODE(esp_task_wdt_init(TWDT_TIMEOUT_S, false), ESP_OK);

    //Subscribe Idle Tasks to TWDT if they were not subscribed at startup
#ifndef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU0
    esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(0));
#endif
#ifndef CONFIG_ESP_TASK_WDT_CHECK_IDLE_TASK_CPU1
    esp_task_wdt_add(xTaskGetIdleTaskHandleForCPU(1));
#endif

    //Create user tasks and add them to watchdog (this is made so the watchdog doesn't say processes are "dead" or sth. like it)
    for(int i = 0; i < portNUM_PROCESSORS; i++){
        xTaskCreatePinnedToCore(reset_task, "reset task", 1024, NULL, 10, &task_handles[i], i);
    }

   /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
	gpio_set_level(BLINK_GPIO, 0);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(wifi_init_apsta());

    /* Initialize file storage */
    ESP_ERROR_CHECK(init_spiffs());


	start_uart_event();

	http_file_queue = xQueueCreate(8, sizeof(uint8_t*));
	if(!http_file_queue) ESP_LOGI(TAG, "tx_queue ERR created");

    xTaskCreate(main_task, "main_task", 50000, NULL, 1|portPRIVILEGE_BIT, NULL);
}
