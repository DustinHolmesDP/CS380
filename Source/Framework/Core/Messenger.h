#pragma once
#include "Messages.h"
#include "Misc/NiceTypes.h"
#include <vector>
#include <array>

// simple messaging system, doesn't support removal, mostly for engine comms
class Messenger
{
public:
    static void listen_for_message(Messages type, Callback callback);
    static void send_message(Messages type);

    static void clear_all_listeners();
private:
    static std::array<std::vector<Callback>, static_cast<size_t>(Messages::NUM_ENTRIES)> inboxes;
};