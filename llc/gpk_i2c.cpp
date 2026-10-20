// #include "gpk_i2c.h"
// #include "gpk_platform_error.h"
// #include "gpk_mutex.h"

// #ifdef GPK_ESP32
// #	include "esp32-hal-i2c.h"
// #	include "esp32-hal.h"
// #	if !CONFIG_DISABLE_HAL_LOCKS
// #		include "freertos/FreeRTOS.h"
// #		include "freertos/task.h"
// #		include "freertos/semphr.h"
// #	endif // !CONFIG_DISABLE_HAL_LOCKS
// #	include "esp_attr.h"
// #	include "esp_system.h"
// #	include "soc/soc_caps.h"
// #	include "soc/i2c_periph.h"
// #	include "hal/i2c_hal.h"
// #	include "hal/i2c_ll.h"
// #	include "driver/i2c.h"
// #endif // GPK_ESP32

// namespace gpk
// {
// 	stxp	u1_t	I2C_INTERRUPTS_MASK	= (u1_t)0x3fff; // I2C all interrupt bitmap
// 	// I2C hardware cmd register fields.
// 	struct SI2CCommandRegister {
// 		u2_t	byte_num	: 8;
// 		u2_t	ack_en		: 1;
// 		u2_t	ack_exp		: 1;
// 		u2_t	ack_val		: 1;
// 		u2_t	op_code		: 3;
// 		u2_t	reserved14	: 17;
// 		u2_t	done		: 1;
// 	};
// 	GDEFINE_ENUM_TYPE(I2C_INTERRUPT, u0_t);
// 	GDEFINE_ENUM_VALUE(I2C_INTERRUPT, Error				, 0);	// I2C error
// 	GDEFINE_ENUM_VALUE(I2C_INTERRUPT, Arbition_Lost		, 1);	// I2C arbition lost event
// 	GDEFINE_ENUM_VALUE(I2C_INTERRUPT, NACK				, 2);	// I2C NACK event
// 	GDEFINE_ENUM_VALUE(I2C_INTERRUPT, Timeout			, 3);	// I2C time out event
// 	GDEFINE_ENUM_VALUE(I2C_INTERRUPT, End_Detected		, 4);	// I2C end detected event
// 	GDEFINE_ENUM_VALUE(I2C_INTERRUPT, Transaction_Done	, 5);	// I2C trans done event
// 	GDEFINE_ENUM_VALUE(I2C_INTERRUPT, RX_FIFO_Full		, 6);	// I2C rxfifo full event
// 	GDEFINE_ENUM_VALUE(I2C_INTERRUPT, TX_FIFO_Empty		, 7);	// I2C txfifo empty event

