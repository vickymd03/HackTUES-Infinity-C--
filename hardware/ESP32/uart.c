#include <string.h>
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include "main.h"
#include "http.h"

#define TXD_PIN (GPIO_NUM_4)
#define RXD_PIN (GPIO_NUM_5)


#define EX_UART_NUM UART_NUM_1
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/

#define BUF_SIZE (38800)
#define RD_BUF_SIZE (BUF_SIZE)



static const char *TAG="example uart";
static QueueHandle_t uart1_queue;
static QueueHandle_t tx_queue;


static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffer_size;
    uint8_t* buf_dtmp = (uint8_t*) malloc(120);
	//uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
	static uint8_t dtmp[RD_BUF_SIZE];
	static uint8_t *ptr_dtmp = (uint8_t*)&dtmp;
	uint8_t ACK;
	uint8_t CRC;
	static uint16_t k;

    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(uart1_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            bzero(buf_dtmp, 120);
            //ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
            switch(event.type) {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
                case UART_DATA:
                    uart_read_bytes(EX_UART_NUM, buf_dtmp, event.size, portMAX_DELAY);
//					if(event.size == 120){
//						for(uint8_t i = 0; i < 120; i++){
					if(event.size >= 120){
						for(uint8_t i = 0; i < event.size; i++){
							dtmp[k++] = buf_dtmp[i];
						}
						break;
					} else {
						for(uint8_t i = 0; i < event.size; i++){
							dtmp[k++] = buf_dtmp[i];
						}
						//ESP_LOGI(TAG, "[UART DATA]: %d", k);
					}
					CRC = 0;
					//for(int i = 0; i < k; i++){
					for(int i = 0; i < 38480; i++){
						if(i != 71) CRC += dtmp[i];
					}
					if(CRC == dtmp[71]){
						//ESP_LOGI(TAG, "CRC = %d", CRC,);
						if(dtmp[0] == 0x42 && dtmp[1] == 0x4d){
							if(dtmp[70] == mlx90640){
								ACK = OK_ACK;
							}else if(dtmp[70] == ov7725){
								ACK = OK_ACK;
								if(http_file_queue != 0)
                                    xQueueSend(http_file_queue, &ptr_dtmp, 1/portTICK_RATE_MS);
							}else {
								ACK = NAK;
							}
							k = 0;
							//bzero(dtmp, RD_BUF_SIZE);
							//free(dtmp);
							//dtmp = NULL;
						}else {
							ACK = NAK;
							ESP_LOGI(TAG, "Bad header");
						}
					}else {
						ACK = NAK;
						ESP_LOGI(TAG, "Bad CRC sum");
					}
					if(tx_queue != 0) xQueueSend(tx_queue, &ACK, 1/portTICK_RATE_MS);
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart1_queue);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EX_UART_NUM);
                    xQueueReset(uart1_queue);
                    break;
                //Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");
                    break;
                //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                //Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;
                //UART_PATTERN_DET
                case UART_PATTERN_DET:
                    uart_get_buffered_data_len(EX_UART_NUM, &buffer_size);
                    int pos = uart_pattern_pop_pos(EX_UART_NUM);
                    ESP_LOGI(TAG, "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffer_size);
                    if (pos == -1) {
                        // There used to be a UART_PATTERN_DET event, but the pattern position queue is full so that it can not
                        // record the position. We should set a larger queue size.
                        // As an example, we directly flush the rx buffer here.
                        uart_flush_input(EX_UART_NUM);
                    } else {
                        uart_read_bytes(EX_UART_NUM, dtmp, pos, 100 / portTICK_PERIOD_MS);
                        uint8_t pat[PATTERN_CHR_NUM + 1];
                        memset(pat, 0, sizeof(pat));
                        uart_read_bytes(EX_UART_NUM, pat, PATTERN_CHR_NUM, 100 / portTICK_PERIOD_MS);
                        ESP_LOGI(TAG, "read data: %s", dtmp);
                        ESP_LOGI(TAG, "read pat : %s", pat);
                    }
                    break;
                //Others
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    //free(dtmp);
    free(buf_dtmp);
    //dtmp = NULL;
    buf_dtmp = NULL;
    vTaskDelete(NULL);
}

static void tx_task(void *arg)
{
	static const char *TX_TASK_TAG = "TX_TASK";
	esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);

	uint8_t ACK[1];

	while(1){
		if(tx_queue != 0){
			if(xQueueReceive(tx_queue, &ACK[0], 10)){
				uint8_t txBytes = uart_write_bytes(UART_NUM_1, (uint8_t *)ACK, 1);
				//ESP_LOGI(TAG, "Send_by_Tx (%d bytes, ACK = \'0x%02x\')", txBytes, ACK[0]);
			}
		}
	}
}

void start_uart_event(void)
{
    esp_log_level_set(TAG, ESP_LOG_INFO);

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 921600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    //Install UART driver, and set the queue.
    uart_driver_install(EX_UART_NUM, 2048 * 2, 1024 * 2, 20, &uart1_queue, 0);
    uart_param_config(EX_UART_NUM, &uart_config);

    //Set UART log level
    esp_log_level_set(TAG, ESP_LOG_INFO);
    //Set UART pins (using UART0 default pins ie no changes.)
    uart_set_pin(UART_NUM_1, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    //Set uart pattern detect function.
    uart_enable_pattern_det_baud_intr(EX_UART_NUM, '+', PATTERN_CHR_NUM, 9, 0, 0);
    //Reset the pattern queue length to record at most 20 pattern positions.
    uart_pattern_queue_reset(EX_UART_NUM, 20);

    //Create a task to handler UART event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 8192, NULL, 1|portPRIVILEGE_BIT, NULL);
	ESP_LOGI(TAG, "UART is runing ...\n");

    //xTaskCreate(rx_task, "uart_rx_task", 2600, NULL, 1|portPRIVILEGE_BIT, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 2600, NULL, 2|portPRIVILEGE_BIT, NULL);

	tx_queue = xQueueCreate(8, sizeof(uint8_t));
	if(!tx_queue) ESP_LOGI(TAG, "tx_queue ERR created");
}
