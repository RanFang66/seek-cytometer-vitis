#ifndef __CYTOMETER__HEADER__
#define __CYTOMETER__HEADER__

#ifdef __cplusplus
extern "C" {
#endif

#include <arch/cc.h>

enum CYTOMETER_STATE {
    STATE_IDLE,
    STATE_ACQUIRING,
    STATE_SORTING,
    STATE_ERROR,    
};




struct DETECTOR_SETTING {
	u8_t 	id;
    u8_t    enabled;
	u8_t 	enable_height;
	u8_t	enable_width;
	u8_t 	enable_area;
	u8_t 	enable_thresh;

	int 	threshold_val;
};


#define AD_CHANNEL_NUM				        (8)
#define THRESHOLD_TYPE_OR 			        (0)
#define THRESHOLD_TYPE_AND		    	    (1)

void update_detector_settings(u8_t id, u8_t enable_height, u8_t enable_width, u8_t enable_area, u8_t enable_thresh, int threshold_val);
void reset_detector(int id);
void init_detectors();
void init_cytometer();
void cytometer_start_analyze();
void cytometer_stop_analyze();
void cytometer_start_sort();
void cytometer_stop_sort();


void update_waveform_setting(u8_t waveform_en, u8_t channels);

int get_current_state();

#ifdef __cplusplus
}
#endif

#endif /* __CYTOMETER__HEADER__ */