// 	// Data structure for calculating I2C bus timing.
// 	struct SI2CClockTiming {
// 		u1_t		scl_low   		= {};	// I2C scl low period
// 		u1_t		scl_high  		= {};	// I2C scl hight period
// 		u1_t		sda_hold  		= {};	// I2C scl low period
// 		u1_t		sda_sample		= {};	// I2C sda sample time
// 		u1_t		setup     		= {};	// I2C start and stop condition setup period
// 		u1_t		hold      		= {};	// I2C start and stop condition hold period
// 		u1_t		tout      		= {};	// I2C bus timeout period
// 	};
// 	// I2C operation mode command
// 	GDEFINE_ENUM_TYPE(I2C_MODE_COMMAND, u0_t);
// 	GDEFINE_ENUM_VALUE(I2C_MODE_COMMAND, Restart	, 0);
// 	GDEFINE_ENUM_VALUE(I2C_MODE_COMMAND, Write  	, 1);
// 	GDEFINE_ENUM_VALUE(I2C_MODE_COMMAND, Read   	, 2);
// 	GDEFINE_ENUM_VALUE(I2C_MODE_COMMAND, Stop   	, 3);
// 	GDEFINE_ENUM_VALUE(I2C_MODE_COMMAND, End    	, 4);
// #ifdef GPK_ESP32
// 	stxp	u2_t		I2C_INTERRUPT_MASTER_TX			= I2C_ACK_ERR_INT_ENA_M | I2C_TIME_OUT_INT_ENA_M | I2C_TRANS_COMPLETE_INT_ENA_M | I2C_ARBITRATION_LOST_INT_ENA_M | I2C_END_DETECT_INT_ENA_M;
// 	stxp	u2_t		I2C_INTERRUPT_MASTER_RX			= I2C_TIME_OUT_INT_ENA_M | I2C_TRANS_COMPLETE_INT_ENA_M | I2C_ARBITRATION_LOST_INT_ENA_M | I2C_END_DETECT_INT_ENA_M;
// 	stxp	u2_t		I2C_INTERRUPT_SLAVE_TX 			= I2C_TXFIFO_EMPTY_INT_ENA_M;
// 	stxp	u2_t		I2C_INTERRUPT_SLAVE_RX 			= I2C_RXFIFO_FULL_INT_ENA_M | I2C_TRANS_COMPLETE_INT_ENA_M;
// 	stxp	u2_t		I2C_MAX_TIMEOUT        			= I2C_TIME_OUT_REG_V;
// #endif
// // 	sinx	err_t		i2cClockSourceFrequency			(i2c_sclk_t & src_clk)  													{ return 80 * 1000 * 1000; }
// // 	stin	i2c_dev_t	i2cGetHW						(u0_t i2c_num)   															{ return (i2c_num) ? &I2C1 : &I2C0; }
// // 	stin	err_t		GetFIFOAddress					(u0_t i2c_num)   															{ return I2C_DATA_APB_REG(i2c_num); }
// // 	stin	err_t		i2c_ll_txfifo_rst           	(i2c_dev_t *hw)                                                             { hw->fifo_conf.tx_fifo_rst = 1; return hw->fifo_conf.tx_fifo_rst = 0; }
// // 	stin	err_t		i2c_ll_rxfifo_rst           	(i2c_dev_t *hw)                                                             { hw->fifo_conf.rx_fifo_rst = 1; return hw->fifo_conf.rx_fifo_rst = 0; }
// // 	stin	err_t		i2c_ll_set_scl_timing       	(i2c_dev_t *hw, int hight_period, int low_period)                           { hw->scl_low_period.period = low_period; return hw->scl_high_period.period = hight_period; }
// // 	stin	err_t		i2c_ll_clr_intsts_mask      	(i2c_dev_t *hw, u2_t mask)                                                  { return hw->int_clr.val = mask; }
// // 	stin	err_t		i2c_ll_enable_intr_mask     	(i2c_dev_t *hw, u2_t mask)                                                  { return hw->int_ena.val |= mask; }
// // 	stin	err_t		i2c_ll_disable_intr_mask    	(i2c_dev_t *hw, u2_t mask)                                                  { return hw->int_ena.val &= (~mask); }
// // 	stin	err_t		i2c_ll_get_intsts_mask      	(i2c_dev_t *hw)                                                             { return hw->int_status.val; }
// // 	stin	err_t		i2c_ll_set_fifo_mode        	(i2c_dev_t *hw, bool fifo_mode_en)                                          { return hw->fifo_conf.nonfifo_en = fifo_mode_en ? 0 : 1; }
// // 	stin	err_t		i2c_ll_set_tout             	(i2c_dev_t *hw, int tout)                                                   { return hw->timeout.tout = tout; }
// // 	stin	err_t		i2c_ll_set_slave_addr       	(i2c_dev_t *hw, u1_t slave_addr, bool addr_10bit_en)                        { hw->slave_addr.addr = slave_addr; return hw->slave_addr.en_10bit = addr_10bit_en; }
// // 	stin	err_t		i2c_ll_write_cmd_reg        	(i2c_dev_t *hw, SI2CCommandRegister cmd, int cmd_idx)                              { return hw->command[cmd_idx].val = cmd.val; }
// // 	stin	err_t		i2c_ll_set_start_timing     	(i2c_dev_t *hw, int start_setup, int start_hold)                            { hw->scl_rstart_setup.time = start_setup; return hw->scl_start_hold.time = start_hold; }
// // 	stin	err_t		i2c_ll_set_stop_timing      	(i2c_dev_t *hw, int stop_setup, int stop_hold)                              { hw->scl_stop_setup.time = stop_setup; return hw->scl_stop_hold.time = stop_hold; }
// // 	stin	err_t		i2c_ll_set_sda_timing       	(i2c_dev_t *hw, int sda_sample, int sda_hold)                               { hw->sda_hold.time = sda_hold; return hw->sda_sample.time = sda_sample; }
// // 	stin	err_t		i2c_ll_set_txfifo_empty_thr 	(i2c_dev_t *hw, u0_t empty_thr)                                             { return hw->fifo_conf.tx_fifo_empty_thrhd = empty_thr; }
// // 	stin	err_t		i2c_ll_set_rxfifo_full_thr  	(i2c_dev_t *hw, u0_t full_thr)                                              { return hw->fifo_conf.rx_fifo_full_thrhd = full_thr; }
// // 	stin	err_t		i2c_ll_set_data_mode        	(i2c_dev_t *hw, i2c_trans_mode_t tx_mode, i2c_trans_mode_t rx_mode)         { hw->ctr.tx_lsb_first = tx_mode; return hw->ctr.rx_lsb_first = rx_mode; }
// // 	stin	err_t		i2c_ll_get_data_mode        	(i2c_dev_t *hw, i2c_trans_mode_t *tx_mode, i2c_trans_mode_t *rx_mode)       { *tx_mode = hw->ctr.tx_lsb_first; return *rx_mode = hw->ctr.rx_lsb_first; }
// // 	stin	err_t		i2c_ll_get_sda_timing       	(i2c_dev_t *hw, int *sda_sample, int *sda_hold)                             { *sda_hold = hw->sda_hold.time; return *sda_sample = hw->sda_sample.time; }
// // 	stin	err_t		i2c_i2c_ll_get_hw_version   	(i2c_dev_t *hw)                                                             { return hw->date; }
// // 	stin	err_t		i2c_i2c_ll_is_bus_busy      	(i2c_dev_t *hw)                                                             { return hw->status_reg.bus_busy; }
// // 	stin	err_t		i2c_i2c_ll_is_master_mode   	(i2c_dev_t *hw)                                                             { return hw->ctr.ms_mode; }
// // 	stin	err_t		i2c_i2c_ll_get_rxfifo_cnt   	(i2c_dev_t *hw)                                                             { return hw->status_reg.rx_fifo_cnt; }
// // 	stin	err_t		i2c_i2c_ll_get_txfifo_len   	(i2c_dev_t *hw)                                                             { return SOC_I2C_FIFO_LEN - hw->status_reg.tx_fifo_cnt; }
// // 	stin	err_t		i2c_i2c_ll_get_tout         	(i2c_dev_t *hw)                                                             { return hw->timeout.tout; }
// // 	stin	err_t		i2c_ll_trans_start          	(i2c_dev_t *hw)                                                             { return hw->ctr.trans_start = 1; }
// // 	stin	err_t		i2c_ll_get_start_timing     	(i2c_dev_t *hw, int *setup_time, int *hold_time)                            { *setup_time = hw->scl_rstart_setup.time; return *hold_time = hw->scl_start_hold.time; }
// // 	stin	err_t		i2c_ll_get_stop_timing      	(i2c_dev_t *hw, int *setup_time, int *hold_time)                            { *setup_time = hw->scl_stop_setup.time;   return *hold_time = hw->scl_stop_hold.time; }
// // 	stin	err_t		i2c_ll_get_scl_timing       	(i2c_dev_t *hw, int *high_period, int *low_period)                          { *high_period = hw->scl_high_period.period; return *low_period = hw->scl_low_period.period; }
// // 	stin	err_t		i2c_ll_write_txfifo         	(i2c_dev_t *hw, u0_t *ptr, u0_t len)                                        { u2_t fifo_addr = (hw == &I2C0) ? 0x6001301c : 0x6002701c; for(int i = 0; i < len; i++) WRITE_PERI_REG(fifo_addr, ptr[i]); return fifo_addr; }
// // 	stin	err_t		i2c_ll_read_rxfifo          	(i2c_dev_t *hw, u0_t *ptr, u0_t len)                                        { for(int i = 0; i < len; i++)  ptr[i] = HAL_FORCE_READ_U32_REG_FIELD(hw->fifo_data, data); return len; }
// // 	stin	err_t		i2c_ll_set_filter           	(i2c_dev_t *hw, u0_t filter_num)                                            { return hw->scl_filter_cfg.en = hw->sda_filter_cfg.en = hw->scl_filter_cfg.thres = hw->sda_filter_cfg.thres = filter_num; }
// // 	stin	err_t		i2c_ll_get_filter           	(i2c_dev_t *hw)                                                             { return hw->sda_filter_cfg.thres; }
// // 	stin	err_t		i2c_ll_master_enable_tx_it  	(i2c_dev_t *hw)                                                             { hw->int_clr.val = ~0; return hw->int_ena.val = I2C_INTERRUPT_MASTER_TX; }
// // 	stin	err_t		i2c_ll_master_enable_rx_it  	(i2c_dev_t *hw)                                                             { hw->int_clr.val = ~0; return hw->int_ena.val = I2C_INTERRUPT_MASTER_RX; }
// // 	stin	err_t		i2c_ll_master_disable_tx_it 	(i2c_dev_t *hw)                                                             { return hw->int_ena.val &= (~I2C_INTERRUPT_MASTER_TX); }
// // 	stin	err_t		i2c_ll_master_disable_rx_it 	(i2c_dev_t *hw)                                                             { return hw->int_ena.val &= (~I2C_INTERRUPT_MASTER_RX); }
// // 	stin	err_t		i2c_ll_master_clr_tx_it     	(i2c_dev_t *hw)                                                             { return hw->int_clr.val = I2C_INTERRUPT_MASTER_TX; }
// // 	stin	err_t		i2c_ll_master_clr_rx_it     	(i2c_dev_t *hw)                                                             { return hw->int_clr.val = I2C_INTERRUPT_MASTER_RX; }
// // 	stin	err_t		i2c_ll_slave_enable_tx_it   	(i2c_dev_t *hw)                                                             { return hw->int_ena.val |= I2C_INTERRUPT_SLAVE_TX; } // Enable I2C slave TX interrupt
// // 	stin	err_t		i2c_ll_slave_enable_rx_it   	(i2c_dev_t *hw)                                                             { return hw->int_ena.val |= I2C_INTERRUPT_SLAVE_RX; } // Enable I2C slave RX interrupt
// // 	stin	err_t		i2c_ll_slave_disable_tx_it  	(i2c_dev_t *hw)                                                             { return hw->int_ena.val &= (~I2C_INTERRUPT_SLAVE_TX); }  // Disable I2C slave TX interrupt
// // 	stin	err_t		i2c_ll_slave_disable_rx_it  	(i2c_dev_t *hw)                                                             { return hw->int_ena.val &= (~I2C_INTERRUPT_SLAVE_RX); }  // Disable I2C slave RX interrupt
// // 	stin	err_t		i2c_ll_slave_clr_tx_it      	(i2c_dev_t *hw)                                                             { return hw->int_clr.val = I2C_INTERRUPT_SLAVE_TX; }  // Clear I2C slave TX interrupt status register
// // 	stin	err_t		i2c_ll_slave_clr_rx_it      	(i2c_dev_t *hw)                                                             { return hw->int_clr.val = I2C_INTERRUPT_SLAVE_RX; } // Clear I2C slave RX interrupt status register.
// // 	stin	err_t		i2c_ll_master_fsm_rst       	(i2c_dev_t *hw)                                                             { return 0; } // Reste I2C master FSM. When the master FSM is stuck, call this function to reset the FSM. No support on ESP32.
// // 	stin	err_t		i2c_ll_master_clr_bus       	(i2c_dev_t *hw)                                                             { return 0; } // Clear I2C bus, when the slave is stuck in a deadlock and keeps pulling the bus low, master can controls the SCL bus to generate 9 CLKs.  Note: The master cannot detect if deadlock happens, but when the scl_st_to interrupt is generated, a deadlock may occur. No support on ESP32
// // 	stin	err_t		i2c_ll_set_source_clk       	(i2c_dev_t *hw, i2c_sclk_t src_clk)                                         { return 0; } // Set I2C source clock. No support on ESP32
// // 	stin	err_t		i2c_ll_master_init          	(i2c_dev_t *hw)                                                             { typeof(hw->ctr) ctrl_reg; ctrl_reg.val = 0; ctrl_reg.ms_mode = 1; ctrl_reg.scl_force_out = ctrl_reg.sda_force_out = 1; return hw->ctr.val = ctrl_reg.val; }
// // 	stin	err_t		i2c_ll_slave_init           	(i2c_dev_t *hw)                                                             { typeof(hw->ctr) ctrl_reg; ctrl_reg.val = 0; ctrl_reg.scl_force_out = ctrl_reg.sda_force_out = 1; hw->ctr.val = ctrl_reg.val; return hw->fifo_conf.fifo_addr_cfg_en = 0; }
// // 	stin	err_t		i2c_ll_update               	(i2c_dev_t *hw)                                                             { return 0; }
// // 	stin	err_t		i2c_ll_set_scl_clk_timing   	(i2c_dev_t *hw, int high_period, int low_period, int wait_high_period)      { (void)wait_high_period; hw->scl_low_period.period = low_period; return hw->scl_high_period.period = high_period; }
// // 	stin	err_t		i2c_ll_get_scl_clk_timing   	(i2c_dev_t *hw, int *high_period, int *low_period, int *wait_high_period)   { *wait_high_period = 0; *high_period = hw->scl_high_period.period; return *low_period = hw->scl_low_period.period; }
// // 	stin	err_t		i2c_ll_slave_get_event      	(i2c_dev_t *hw, I2C_INTERRUPT *event)                                    {
// // 	    typeof(hw->int_status)  int_sts           = hw->int_status;
// // 	         if (int_sts.tx_fifo_empty      ) *event = I2C_INTERRUPT_TX_FIFO_Empty;
// // 	    else if (int_sts.trans_complete     ) *event = I2C_INTERRUPT_Transaction_Done;
// // 	    else if (int_sts.rx_fifo_full       ) *event = I2C_INTERRUPT_RX_FIFO_Full;
// // 	    else
// // 	        *event = I2C_INTERRUPT_Error;
// // 		return *event;
// // 	}
// // 	stin	err_t		i2c_ll_master_get_event          (i2c_dev_t *hw, I2C_INTERRUPT *event)                                    {
// // 	    typeof(hw->int_status)  int_sts           = hw->int_status;
// // 	         if (int_sts.arbitration_lost   ) *event = I2C_INTERRUPT_Arbition_Lost;
// // 	    else if (int_sts.ack_err            ) *event = I2C_INTERRUPT_NACK;
// // 	    else if (int_sts.time_out           ) *event = I2C_INTERRUPT_Timeout;
// // 	    else if (int_sts.end_detect         ) *event = I2C_INTERRUPT_End_Detected;
// // 	    else if (int_sts.trans_complete     ) *event = I2C_INTERRUPT_Transaction_Done;
// // 	    else
// // 	        *event = I2C_INTERRUPT_Error;
// // 		return *event;
// // 	}
// // 	stin	err_t		i2c_ll_cal_bus_clk               (u2_t source_clk, u2_t bus_freq, SI2CClockTiming *clk_cal)                    {
// // 	    u2_t                half_cycle        = source_clk / bus_freq / 2;
// // 	    clk_cal->scl_low    = clk_cal->scl_high   =
// // 	    clk_cal->setup      = clk_cal->hold       = half_cycle;
// // 	    clk_cal->sda_hold   = half_cycle / 2;
// // 	    clk_cal->sda_sample = clk_cal->scl_high / 2;
// // 	    clk_cal->tout       = half_cycle * 20; //default we set the timeout value to 10 bus cycles.
// // 		return half_cycle;
// // 	}
// // 	 // SCL period. According to the TRM, we should always subtract 1 to SCL low period
// // 	stin	err_t		i2c_ll_set_bus_timing            (i2c_dev_t *hw, SI2CClockTiming & bus_cfg)                                     {
// // 	    assert(bus_cfg.scl_low > 0);
// // 	    hw->scl_low_period.period    = bus_cfg->scl_low - 1;
// // 	    u1_t			scl_high            = bus_cfg->scl_high;	// Still according to the TRM, if filter is not enbled, we have to subtract 7, if SCL filter is enabled, we have to subtract: 8 if SCL filter is between 0 and 2 (included) 6 + SCL threshold if SCL filter is between 3 and 7 (included) to SCL high period
// // 	    assert(scl_high > 13);  								// In the "worst" case, we will subtract 13, make sure the result will still be correct
// // 	    if (0 == hw->scl_filter_cfg.en)
// // 	        scl_high -= 7;
// // 	     else {
// // 	        if (hw->scl_filter_cfg.thres <= 2)
// // 	            scl_high -= 8;
// // 	        else {
// // 	            assert(hw->scl_filter_cfg.thres <= 7);
// // 	            scl_high -= hw->scl_filter_cfg.thres + 6;
// // 	        }
// // 	    }
// // 	    hw->scl_high_period.period	= scl_high;
// // 	    // sda sample
// // 	    hw->sda_hold.time			= bus_cfg->sda_hold;
// // 	    hw->sda_sample.time			= bus_cfg->sda_sample;
// // 	    // setup
// // 	    hw->scl_rstart_setup.time	= 
// // 	    hw->scl_stop_setup.time		= bus_cfg->setup;
// // 	    // hold
// // 	    hw->scl_start_hold.time		= 
// // 	    hw->scl_stop_hold.time		= bus_cfg->hold;
// // 	    hw->timeout.tout			= bus_cfg->tout;
// // 		return scl_high;
// // 	}

