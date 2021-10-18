#include "BatteryStatus.h"
#include "assert.h"
#include "util.hpp"
uint32_t serialize_int64(int64_t number, uint8_t *buffer, uint32_t buffer_size) {
    return Util::serialize_int<int64_t>(number, buffer, buffer_size);
}

int64_t deserialize_int64(const uint8_t *buffer) {
    return Util::deserialize_int<int64_t>(buffer);
}

uint32_t BatteryStatus_serialize(const struct BatteryStatus *status, uint8_t *buffer, uint32_t buffer_size) {
    uint32_t remaining_buffer_size = buffer_size;
    uint32_t used_bytes = 0;

    // uint8_t *b = buffer;
    
    if (buffer_size < 48) { 
        return 0; 
    }
    
    used_bytes = serialize_int64(status->voltage_mV, buffer, remaining_buffer_size);
    remaining_buffer_size -= used_bytes;
    buffer += used_bytes;

    used_bytes = serialize_int64(status->current_mA, buffer, remaining_buffer_size);
    remaining_buffer_size -= used_bytes;
    buffer += used_bytes;

    used_bytes = serialize_int64(status->state_of_charge_mAh, buffer, remaining_buffer_size);
    remaining_buffer_size -= used_bytes;
    buffer += used_bytes;

    used_bytes = serialize_int64(status->max_capacity_mAh, buffer, remaining_buffer_size);
    remaining_buffer_size -= used_bytes;
    buffer += used_bytes;

    used_bytes = serialize_int64(status->max_charging_current_mA, buffer, remaining_buffer_size);
    remaining_buffer_size -= used_bytes;
    buffer += used_bytes;

    used_bytes = serialize_int64(status->max_discharging_current_mA, buffer, remaining_buffer_size);
    remaining_buffer_size -= used_bytes;
    buffer += used_bytes;

    // print_buffer(b, 48);

    return buffer_size - remaining_buffer_size;
}


uint32_t BatteryStatus_deserialize(struct BatteryStatus *status, const uint8_t *buffer, uint32_t buffer_size) {
    if (buffer_size < 48) {
        return 0;
    }
    
    status->voltage_mV                 = deserialize_int64(buffer + (sizeof(int64_t) * 0));
    status->current_mA                 = deserialize_int64(buffer + (sizeof(int64_t) * 1));
    status->state_of_charge_mAh        = deserialize_int64(buffer + (sizeof(int64_t) * 2));
    status->max_capacity_mAh           = deserialize_int64(buffer + (sizeof(int64_t) * 3));
    status->max_charging_current_mA    = deserialize_int64(buffer + (sizeof(int64_t) * 4));
    status->max_discharging_current_mA = deserialize_int64(buffer + (sizeof(int64_t) * 5));

    return sizeof(int64_t) * 6;
}


int32_t BatteryStatus_compare(const BatteryStatus *a, const BatteryStatus *b) {
    return (a->voltage_mV == b->voltage_mV) && 
        (a->current_mA == b->current_mA) && 
        (a->state_of_charge_mAh == b->state_of_charge_mAh) && 
        (a->max_capacity_mAh == b->max_capacity_mAh) && 
        (a->max_charging_current_mA == b->max_charging_current_mA) && 
        (a->max_discharging_current_mA == b->max_discharging_current_mA);
}

void print_buffer(uint8_t *buffer, uint32_t buffer_size) {
    Util::print_buffer(buffer, buffer_size); 
}


void test_battery_status() {
    // alternating bits: 5 = b'0101, A = b'1010
    struct BatteryStatus status;
    
    status.voltage_mV = 0xA5A5A5A5;
    status.current_mA = 0x5A5A5A5A;
    status.state_of_charge_mAh = 0xA5A5A5A5;
    status.max_capacity_mAh = 0x5A5A5A5A;
    status.max_charging_current_mA = 0xA5A5A5A5;
    status.max_discharging_current_mA = 0x5A5A5A5A;

    uint8_t buffer[48];
    uint32_t buffer_size = 48;

    struct BatteryStatus status2;

    if (BatteryStatus_serialize(&status, buffer, buffer_size) != 48) exit(1);

    printf("Buffer: ");
    print_buffer(buffer, buffer_size);

    if (BatteryStatus_deserialize(&status2, buffer, buffer_size) != 48) exit(1);
    
    
    printf("\nIs equal: %d\n", (int)BatteryStatus_compare(&status, &status2));


    if (BatteryStatus_serialize(&status2, buffer, buffer_size) != 48) exit(1);
    printf("Buffer: ");
    print_buffer(buffer, buffer_size);
    printf("\n");

    struct BatteryStatus status3;
    status3.voltage_mV = 0x12345678;
    status3.current_mA = 0x9ABCEF12;
    status3.state_of_charge_mAh = 0xDEADBEEF;
    status3.max_capacity_mAh = 0xFEEBDAED;
    status3.max_charging_current_mA = 0x98765432;
    status3.max_discharging_current_mA = 0xABCD5678;


    if (BatteryStatus_serialize(&status3, buffer, buffer_size) != 48) exit(1);

    if (BatteryStatus_deserialize(&status2, buffer, buffer_size) != 48) exit(1);
    printf("Buffer: ");
    print_buffer(buffer, buffer_size);
    printf("\nIs equal: %d\n", (int)BatteryStatus_compare(&status3, &status2));

    if (BatteryStatus_serialize(&status2, buffer, buffer_size) != 48) exit(1);
    printf("Buffer: ");
    print_buffer(buffer, buffer_size);
    printf("\n");

    printf("sizeof BatteryStatus = %lu\n", (unsigned long)sizeof(BatteryStatus));

}

















