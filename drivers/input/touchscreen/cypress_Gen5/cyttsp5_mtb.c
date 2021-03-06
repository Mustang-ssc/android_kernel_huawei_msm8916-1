/*
 * cyttsp5_mtb.c
 * Cypress TrueTouch(TM) Standard Product V5 Multi-touch module.
 * For use with Cypress Txx5xx parts.
 * Supported parts include:
 * TMA5XX
 *
 * Copyright (C) 2012-2013 Cypress Semiconductor
 * Copyright (C) 2011 Sony Ericsson Mobile Communications AB.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, and only version 2, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Contact Cypress Semiconductor at www.cypress.com <ttdrivers@cypress.com>
 *
 */

#include <linux/input.h>
#include <linux/input/mt.h>

#include "cyttsp5_core.h"
#include "cyttsp5_mt_common.h"

//extern int get_boot_into_recovery_flag(void);

static void cyttsp5_final_sync(struct input_dev *input, int max_slots,
		int mt_sync_count, unsigned long *ids)
{
	int t;

	for (t = 0; t < max_slots; t++) {
		if (test_bit(t, ids))
			continue;
		input_mt_slot(input, t);
		input_mt_report_slot_state(input, MT_TOOL_FINGER, false);
	}

	input_sync(input);
}

static void cyttsp5_input_report(struct input_dev *input, int sig,
		int t, int type)
{
	input_mt_slot(input, t);

	if (type == CY_OBJ_STANDARD_FINGER || type == CY_OBJ_GLOVE
			|| type == CY_OBJ_HOVER)
		input_mt_report_slot_state(input, MT_TOOL_FINGER, true);
	else if (type == CY_OBJ_STYLUS)
		input_mt_report_slot_state(input, MT_TOOL_PEN, true);
}

static void cyttsp5_report_slot_liftoff(struct cyttsp5_mt_data *md,
		int max_slots)
{
	int t;

	if (md->num_prv_tch == 0)
		return;

	for (t = 0; t < max_slots; t++) {
		/* Output t, max_slots */
		TS_LOG_DEBUG( "%s,t = %d, max_slots=%d\n", __func__,t, max_slots);
		input_mt_slot(md->input, t);
		input_mt_report_slot_state(md->input,
			MT_TOOL_FINGER, false);
	}
}

static int cyttsp5_input_register_device(struct input_dev *input,
		int max_slots)
{
	input_mt_init_slots(input, max_slots, 0);
	return input_register_device(input);
}

void cyttsp5_init_function_ptrs(struct cyttsp5_mt_data *md)
{
	md->mt_function.report_slot_liftoff = cyttsp5_report_slot_liftoff;
	md->mt_function.final_sync = cyttsp5_final_sync;
	md->mt_function.input_sync = NULL;
	md->mt_function.input_report = cyttsp5_input_report;
	md->mt_function.input_register_device = cyttsp5_input_register_device;
}

static int __init cyttsp5_mt_init(void)
{
	int rc;
/*
	if(get_boot_into_recovery_flag())
		return 0;
*/
	cyttsp5_mt_driver.driver.owner = THIS_MODULE;
	rc = cyttsp5_register_driver(&cyttsp5_mt_driver);
	TS_LOG_INFO("%s: Cypress TTSP MT v5 multi-touch (Built %s), rc=%d\n",
		 __func__, CY_DRIVER_DATE, rc);
	return rc;
}
module_init(cyttsp5_mt_init);

static void __exit cyttsp5_mt_exit(void)
{
	cyttsp5_unregister_driver(&cyttsp5_mt_driver);
}
module_exit(cyttsp5_mt_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Cypress TrueTouch(R) Standard 2D multi-touch driver");
MODULE_AUTHOR("Cypress Semiconductor");