// // 	//sttc	gpk::err_t	i2c_driver_install              (i2c_port_t i2c_num, i2c_mode_t mode, size_t slv_rx_buf_len, size_t slv_tx_buf_len, int intr_alloc_flags);
// // 	//sttc	gpk::err_t	i2c_set_pin                     (i2c_port_t i2c_num, int sda_io_num, int scl_io_num,
// // 	//sttc	gpk::err_t	i2c_driver_delete               (i2c_port_t i2c_num);
// // 	//sttc	gpk::err_t	i2c_param_config                (i2c_port_t i2c_num, const i2c_config_t *i2c_conf);
// // 	//sttc	gpk::err_t	i2c_reset_rx_fifo               (i2c_port_t i2c_num);
// // 	//sttc	gpk::err_t	i2c_isr_register                (i2c_port_t i2c_num, void (*fn)(void *), void *arg, int intr_alloc_flags, intr_handle_t *handle);
// // 	//sttc	gpk::err_t	i2c_isr_free                    (intr_handle_t handle);
// // 	//sttc	gpk::err_t	i2c_master_write_to_device      (i2c_port_t i2c_num, u0_t device_address, const u0_t* write_buffer, size_t write_size, TickType_t ticks_to_wait);
// // 	//sttc	gpk::err_t	i2c_master_read_from_device     (i2c_port_t i2c_num, u0_t device_address, u0_t* read_buffer, size_t read_size, TickType_t ticks_to_wait);
// // 	//sttc	gpk::err_t	i2c_master_write_read_device    (i2c_port_t i2c_num, u0_t device_address, const u0_t* write_buffer, size_t write_size, u0_t* read_buffer, size_t read_size, TickType_t ticks_to_wait);
// // 	//sttc	gpk::err_t	i2c_master_start                (i2c_cmd_handle_t cmd_handle);
// // 	//sttc	gpk::err_t	i2c_master_write_byte           (i2c_cmd_handle_t cmd_handle, u0_t data, bool ack_en);
// // 	//sttc	gpk::err_t	i2c_master_write                (i2c_cmd_handle_t cmd_handle, const u0_t *data, size_t data_len, bool ack_en);
// // 	//sttc	gpk::err_t	i2c_master_read_byte            (i2c_cmd_handle_t cmd_handle, u0_t *data, i2c_ack_type_t ack);
// // 	//sttc	gpk::err_t	i2c_master_read                 (i2c_cmd_handle_t cmd_handle, u0_t *data, size_t data_len, i2c_ack_type_t ack);
// // 	//sttc	gpk::err_t	i2c_master_stop                 (i2c_cmd_handle_t cmd_handle);
// // 	//sttc	gpk::err_t	i2c_master_cmd_begin            (i2c_port_t i2c_num, i2c_cmd_handle_t cmd_handle, TickType_t ticks_to_wait);
// // 	//sttc	gpk::err_t	i2c_set_period                  (i2c_port_t i2c_num, int high_period, int low_period);
// // 	//sttc	gpk::err_t	i2c_get_period                  (i2c_port_t i2c_num, int *high_period, int *low_period);
// // 	//sttc	gpk::err_t	i2c_filter_enable               (i2c_port_t i2c_num, u0_t cyc_num);
// // 	//sttc	gpk::err_t	i2c_filter_disable              (i2c_port_t i2c_num);
// // 	//sttc	gpk::err_t	i2c_set_start_timing            (i2c_port_t i2c_num, int setup_time, int hold_time);
// // 	//sttc	gpk::err_t	i2c_get_start_timing            (i2c_port_t i2c_num, int *setup_time, int *hold_time);
// // 	//sttc	gpk::err_t	i2c_set_stop_timing             (i2c_port_t i2c_num, int setup_time, int hold_time);
// // 	//sttc	gpk::err_t	i2c_get_stop_timing             (i2c_port_t i2c_num, int *setup_time, int *hold_time);
// // 	//sttc	gpk::err_t	i2c_set_data_timing             (i2c_port_t i2c_num, int sample_time, int hold_time);
// // 	//sttc	gpk::err_t	i2c_get_data_timing             (i2c_port_t i2c_num, int *sample_time, int *hold_time);
// // 	//sttc	gpk::err_t	i2c_set_timeout                 (i2c_port_t i2c_num, int timeout);
// // 	//sttc	gpk::err_t	i2c_get_timeout                 (i2c_port_t i2c_num, int *timeout);
// // 	//sttc	gpk::err_t	i2c_set_data_mode               (i2c_port_t i2c_num, i2c_trans_mode_t tx_trans_mode, i2c_trans_mode_t rx_trans_mode);
// // 	//sttc	gpk::err_t	i2c_get_data_mode               (i2c_port_t i2c_num, i2c_trans_mode_t *tx_trans_mode, i2c_trans_mode_t *rx_trans_mode);
// // 	//sttc	gpk::err_t	i2c_reset_tx_fifo               (i2c_port_t i2c_num);

