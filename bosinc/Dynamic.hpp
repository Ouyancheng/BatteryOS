#ifndef DYNAMIC_HPP
#define DYNAMIC_HPP

#include "BatteryInterface.hpp"
#include <dlfcn.h>

class DynamicBattery : public Battery {
public: 
    /// init_func: init_argument -> init_result
    typedef void*(*init_func_t)(void*);
    /// destruct_func: init_result -> void
    typedef void(*destruct_func_t)(void*);
    /// get_status: init_result -> BatteryStatus
    typedef BatteryStatus(*get_status_func_t)(void*);
    /// set_current: (init_result, target_current_mA) -> some returnvalue
    typedef uint32_t(*set_current_func_t)(void*, int64_t);
    /// get_delay: (init_result, from_mA, to_mA) -> delay_ms
    typedef int64_t(*get_delay_func_t)(void*, int64_t, int64_t);
    static int64_t no_delay(void* dummy, int64_t from, int64_t to) {
        return 0;
    }
private: 
    static std::map<std::string, void*> loaded_dlls;
    void *dll_handle;
    init_func_t        init_func;
    destruct_func_t    destruct_func;
    get_status_func_t  get_status_func;
    set_current_func_t set_current_func;
    get_delay_func_t   get_delay_func;
    bool initialized;
    void *init_result;
public: 
    DynamicBattery(
        const std::string &dll_path, 
        const std::string &init_func_name, 
        const std::string &destruct_func_name, 
        const std::string &get_status_func_name, 
        const std::string &set_current_func_name,
        const std::string &get_delay_func_name,
        void *init_argument) : 
        init_func(0), 
        destruct_func(0), 
        get_status_func(0), 
        set_current_func(0), 
        get_delay_func(0),
        initialized(false),
        init_result(0)
    {
        if (loaded_dlls.find(dll_path) != loaded_dlls.end()) {
            this->dll_handle = loaded_dlls[dll_path];
        } 
        else {
            this->dll_handle = dlopen(dll_path.c_str(), RTLD_LAZY);
            if (!this->dll_handle) {
                WARNING() << "DLL not opened! Error: " << dlerror();
                this->dll_handle = 0;
                return;
            } 
        }
        bool failed = false;
        this->init_func = dlsym(this->dll_handle, init_func_name.c_str());
        if (!init_func) { WARNING() << "init_func not found! init_func = " << init_func_name << " Error: " << dlerror(); failed = true; }
        this->destruct_func = dlsym(this->dll_handle, destruct_func_name.c_str());
        if (!destruct_func) { WARNING() << "destruct_func not found! destruct_func = " << destruct_func_name << " Error: " << dlerror(); failed = true; }
        this->get_status_func = dlsym(this->dll_handle, get_status_func_name.c_str());
        if (!get_status_func) { WARNING() << "get_status_func not found! get_status_func = " << get_status_func_name << " Error: " << dlerror(); failed = true; }
        this->set_current_func = dlsym(this->dll_handle, set_current_func_name.c_str());
        if (!set_current_func) { WARNING() << "set_current_func not found! set_current_func = " << set_current_func_name << " Error: " << dlerror(); failed = true; }
        this->get_delay_func = dlsym(this->dll_handle, get_delay_func_name.c_str());
        if (!get_delay_func) {
            this->get_delay_func = &no_delay;
        }

        if (failed) {
            WARNING() << "DLL failed to find required symbols!";
            init_func = destruct_func = get_status_func = set_current_func = 0;
            return; 
        } 
        
        loaded_dlls[dll_path] = this->handle;
        this->initialized = true;

        init_result = this->init_func(init_argument);
    }
    ~DynamicBattery() {
        if (this->initialized) {
            this->destruct_func(this->init_result);
        }
    }
    bool is_initialized() {
        return this->initialized;
    }
protected: 
    BatteryStatus refresh() override {
        if (!this->initialized) {
            return BatteryStatus();
        }
        return this->get_status_func(this->init_result);
    }
    uint32_t set_current(int64_t new_current_mA, bool is_greater_than_target, void *other_data) override {
        if (!this->initialized) {
            return -1;
        }
        return this->set_current_func(this->init_result, new_current_mA);
    }
    std::chrono::milliseconds get_delay(int64_t from_mA, int64_t to_mA) {
        int64_t delay_ms = this->get_delay_func(this->init_result, from_mA, to_mA);
        return std::chrono::milliseconds(delay_ms);
    }
}; 

#endif // ! DYNAMIC_HPP







