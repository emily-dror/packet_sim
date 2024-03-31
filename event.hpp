#ifndef _EVENT_HPP_
#define _EVENT_HPP_

class event_c {
public:
    // Construct sets time of event_c.
    event_c(double time);
    virtual ~event_c() = default;

    // Execute event_c by invoking this method.
    virtual void process_event() = 0;

    // time the event occurred
    const double time;
};

class arrival_event_c : public event_c {
public:
    arrival_event_c(double time, unsigned int entry_port);
    void process_event() override;

private:
    unsigned int entry_port;
};

class leave_event_c : public event_c {
public:
    leave_event_c(double time, unsigned int exit_port);
    void process_event() override;

private:
    unsigned int exit_port;
};

struct eventComparator {
    bool operator()(event_c *left, event_c *right) const {
        return left->time > right->time;
    }
};

#endif  // _EVENT_HPP_