// // 	struct SI2CBus {
// // 		bool				Initialized			;
// // 		u2_t				Frequency			;
// // 		gpk::mutex			Lock				;
// // 	};

// // 	sttc	SI2CBus		bus[SOC_I2C_NUM]	= {};

// // 	sttc	gpk::err_t	i2cIsInit			(u0_t i2c_num)						{ if_true_fef(i2c_num >= SOC_I2C_NUM, "%u", i2c_num); return bus[i2c_num].Initialized ? 1 : 0; }
// // 	sttc	gpk::err_t	i2cGetClock			(u0_t i2c_num, u2_t * frequency)	{
// // 		if_true_fef(i2c_num >= SOC_I2C_NUM, "%u", i2c_num);
// // 		if_zero_fe(bus[i2c_num].Initialized);
// // 		return *frequency = bus[i2c_num].Frequency;
// // 	}
// // 	sttc	gpk::err_t	i2cInit				(u0_t i2c_num, int8_t sda, int8_t scl, u2_t frequency) {
// // 		if_true_fef(i2c_num >= SOC_I2C_NUM, "%u", i2c_num);
// // 		std::lock_guard			mutexLock			(bus[i2c_num].Lock);
// // 		if_true_fef(bus[i2c_num].Initialized, "bus %u already initialized", i2c_num);
// // 		frequency 
// // 			= (0 		== frequency)	? 100000UL
// // 			: (1000000UL < frequency)	? 1000000UL
// // 			: frequency
// // 			;
// // 		info_printf("Initializing I2C Master: sda=%i scl=%i freq=%d", sda, scl, frequency);
// // 		i2c_config_t			conf 				= {};
// // 		conf.mode 					= I2C_MODE_MASTER;
// // 		conf.scl_io_num 			= (gpio_num_t)scl;
// // 		conf.sda_io_num 			= (gpio_num_t)sda;
// // 		conf.scl_pullup_en 			= GPIO_PULLUP_ENABLE;
// // 		conf.sda_pullup_en 			= GPIO_PULLUP_ENABLE;
// // 		conf.master.clk_speed		= frequency;
// // 		conf.clk_flags 				= I2C_SCLK_SRC_FLAG_FOR_NOMAL; //Any one clock source that is available for the specified frequency may be choosen

