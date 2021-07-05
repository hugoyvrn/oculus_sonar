#ifndef _DEF_NARVAL_OCULUS_SONAR_DRIVER_H_
#define _DEF_NARVAL_OCULUS_SONAR_DRIVER_H_

#include <narval_oculus/Oculus.h>
#include <narval_oculus/utils.h>
#include <narval_oculus/print_utils.h>
#include <narval_oculus/CallbackQueue.h>
#include <narval_oculus/SonarClient.h>

namespace narval { namespace oculus {

class SonarDriver : public SonarClient
{
    public:

    using PingConfig    = OculusSimpleFireMessage;
    using PingResult    = OculusSimplePingResult;
    using PingCallbacks = CallbackQueue<const PingResult&,
                                        const std::vector<uint8_t>&>; 
    using DummyCallbacks   = CallbackQueue<const OculusMessageHeader&>;
    using MessageCallbacks = CallbackQueue<const OculusMessageHeader&,
                                           const std::vector<uint8_t>&>;

    protected:

    bool         isStandingBy_;
    PingConfig   currentConfig_;
    PingRateType lastPingRate_;

    PingCallbacks    pingCallbacks_;
    DummyCallbacks   dummyCallbacks_;
    MessageCallbacks messageCallbacks_; // will be called on every received message.

    public:

    SonarDriver(boost::asio::io_service& service,
                const Duration& checkerPeriod = boost::posix_time::seconds(1));

    bool send_fire_config(PingConfig fireMsg);
    PingConfig request_fire_config(const PingConfig& fireMsg);
    PingConfig current_fire_config();

    // Stanby mode (saves current ping rate and set it to 0 on the sonar
    void standby();
    void resume();
    
    virtual void on_connect();
    virtual void handle_message(const OculusMessageHeader& header,
                                const std::vector<uint8_t>& data);

    /////////////////////////////////////////////
    // All remaining member function are related to callbacks and are merely
    // helpers to add callbacks.

    // status callbacks managing functions
    template <typename F, class... Args>
    unsigned int add_status_callback(F&& func, Args&&... args);
    unsigned int add_status_callback(const StatusListener::CallbackT& callback);
    bool remove_status_callback(unsigned int callbackId);
    template <typename F, class... Args>
    bool on_next_status(F&& func, Args&&... args);
    bool on_next_status(const StatusListener::CallbackT& callback);

    // ping callbacks managing functions
    template <typename F, class... Args>
    unsigned int add_ping_callback(F&& func, Args&&... args);
    unsigned int add_ping_callback(const PingCallbacks::CallbackT& callback);
    bool remove_ping_callback(unsigned int callbackId);
    // these are synchronous function which will wait for the next callback call.
    template <typename F, class... Args>
    bool on_next_ping(F&& func, Args&&... args);
    bool on_next_ping(const PingCallbacks::CallbackT& callback);
    
    // dummy message callbacks managing functions
    template <typename F, class... Args>
    unsigned int add_dummy_callback(F&& func, Args&&... args);
    unsigned int add_dummy_callback(const DummyCallbacks::CallbackT& callback);
    bool remove_dummy_callback(unsigned int callbackId);
    // these are synchronous function which will wait for the next callback call.
    template <typename F, class... Args>
    bool on_next_dummy(F&& func, Args&&... args);
    bool on_next_dummy(const DummyCallbacks::CallbackT& callback);
    
    // generic message callbacks managing functions (will be called on any
    // message received).
    template <typename F, class... Args>
    unsigned int add_message_callback(F&& func, Args&&... args);
    unsigned int add_message_callback(const MessageCallbacks::CallbackT& callback);
    bool remove_message_callback(unsigned int callbackId);
    // these are synchronous function which will wait for the next callback call.
    template <typename F, class... Args>
    bool on_next_message(F&& func, Args&&... args);
    bool on_next_message(const MessageCallbacks::CallbackT& callback);
};

// status callbacks
template <typename F, class... Args>
unsigned int SonarDriver::add_status_callback(F&& func, Args&&... args)
{
    return statusListener_.add_callback(func, args...);
}

template <typename F, class... Args>
bool SonarDriver::on_next_status(F&& func, Args&&... args)
{
    return statusListener_.on_next_status(func, args...);
}

// ping callbacks
template <typename F, class... Args>
unsigned int SonarDriver::add_ping_callback(F&& func, Args&&... args)
{
    // static_cast is to avoid infinite loop at type resolution at compile time
    return this->add_ping_callback(static_cast<const PingCallbacks::CallbackT&>(
        std::bind(func, args..., std::placeholders::_1, std::placeholders::_2)));
}

template <typename F, class... Args>
bool SonarDriver::on_next_ping(F&& func, Args&&... args)
{
    return this->on_next_ping(static_cast<const PingCallbacks::CallbackT&>(
        std::bind(func, args..., std::placeholders::_1, std::placeholders::_2)));
}

// dummy callbacks
template <typename F, class... Args>
unsigned int SonarDriver::add_dummy_callback(F&& func, Args&&... args)
{
    // static_cast is to avoid infinite loop at type resolution at compile time
    return this->add_dummy_callback(static_cast<const DummyCallbacks::CallbackT&>(
        std::bind(func, args..., std::placeholders::_1)));
}

template <typename F, class... Args>
bool SonarDriver::on_next_dummy(F&& func, Args&&... args)
{
    return this->on_next_dummy(static_cast<const DummyCallbacks::CallbackT&>(
        std::bind(func, args..., std::placeholders::_1)));
}

// generic messages callbacks
template <typename F, class... Args>
unsigned int SonarDriver::add_message_callback(F&& func, Args&&... args)
{
    // static_cast is to avoid infinite loop at type resolution at compile time
    return this->add_message_callback(static_cast<const MessageCallbacks::CallbackT&>(
        std::bind(func, args..., std::placeholders::_1, std::placeholders::_2)));
}

template <typename F, class... Args>
bool SonarDriver::on_next_message(F&& func, Args&&... args)
{
    return this->on_next_message(static_cast<const MessageCallbacks::CallbackT&>(
        std::bind(func, args..., std::placeholders::_1, std::placeholders::_2)));
}


}; //namespace oculus
}; //namespace narval

#endif //_DEF_NARVAL_OCULUS_SONAR_DRIVER_H_


