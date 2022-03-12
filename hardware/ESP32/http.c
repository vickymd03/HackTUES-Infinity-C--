#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_client.h"
//#include "esp_eth.h"								// receive request
#include "esp_tls.h"
#include "main.h"
#include "uart.h"
#include "http.h"


#define URL_post_esp_data "http://learningmoorree.000webhostapp.com/Measurement_shots/post-esp-data.php"
#define URL_post_esp_ov7725 "http://learningmore1.000webhostapp.com/Lunnar_rover/receive_frame.php"

////////////////////////////////////////////////////////////////////////////////

QueueHandle_t http_file_queue;

static const char *TAG="example";

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
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
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if(1){
				ESP_LOGI(TAG, "esp_http_client_is_chunked_response");
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                if (output_buffer != NULL) {
                    free(output_buffer);
                    output_buffer = NULL;
                }
                output_len = 0;
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            break;
    }
    return ESP_OK;
}

void send_request_for_controls(char *buf)
{
    char output_buffer[/*MAX_HTTP_OUTPUT_BUFFER*/1024] = {0};   // Buffer to store response of http request
    //int content_length = 0;

	esp_http_client_config_t config = {
		.url = URL_post_esp_data,
	    .event_handler = _http_event_handler,
        .user_data = output_buffer,		        // Pass address of local buffer to get response
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);

	esp_http_client_set_method(client, HTTP_METHOD_POST);
    //esp_http_client_set_header(client, "Content-Type", "application/sql");
	esp_http_client_set_post_field(client, buf, strlen(buf));
	ESP_LOGI(TAG, "Post_field: %s", buf);
	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK) {
		ESP_LOGI(TAG, "Status = %d, content_length = %d",
					esp_http_client_get_status_code(client),
					esp_http_client_get_content_length(client));
	}
#if(0)
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_err_t err = esp_http_client_open(client, strlen(buf));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    } else {
        int wlen = esp_http_client_write(client, buf, strlen(buf));
        if (wlen < 0) {
            ESP_LOGE(TAG, "Write failed");
        }
        int data_read = esp_http_client_read_response(client, output_buffer, /*MAX_HTTP_OUTPUT_BUFFER*/1024);
        if (data_read >= 0) {
            ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));
            ESP_LOG_BUFFER_HEX(TAG, output_buffer, strlen(output_buffer));
        } else {
            ESP_LOGE(TAG, "Failed to read response");
        }
    }
#endif
	ESP_LOGI(TAG, "Response: %s", output_buffer);	// TEST

	if(strstr(output_buffer, "New measurement created successfully")){
		ESP_LOGI(TAG, "New measurment made!");	// TEST
	}else if(strstr(output_buffer, "New event_table created successfully")){
		ESP_LOGI(TAG, "New event_table made!");	// TEST
	}else if(strstr(output_buffer, "Error:")){
		if(strstr(output_buffer, "doesn't exist")){
			ESP_LOGI(TAG, "REASON FOR Error: doesn't exist");
		}
	}else ESP_LOGI(TAG, "Unknown Error");

	esp_http_client_cleanup(client);
}

void send_ov7725_data_to_API(uint8_t* file, uint16_t size_of_file)
{
    char output_buffer[/*MAX_HTTP_OUTPUT_BUFFER*/1024] = {0};   // Buffer to store response of http request
	char head[] = "--GreenBo\r\nContent-Disposition: form-data; name=\"shotFile\"; filename=\"SmartEM_shot.bin\"\r\nContent-Type: application/octet-stream\r\n\r\n";
	char tail[] = "\r\n--GreenBo--\r\n";
	uint8_t buf_post[38840];
    uint8_t content_length[] = "hello";

	uint16_t k = strlen(head);
	for(uint16_t i = 0; i < k; i++){
		buf_post[i] = (uint8_t)head[i];
	}
	for(uint16_t i = k; i < k + size_of_file; i++){
		buf_post[i] = file[i - k];
	}
	k += size_of_file;
	for(uint16_t i = k; i < k + strlen(tail); i++){
		buf_post[i] = (uint8_t)tail[i - k];
	}

	esp_http_client_config_t config = {
		.url = URL_post_esp_ov7725,
	    .event_handler = _http_event_handler,
        .user_data = output_buffer,		        // Pass address of local buffer to get response
	};
	esp_http_client_handle_t client = esp_http_client_init(&config);

	esp_http_client_set_method(client, HTTP_METHOD_POST);
    //esp_http_client_set_header(client, "Content-Length", "16");
    esp_http_client_set_header(client, "Content-Type", "multipart/form-data; boundary=GreenBo");
	esp_http_client_set_post_field(client, (char*)buf_post, (strlen(head) + size_of_file + strlen(tail)));
	//ESP_LOGI(TAG, "Post_field: %s", (char*)buf_post);
	ESP_LOG_BUFFER_HEXDUMP(TAG, buf_post, 380, ESP_LOG_INFO);
	esp_err_t err = esp_http_client_perform(client);
	if (err == ESP_OK) {
		ESP_LOGI(TAG, "Status = %d, content_length = %d", esp_http_client_get_status_code(client), esp_http_client_get_content_length(client));
	}

	ESP_LOGI(TAG, "Response: %s", output_buffer);	// TEST
    if(1/*strstr(output_buffer, "Successfull transaction")*/){
        send_controls((uint8_t*)&output_buffer[0]);
    }

	esp_http_client_cleanup(client);
}