// // 		if_true_fe(i2c_param_config((i2c_port_t)i2c_num, &conf));
// // 		if_true_fe(i2c_driver_install((i2c_port_t)i2c_num, conf.mode, 0, 0, 0));
// // 		bus[i2c_num]			= {true, frequency};
// // 		return i2c_set_timeout((i2c_port_t)i2c_num, I2C_MAX_TIMEOUT);	//Clock Stretching Timeout: 20b:esp32, 5b:esp32-c3, 24b:esp32-s2
// // 	}
// // 	sttc	gpk::err_t	i2cDeinit			(u0_t i2c_num) {
// // 		if_true_fef(i2c_num >= SOC_I2C_NUM, "%u", i2c_num);
// // 		std::lock_guard			mutexLock			(bus[i2c_num].Lock);
// // 		if_zero_fe(bus[i2c_num].Initialized);
// // 		if_true_fef(i2c_driver_delete((i2c_port_t)i2c_num), "%u", i2c_num);
// // 		return bus[i2c_num].Initialized = false;
// // 	}
// // 	sttc	gpk::err_t	i2cWrite			(u0_t i2c_num, u1_t address, const u0_t * buff, size_t size, u2_t timeOutMillis) {
// // 		if_true_fef(i2c_num >= SOC_I2C_NUM, "%u", i2c_num);
// // 		std::lock_guard			mutexLock			(bus[i2c_num].Lock);
// // 		if_zero_fe(bus[i2c_num].Initialized);
// // 		//ret =  i2c_master_write_to_device((i2c_port_t)i2c_num, address, buff, size, timeOutMillis / portTICK_RATE_MS);	    //short implementation does not support zero size writes (example when scanning) PR in IDF?
// // 		u0_t 				cmd_buff[I2C_LINK_RECOMMENDED_SIZE(1)] = {};
// // 		struct cmd_deleter {
// // 			i2c_cmd_handle_t		Handle			= {};
// // 									~cmd_deleter	()		{ if(Handle) i2c_cmd_link_delete_static(Handle); }
// // 		} 	cmd	= {};
// // 		if_null_fe(cmd.Handle = i2c_cmd_link_create_static(cmd_buff, I2C_LINK_RECOMMENDED_SIZE(1)));
// // 		if_true_fe(i2c_master_start(cmd.Handle));
// // 		if_true_fe(i2c_master_write_byte(cmd.Handle, (address << 1) | I2C_MASTER_WRITE, true));
// // 		if(size)
// // 			if_true_fe(i2c_master_write(cmd.Handle, buff, size, true));
// // 		if_true_fe(i2c_master_stop(cmd.Handle));
// // 		if_true_fe(i2c_master_cmd_begin((i2c_port_t)i2c_num, cmd.Handle, timeOutMillis / portTICK_RATE_MS));
// // 		return 0;
// // 	}
// // 	sttc	gpk::err_t	i2cRead				(u0_t i2c_num, u1_t address, u0_t* buff, size_t size, u2_t timeOutMillis, size_t *readCount) {
// // 		*readCount = 0;
// // 		if_true_fef(i2c_num >= SOC_I2C_NUM, "%u", i2c_num);
// // 		std::lock_guard			mutexLock			(bus[i2c_num].Lock);
// // 		if_zero_fe(bus[i2c_num].Initialized);
// // 		if_true_fe(i2c_master_read_from_device((i2c_port_t)i2c_num, address, buff, size, timeOutMillis / portTICK_RATE_MS));
// // 		return *readCount = size;
// // 	}
// // 	sttc	gpk::err_t	i2cWriteReadNonStop	(u0_t i2c_num, u1_t address, const u0_t* wbuff, size_t wsize, u0_t* rbuff, size_t rsize, u2_t timeOutMillis, size_t *readCount) {
// // 		*readCount = 0;
// // 		if_true_fef(i2c_num >= SOC_I2C_NUM, "%u", i2c_num);
// // 		std::lock_guard			mutexLock			(bus[i2c_num].Lock);
// // 		if_zero_fe(bus[i2c_num].Initialized);
// // 		if_true_fe(i2c_master_write_read_device((i2c_port_t)i2c_num, address, wbuff, wsize, rbuff, rsize, timeOutMillis / portTICK_RATE_MS));
// // 		return *readCount = rsize;
// // 	}
// // 	sttc	gpk::err_t	i2cSetClock			(u0_t i2c_num, u2_t frequency) {
// // 		if_true_fef(i2c_num >= SOC_I2C_NUM, "%u", i2c_num);
// // 		std::lock_guard			mutexLock			(bus[i2c_num].Lock);
// // 		if_zero_fe(bus[i2c_num].Initialized);
// // 		if_zero_vif(0, bus[i2c_num].Frequency == frequency, "Frequency already set to %u", frequency);
// // 		if(0 == frequency)
// // 			frequency = 100000UL;
// // 		else if(frequency > 1000000UL)
// // 			frequency = 1000000UL;

