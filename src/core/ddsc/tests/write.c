/*
 * Copyright(c) 2006 to 2018 ADLINK Technology Limited and others
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v. 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0, or the Eclipse Distribution License
 * v. 1.0 which is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause
 */
#include <stdio.h>
#include "CUnit/Test.h"
#include "CUnit/Theory.h"
#include "ddsc/dds.h"
#include "RoundTrip.h"
#include "Space.h"
#include "os/os.h"

/* Tests in this file only concern themselves with very basic api tests of
   dds_write and dds_write_ts */

static const uint32_t payloadSize = 32;
static RoundTripModule_DataType data, data2;

static dds_entity_t participant = 0;
static dds_entity_t topic = 0;
static dds_entity_t publisher = 0;
static dds_entity_t writer = 0;

static void
setup(void)
{
    participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    CU_ASSERT_FATAL (participant > 0);
    topic = dds_create_topic (participant, &RoundTripModule_DataType_desc, "RoundTrip", NULL, NULL);
    CU_ASSERT_FATAL (topic > 0);
    publisher = dds_create_publisher (participant, NULL, NULL);
    CU_ASSERT_FATAL (publisher > 0);
    writer = dds_create_writer (participant, topic, NULL, NULL);
    CU_ASSERT_FATAL (writer > 0);

    memset (&data, 0, sizeof(data));
    data.payload._length = payloadSize;
    data.payload._buffer = dds_alloc (payloadSize);
    memset (data.payload._buffer, 'a', payloadSize);
    data.payload._release = true;
    data.payload._maximum = 0;


    memset (&data2, 0, sizeof(data2));
    data2.payload._length = payloadSize;
    data2.payload._buffer = dds_alloc (payloadSize);
    memset (data2.payload._buffer, 'a', payloadSize);
    data2.payload._release = true;
    data2.payload._maximum = 0;
}

static void
teardown(void)
{
    RoundTripModule_DataType_free (&data, DDS_FREE_CONTENTS);
    memset (&data, 0, sizeof(data));

    RoundTripModule_DataType_free (&data2, DDS_FREE_CONTENTS);
    memset (&data2, 0, sizeof(data2));

    dds_delete (writer);
    dds_delete (publisher);
    dds_delete (topic);
    dds_delete (participant);
}

CU_Test(ddsc_write, basic, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (writer, &data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);
}

CU_Test(ddsc_write_ih, basic, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (writer, &data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);
    status = dds_write_ih (writer, &data2, handle);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);
}

