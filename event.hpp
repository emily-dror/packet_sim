#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include <memory>

class event_c {
public:
    // Construct sets time of event_c.
    event_c(double time): time(time) {}

    // Execute event_c by invoking this method.
    virtual void process_event() = 0;
    const double time;
    virtual ~event_c() = default;
};

struct eventComparator {
    bool operator()(event_c *left, event_c *right) const {
        return left->time > right->time;
    }
};

class arrival_event_c : public event_c {
public:
    arrival_event_c(double time, unsigned int entry_port): event_c(time), entry_port(entry_port) {}
    virtual void process_event();
private:
    unsigned int entry_port;
};

class leave_event_c : public event_c {
public:
    leave_event_c(double time, unsigned int exit_port): event_c(time), exit_port(exit_port) {}
    virtual void process_event();
    unsigned int exit_port;
private:
};


#endif  // _EVENT_HPP_