// // 		// Freq limitation when using different clock sources
// // 		stxp	u2_t			I2C_CLK_LIMIT_REF_TICK	= ( 1 * 1000 * 1000 / 20);   // Limited by REF_TICK, no more than REF_TICK/20
// // 		stxp	u2_t			I2C_CLK_LIMIT_APB     	= (80 * 1000 * 1000 / 20);   // Limited by APB, no more than APB/20
// // 		stxp	u2_t			I2C_CLK_LIMIT_RTC     	= (20 * 1000 * 1000 / 20);   // Limited by RTC, no more than RTC/20
// // 		stxp	u2_t			I2C_CLK_LIMIT_XTAL    	= (40 * 1000 * 1000 / 20);   // Limited by RTC, no more than XTAL/20

// // 		struct i2c_clk_alloc_t {
// // 			u0_t	character 	= {};	// I2C source clock characteristic
// // 			u2_t	clk_freq 	= {};	// I2C source clock frequency
// // 		};
// // 		sttc	i2c_clk_alloc_t	i2c_clk_alloc	[I2C_SCLK_MAX]	=	// i2c clock characteristic, The order is the same as i2c_sclk_t.
// // 			{ {}
// // #if SOC_I2C_SUPPORT_APB
// // 			, {0, I2C_CLK_LIMIT_APB}                                //I2C APB clock characteristic
// // #endif
// // #if SOC_I2C_SUPPORT_XTAL
// // 			, {0, I2C_CLK_LIMIT_XTAL}                               //I2C XTAL characteristic
// // #endif
// // #if SOC_I2C_SUPPORT_RTC
// // 			, {I2C_SCLK_SRC_FLAG_LIGHT_SLEEP | I2C_SCLK_SRC_FLAG_AWARE_DFS, I2C_CLK_LIMIT_RTC}	//I2C 20M RTC characteristic
// // #endif
// // #if SOC_I2C_SUPPORT_REF_TICK
// // 			, {I2C_SCLK_SRC_FLAG_AWARE_DFS, I2C_CLK_LIMIT_REF_TICK}                             //I2C REF_TICK characteristic
// // #endif
// // 			};