CU_Test(ddsc_write, null_writer, .init = setup, .fini = teardown)
{
    dds_return_t status;

    /* Disable warning related to improper API usage by passing incompatible parameter. */
    OS_WARNING_MSVC_OFF (28020);
    status = dds_write (0, &data);
    OS_WARNING_MSVC_ON (28020);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write_ih, null_writer, .init = setup, .fini = teardown)
{
    dds_return_t status;

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    /* Disable warning related to improper API usage by passing incompatible parameter. */
    OS_WARNING_MSVC_OFF (28020);
    status = dds_write_ih (0, &data, handle);
    OS_WARNING_MSVC_ON (28020);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write_ih_ts, null_writer, .init = setup, .fini = teardown)
{
    dds_return_t status;

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    /* Disable warning related to improper API usage by passing incompatible parameter. */
    OS_WARNING_MSVC_OFF (28020);
    status = dds_write_ih_ts (0, &data, handle, dds_time());
    OS_WARNING_MSVC_ON (28020);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write_ih, bad_handle, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write_ih (writer, &data2, DDS_HANDLE_NIL);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write_ih_ts, bad_handle, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write_ih_ts(writer, &data2, DDS_HANDLE_NIL, dds_time());
    CU_ASSERT_EQUAL_FATAL(dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write, bad_writer, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (publisher, &data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_ILLEGAL_OPERATION);
}

CU_Test(ddsc_write_ih, bad_writer, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (writer, &data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance( writer, (void*) &data);

    status = dds_write_ih (publisher, &data, handle);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_ILLEGAL_OPERATION);
}

CU_Test(ddsc_write_ih_ts, bad_writer, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (writer, &data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    status = dds_write_ih_ts (publisher, &data, handle, dds_time());
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_ILLEGAL_OPERATION);
}

CU_Test(ddsc_write_ih, wrong_handle, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (writer, &data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    dds_entity_t top;
    top = dds_create_topic (participant, &Space_simpletypes_desc, "SimpleTypes", NULL, NULL);
    dds_entity_t writer2 = dds_create_writer (publisher, top, NULL, NULL);

    const Space_simpletypes st_data = {
            .l = -1,
            .ll = -1,
            .us = 1,
            .ul = 1,
            .ull = 1,
            .f = 1.0f,
            .d = 1.0f,
            .c = '1',
            .b = true,
            .o = 1,
            .s = "Keyvalue1"
    };

    status = dds_write_ih (writer2, &st_data, handle);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);

    dds_delete (top);
    dds_delete (writer2);
}

CU_Test(ddsc_write_ih_ts, wrong_handle, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (writer, &data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    dds_entity_t top;
    top = dds_create_topic (participant, &Space_simpletypes_desc, "SimpleTypes", NULL, NULL);
    dds_entity_t writer2 = dds_create_writer (publisher, top, NULL, NULL);

    const Space_simpletypes st_data = {
            .l = -1,
            .ll = -1,
            .us = 1,
            .ul = 1,
            .ull = 1,
            .f = 1.0f,
            .d = 1.0f,
            .c = '1',
            .b = true,
            .o = 1,
            .s = "Keyvalue1"
    };

    status = dds_write_ih_ts (writer2, &st_data, handle, dds_time());
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);

    dds_delete (top);
    dds_delete (writer2);
}

CU_Test(ddsc_write, closed_writer, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_delete (writer);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);
    status = dds_write (writer, &data);
    writer = 0;
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_ALREADY_DELETED);
}

