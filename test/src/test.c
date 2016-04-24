#include "testfw.h"
#include <segment.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

TEST(test_write_read) {
    const size_t size = 10485760; // 10 MB
    const const char* dir = "/tmp/test_write_read";

    segment_t* sgm = segment_open(dir, 0, size);
    ASSERT(sgm);

    const char* str = "Lorem ipsum dolor sit amet, etc ...";
    const size_t str_size = strlen(str);
    ssize_t written = segment_write(sgm, str, str_size);
    ASSERT(str_size == (size_t)written);

    const char* str2 = "what's up?";
    const size_t str2_size = strlen(str2);
    written = segment_write(sgm, str2, str2_size);
    ASSERT(str2_size == (size_t)written);

    uint64_t offset = 0;
    struct frame fr;

    int rc = segment_read(sgm, offset, &fr);
    ASSERT(rc == 0);

    offset += fr.hdr->size;
    size_t payload_size = frame_payload_size(&fr);
    ASSERT(payload_size == str_size);
    ASSERT(strncmp((const char*)fr.buffer, str, payload_size) == 0);

    rc = segment_read(sgm, offset, &fr);
    ASSERT(rc == 0);

    offset += fr.hdr->size;
    payload_size = frame_payload_size(&fr);
    ASSERT(payload_size == str2_size);
    ASSERT(strncmp((const char*)fr.buffer, str2, payload_size) == 0);

    ASSERT(segment_close(sgm) == 0);

    ASSERT(segment_destroy(dir) == 0);
}

TEST(test_write_close_open_read) {
    const size_t size = 10485760; // 10 MB
    const const char* dir = "/tmp/test_write_close_open_read";

    segment_t* sgm = segment_open(dir, 0, size);
    ASSERT(sgm);

    const int n = 14434;
    const size_t n_size = sizeof(n);
    ssize_t written = segment_write(sgm, &n, n_size);
    ASSERT(n_size == (size_t)written);

    const double d = 45435.2445;
    const size_t d_size = sizeof(d);
    written = segment_write(sgm, &d, d_size);
    ASSERT(d_size == (size_t)written);

    ASSERT(segment_close(sgm) == 0);

    sgm = segment_open(dir, 0, size);
    ASSERT(sgm);

    uint64_t offset = 0;
    struct frame fr;

    int rc = segment_read(sgm, offset, &fr);
    ASSERT(rc == 0);

    offset += fr.hdr->size;
    size_t payload_size = frame_payload_size(&fr);
    ASSERT(payload_size == n_size);
    int n_read = *(int*)fr.buffer;
    ASSERT(n == n_read);

    rc = segment_read(sgm, offset, &fr);
    ASSERT(rc == 0);
    payload_size = frame_payload_size(&fr);
    ASSERT(payload_size == d_size);
    double d_read = *(double*)fr.buffer;
    ASSERT(d == d_read);

    offset += fr.hdr->size;

    ASSERT(segment_close(sgm) == 0);

    ASSERT(segment_destroy(dir) == 0);
}