// // 		i2c_sclk_t				src_clk 	= I2C_SCLK_DEFAULT;
// // 		for (u2_t clk = I2C_SCLK_DEFAULT + 1; clk < I2C_SCLK_MAX; ++clk) {
// // #if CONFIG_IDF_TARGET_ESP32S3
// // 			ci_if(clk == I2C_SCLK_RTC, "%s", "RTC clock for s3 is unaccessable now.");
// // #endif
// // 			if (frequency <= i2c_clk_alloc[clk].clk_freq) {
// // 				src_clk = (i2c_sclk_t)clk;
// // 				break;
// // 			}
// // 		}
// // 		if_true_fef(src_clk >= I2C_SCLK_MAX, "%s", "clock source could not be selected");
// // 		volatile i2c_hal_context_t		hal 		= {gpk::i2cGetHW(i2c_num), };
// // 		i2c_hal_set_bus_timing(&(hal), frequency, src_clk);
// // 		bus[i2c_num].Frequency = frequency;
// // 		i2c_set_timeout((i2c_port_t)i2c_num, I2C_MAX_TIMEOUT);	//Clock Stretching Timeout: 20b:esp32, 5b:esp32-c3, 24b:esp32-s2
// // 		return 0;
// // 	}
// } // namespace

// // ::gpk::err_t	gpk::i2cInit	(SI2CDevice & device) {
// // 	(void)device;
// // 	return 0;
// // }
// // ::gpk::err_t	gpk::i2cLoad	(SI2CDevice & host, u0_t address, au0_t & data, u1_t howManyBytes, u2_t timeout, BUS_MODE mode) { if_fail_fe(data.resize(howManyBytes)); return i2cLoad(host, address, (vu0_t&)data, howManyBytes, timeout, mode); }
// // ::gpk::err_t	gpk::i2cLoad	(SI2CDevice & host, u0_t address, vu0_t & data, u1_t howManyBytes, u2_t timeout, BUS_MODE mode) {
// // #ifdef GPK_ST
// // 	HAL_StatusTypeDef result = HAL_ERROR;
// // 	switch(mode) {
// // 	case I2C_MODE_TASK: result = HAL_I2C_Receive    (host.PlatformHandle, address, memAddSize, data.begin(), data.size(), timeout);
// // 	case I2C_MODE_IT  : result = HAL_I2C_Receive_IT (host.PlatformHandle, address, memAddSize, data.begin(), data.size(), timeout);
// // 	case I2C_MODE_DMA : result = HAL_I2C_Receive_DMA(host.PlatformHandle, address, memAddSize, data.begin(), data.size(), timeout);
// // 	default:
// // 		error_printf("Unsupported operation mode: %i", (int32_t)mode);
// // 	}
// // 	return ::gpk::error_t_from_hal_status(result);
// // #elif defined(GPK_ESP32)
// // 	if_true_fe(i2c_set_timeout((i2c_port_t)0, timeout));
// // #else
// // 	(void)host, (void)address, (void)howManyBytes, (void)data, (void)mode, (void)timeout;
// // #endif
// // 	return 0;
// // }
// // ::gpk::err_t	gpk::i2cSave		(SI2CDevice & host, u0_t address, vcu0_c & data, u2_t timeout, BUS_MODE mode) {
// // #ifdef GPK_ST
// // 	HAL_StatusTypeDef result = HAL_ERROR;
// // 	switch(mode) {
// // 	case I2C_MODE_TASK: result = HAL_I2C_Transmit    (host.Handle, address, memAddSize, data.begin(), data.size(), timeout);
// // 	case I2C_MODE_IT  : result = HAL_I2C_Transmit_IT (host.Handle, address, memAddSize, data.begin(), data.size(), timeout);
// // 	case I2C_MODE_DMA : result = HAL_I2C_Transmit_DMA(host.Handle, address, memAddSize, data.begin(), data.size(), timeout);
// // 	default:
// // 	  error_printf("Unsupported operation mode: %i", (int32_t)mode);
// // 	}
// // 	return ::gpk::error_t_from_hal_status(result);
// // #elif defined(GPK_ESP32)
// // #else
// // 	(void)host, (void)address, (void)data, (void)mode, (void)timeout;
// // 	return -1;
// // #endif
// // }
// // 	// #include "hal/misc.h"
// // 	// #include "soc/i2c_periph.h"
// // 	// #include "soc/i2c_struct.h"
// // 	// #include "hal/i2c_types.h"