CU_Test(ddsc_write_ih, closed_writer, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write(writer, &data);
    dds_instance_handle_t handle;
    handle = dds_lookup_instance(writer, (void*) &data);

    status = dds_delete(writer);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    status = dds_write_ih(writer, &data2, handle);
    writer = 0;
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write_ih_ts, closed_writer, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (writer, &data);
    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    status = dds_delete (writer);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    status = dds_write_ih_ts (writer, &data2, handle, dds_time());
    writer = 0;
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write, null_sample, .init = setup, .fini = teardown)
{
    dds_return_t status;

    /* Disable warning related to improper API usage by passing NULL to a non-NULL parameter. */
    OS_WARNING_MSVC_OFF ( 6387);
    status = dds_write (writer, NULL);
    OS_WARNING_MSVC_ON (6387);

    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write_ih, null_sample, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (writer, &data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    /* Disable warning related to improper API usage by passing NULL to a non-NULL parameter. */
    OS_WARNING_MSVC_OFF (6387);
    status = dds_write_ih (writer, NULL, handle);
    OS_WARNING_MSVC_ON (6387);

    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write_ih_ts, null_sample, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write (writer, &data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    /* Disable warning related to improper API usage by passing NULL to a non-NULL parameter. */
    OS_WARNING_MSVC_OFF (6387);
    status = dds_write_ih_ts (writer, NULL, handle, dds_time());
    OS_WARNING_MSVC_ON (6387);

    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write_ts, basic, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write_ts (writer, &data, dds_time());
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

}

CU_Test(ddsc_write_ih_ts, basic, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write_ts (writer, &data, dds_time());
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    status = dds_write_ih_ts (writer, &data2, handle, dds_time());
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

}

CU_Test(ddsc_write_ts, bad_timestamp, .init = setup, .fini = teardown)
{
    dds_return_t status;

    status = dds_write_ts (writer, &data, -1);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write_ih_ts, bad_timestamp, .init = setup, .fini = teardown)
{
    dds_return_t status;

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (writer, (void*) &data);

    status = dds_write_ih_ts (writer, &data, handle, -1);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_BAD_PARAMETER);
}

CU_Test(ddsc_write, simpletypes)
{
    dds_return_t status;
    dds_entity_t par, top, wri;
    const Space_simpletypes st_data = {
        .l = -1,
        .ll = -1,
        .us = 1,
        .ul = 1,
        .ull = 1,
        .f = 1.0f,
        .d = 1.0f,
        .c = '1',
        .b = true,
        .o = 1,
        .s = "This string is exactly so long that it would previously trigger CHAM-405. If this string is shortened exactly one character, all is well. Since it is fixed now, there doesn't need to be any further investigation."
    };

    par = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    CU_ASSERT_FATAL (par > 0);
    top = dds_create_topic (par, &Space_simpletypes_desc, "SimpleTypes", NULL, NULL);
    CU_ASSERT_FATAL (top > 0);
    wri = dds_create_writer (par, top, NULL, NULL);
    CU_ASSERT_FATAL (wri > 0);

    status = dds_write (wri, &st_data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_delete (wri);
    dds_delete (top);
    dds_delete (par);
}


CU_Test(ddsc_write_ih, simpletypes)
{
    dds_return_t status;
    dds_entity_t par, top, wri;
    const Space_simpletypes st_data = {
        .l = -1,
        .ll = -1,
        .us = 1,
        .ul = 1,
        .ull = 1,
        .f = 1.0f,
        .d = 1.0f,
        .c = '1',
        .b = true,
        .o = 1,
        .s = "Keyvalue1"
    };

    const Space_simpletypes st_data2 = {
            .l = -2,
            .ll = -2,
            .us = 2,
            .ul = 2,
            .ull = 2,
            .f = 2.0f,
            .d = 2.0f,
            .c = '2',
            .b = true,
            .o = 2,
            .s = "Keyvalue1"
        };

    par = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    CU_ASSERT_FATAL (par > 0);
    top = dds_create_topic (par, &Space_simpletypes_desc, "SimpleTypes", NULL, NULL);
    CU_ASSERT_FATAL (top > 0);
    wri = dds_create_writer (par, top, NULL, NULL);
    CU_ASSERT_FATAL (wri > 0);

    status = dds_write (wri, &st_data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (wri, (void*) &st_data);

    status = dds_write_ih (wri, &st_data2, handle);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_delete (wri);
    dds_delete (top);
    dds_delete (par);
}

CU_Test(ddsc_write_ih_ts, simpletypes)
{
    dds_return_t status;
    dds_entity_t par, top, wri;
    const Space_simpletypes st_data = {
        .l = -1,
        .ll = -1,
        .us = 1,
        .ul = 1,
        .ull = 1,
        .f = 1.0f,
        .d = 1.0f,
        .c = '1',
        .b = true,
        .o = 1,
        .s = "Keyvalue1"
    };

    const Space_simpletypes st_data2 = {
            .l = -2,
            .ll = -2,
            .us = 2,
            .ul = 2,
            .ull = 2,
            .f = 2.0f,
            .d = 2.0f,
            .c = '2',
            .b = true,
            .o = 2,
            .s = "Keyvalue1"
        };

    par = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
    CU_ASSERT_FATAL (par > 0);
    top = dds_create_topic (par, &Space_simpletypes_desc, "SimpleTypes", NULL, NULL);
    CU_ASSERT_FATAL (top > 0);
    wri = dds_create_writer (par, top, NULL, NULL);
    CU_ASSERT_FATAL (wri > 0);

    status = dds_write (wri, &st_data);
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_instance_handle_t handle;
    handle = dds_lookup_instance (wri, (void*) &st_data);

    status = dds_write_ih_ts (wri, &st_data2, handle, dds_time());
    CU_ASSERT_EQUAL_FATAL (dds_err_nr(status), DDS_RETCODE_OK);

    dds_delete (wri);
    dds_delete (top);
    dds_delete (par);
}
