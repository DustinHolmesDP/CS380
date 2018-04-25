#include <pch.h>
#include "Messenger.h"

std::array<std::vector<Callback>, static_cast<size_t>(Messages::NUM_ENTRIES)> Messenger::inboxes;

void Messenger::listen_for_message(Messages type, Callback callback)
{
    inboxes[static_cast<size_t>(type)].emplace_back(callback);
}

void Messenger::send_message(Messages type)
{
    for (auto && cb : inboxes[static_cast<size_t>(type)])
    {
        cb();
    }
}

void Messenger::clear_all_listeners()
{
    for (auto && cbv : inboxes)
    {
        cbv.clear();
    }
}
