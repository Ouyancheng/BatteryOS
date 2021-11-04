#ifndef SPLITTED_BATTERY_HPP
#define SPLITTED_BATTERY_HPP
#include "BatteryInterface.hpp"
#include "SplitterPolicy.hpp"

class SplittedBattery : public VirtualBattery {
    std::string policy_name;
    BOSDirectory *pdirectory;
public: 
    SplittedBattery(
        const std::string &name, 
        const std::chrono::milliseconds &sample_period, 
        const std::string &policy_name, 
        BOSDirectory &directory
    ) : 
        VirtualBattery(name, sample_period), 
        policy_name(policy_name)
    {
        this->type = BatteryType::SPLITTED;
        this->pdirectory = &directory;
    }
protected: 
    BatteryStatus refresh() override {
        Battery *policy_battery = pdirectory->get_battery(this->policy_name);
        if (policy_battery->get_battery_type() != BatteryType::SPLIT_POLICY) {
            WARNING() << ("the policy is not a policy");
            return this->status;
        }
        // just cast it to the policy pointer 
        SplitterPolicy *sp = dynamic_cast<SplitterPolicy*>(policy_battery);
        BatteryStatus status = sp->get_status_of(this->name);
        this->status = status;
        return this->status;
    }

    uint32_t set_current(int64_t target_current_mA, bool is_greater_than_target) override {
        // nothing here
        return 0;
    }
public: 
    // BatteryStatus get_status() override {
    //     // always refresh?? Or use the default one... 
    //     return this->status;
    // }

    uint32_t schedule_set_current(int64_t target_current_mA, bool is_greater_than_target, timepoint_t when_to_set, timepoint_t until_when) override {
        // forward this to the Policy
        Battery *policy_battery = pdirectory->get_battery(this->policy_name);
        if (policy_battery->get_battery_type() != BatteryType::SPLIT_POLICY) {
            WARNING() << "the policy is not a policy";
            return 0;
        }
        SplitterPolicy *sp = dynamic_cast<SplitterPolicy*>(policy_battery);
        return sp->schedule_set_current_of(this->name, target_current_mA, is_greater_than_target, when_to_set, until_when);
    }


};

#endif // ! SPLITTED_BATTERY_HPP 































