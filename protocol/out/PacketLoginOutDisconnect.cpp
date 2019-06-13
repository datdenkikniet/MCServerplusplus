#include <utility>

//
// Created by jona on 2019-06-11.
//

#include "PacketLoginOutDisconnect.hpp"
#include "../util/Util.hpp"

namespace protocol {
    PacketLoginOutDisconnect::PacketLoginOutDisconnect(std::string reason) : PacketOutBase(0x1B, DISCONNECT),
                                                                             reason(std::move(reason)) {

    }

    const std::string &PacketLoginOutDisconnect::getReason() const {
        return reason;
    }

    void PacketLoginOutDisconnect::serialize(PacketSerializer *packetSerializer) {
        packetSerializer->writeString(getReason());
    }

}