#include "cytometer.h"
#include "xil_printf.h"



/* Current Cytometer State */
enum CYTOMETER_STATE current_state;


int     waveform_channel_enabled[AD_CHANNEL_NUM];
struct  DETECTOR_SETTING detector_settings[AD_CHANNEL_NUM];
int     threshold_type = THRESHOLD_TYPE_OR;
u8_t    waveform_enabled = 0;
u8_t    waveform_channels = 0;

void init_detectors()
{
	for (int i = 0; i < AD_CHANNEL_NUM; i++) {
		detector_settings[i].id = i+1;
        detector_settings[i].enabled = 0;
		detector_settings[i].enable_height = 0;
		detector_settings[i].enable_width = 0;
		detector_settings[i].enable_area = 0;
		detector_settings[i].enable_thresh = 0;
		detector_settings[i].threshold_val = 3000;
	}
}

void reset_detector(int id)
{
    detector_settings[id-1].enabled = 0;
    detector_settings[id-1].enable_height = 0;
    detector_settings[id-1].enable_width = 0;
    detector_settings[id-1].enable_area = 0;
    detector_settings[id-1].enable_thresh = 0;
    detector_settings[id-1].threshold_val = 3000;
}

void update_detector_settings(u8_t id, u8_t enable_height, u8_t enable_width, u8_t enable_area, u8_t enable_thresh, int threshold_val)
{
	struct DETECTOR_SETTING *detector = &(detector_settings[id-1]);
    detector->enabled = 1;
	detector->enable_height = enable_height;
	detector->enable_width = enable_width;
	detector->enable_area = enable_area;
	detector->enable_thresh = enable_thresh;
	detector->threshold_val = threshold_val;
}


void update_waveform_setting(u8_t waveform_en, u8_t channels)
{
    waveform_enabled = waveform_en;
    waveform_channels = channels; 
}


void update_therhold_type(int type)
{
    if (type != THRESHOLD_TYPE_OR || type != THRESHOLD_TYPE_AND)
        return;

    threshold_type = type;
}


void init_cytometer()
{
    current_state = STATE_IDLE;
	threshold_type = THRESHOLD_TYPE_OR;
	for (int i = 0; i < AD_CHANNEL_NUM; i++) {
        waveform_channel_enabled[i] = 0;
    }
    init_detectors();

	xil_printf("Initialize cytometer parameters ok.\n");
}

int get_current_state()
{
    return current_state;
}

void cytometer_start_analyze()
{
    if (current_state == STATE_IDLE) {
        current_state = STATE_ACQUIRING;
    }
}

void cytometer_stop_analyze()
{
    if (current_state == STATE_ACQUIRING) {
        current_state = STATE_IDLE;
    }
}

void cytometer_start_sort()
{
    if (current_state == STATE_IDLE) {
        current_state = STATE_SORTING;
    }
}
void cytometer_stop_sort()
{
    if (current_state == STATE_SORTING) {
        current_state = STATE_IDLE;
    }
}