// // // // I know you aren't human, but my biological systems first have to interpret the code visually, and for that is really helpful to have the code vertically aligned. It allows me to automatically avoid spending any energy and bioavailable resources my body has by discarding/not reading some entire columns -aka massive character amounts- with no effort and focus on just what's valuable, which in general means to ignore variable types and modifiers and focus on the variables themselves and their logic:



// // // /// - Mine: Easier to see the pattern:
// // // #include <cstddef> // For size_t

// // // #ifndef FORGE_VIEW_HPP
// // // #define FORGE_VIEW_HPP

// // // template <typename T>
// // // class view {
// // //     T                       * data  = {}; 
// // //     std::size_t             size    = {};
// // // public:         
// // //     constexpr               view        ()                              noexcept = default;
// // //     constexpr               view        (T * elements, std::size_t len) noexcept : data(ptr), size(len) {}

// // //     constexpr   T&          operator[]  (std::size_t index)             noexcept { return data[index]; }
// // //     constexpr   const T&    operator[]  (std::size_t index) const       noexcept { return data[index]; }
// // //     constexpr   std::size_t length      ()                  const       noexcept { return size; }
// // //     constexpr   bool        empty       ()                  const       noexcept { return size == 0; }
// // // };

// // // #endif // FORGE_VIEW_HPP
// // // // 
// // // // VS.
// // // //
// // // // Yours, with no pattern expressed by the format, resulting in chaotic reading:
// // // #include <cstddef> // For size_t

// // // #ifndef SIGHT_VIEW_HPP
// // // #define SIGHT_VIEW_HPP

// // // template <typename T>
// // // class view {
// // //     T* data = {};
// // //     std::size_t size = {};
// // // public:
// // //     constexpr view() noexcept = default;
// // //     constexpr view(T* ptr, std::size_t len) noexcept : data(ptr), size(len) {}

// // //     constexpr T& operator[](std::size_t index) noexcept { return data[index]; }
// // //     constexpr const T& operator[](std::size_t index) const noexcept { return data[index]; }
// // //     constexpr std::size_t length() const noexcept { return size; }
// // //     constexpr bool empty() const noexcept { return size == 0; }
// // // };

// // // #endif // SIGHT_VIEW_HPP
// // 	// #include "hal/misc.h"
// // 	// #include "soc/i2c_periph.h"
// // 	// #include "soc/i2c_struct.h"
// // 	// #include "hal/i2c_types.h"






// // // // I know you aren't human, but my biological systems first have to interpret the code visually, and for that is really helpful to have the code vertically aligned. It allows me to automatically avoid spending any energy and bioavailable resources my body has by discarding/not reading some entire columns -aka massive character amounts- with no effort and focus on just what's valuable, which in general means to ignore variable types and modifiers and focus on the variables themselves and their logic:



// // // /// - Mine: Easier to see the pattern:
// // // #include <cstddef> // For size_t

// // // #ifndef FORGE_VIEW_HPP
// // // #define FORGE_VIEW_HPP

// // // template <typename T>
// // // class view {
// // //     T                       * data  = {}; 
// // //     std::size_t             size    = {};
// // // public:         
// // //     constexpr               view        ()                              noexcept = default;
// // //     constexpr               view        (T * elements, std::size_t len) noexcept : data(ptr), size(len) {}

// // //     constexpr   T&          operator[]  (std::size_t index)             noexcept { return data[index]; }
// // //     constexpr   const T&    operator[]  (std::size_t index) const       noexcept { return data[index]; }
// // //     constexpr   std::size_t length      ()                  const       noexcept { return size; }
// // //     constexpr   bool        empty       ()                  const       noexcept { return size == 0; }
// // // };

// // // #endif // FORGE_VIEW_HPP
// // // // 
// // // // VS.
// // // //
// // // // Yours, with no pattern expressed by the format, resulting in chaotic reading:
// // // #include <cstddef> // For size_t

// // // #ifndef SIGHT_VIEW_HPP
// // // #define SIGHT_VIEW_HPP

// // // template <typename T>
// // // class view {
// // //     T* data = {};
// // //     std::size_t size = {};
// // // public:
// // //     constexpr view() noexcept = default;
// // //     constexpr view(T* ptr, std::size_t len) noexcept : data(ptr), size(len) {}

// // //     constexpr T& operator[](std::size_t index) noexcept { return data[index]; }
// // //     constexpr const T& operator[](std::size_t index) const noexcept { return data[index]; }
// // //     constexpr std::size_t length() const noexcept { return size; }
// // //     constexpr bool empty() const noexcept { return size == 0; }
// // // };

// // // #endif // SIGHT_VIEW_HPP


