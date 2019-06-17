//
// Created by jona on 2019-06-10.
//

#include <unistd.h>
#include <strings.h>
#include "PacketParser.hpp"
#include "in/PacketInHandshake.hpp"
#include "in/PacketInStatusRequest.hpp"
#include "in/PacketInLoginStart.hpp"
#include "in/PacketInLoginEncryptionResponse.hpp"
#include "in/PacketInStatusPing.hpp"
#include "in/PacketInPlayPluginMessage.hpp"
#include "Identifier.hpp"
#include "exception/UnknownPacketException.hpp"
#include "exception/Exception.hpp"
#include "exception/OffsetOutOfBoundsException.hpp"
#include "in/PacketInPlayClientSettings.hpp"
#include "in/PacketInPlayTeleportConfirm.hpp"
#include "in/PacketInLegacyPingRequest.hpp"
#include "in/PacketInPlayUpdateStructureBlock.hpp"
#include "in/PacketInLoginPluginResponse.hpp"

namespace protocol {


    PacketParser::PacketParser() {
        dataBuffer = nullptr;
        dataLength = 0;
        currentOffset = 0;
    }

    PacketInBase *
    PacketParser::parse(ConnectionState currentState, int packetId, unsigned char *data,
                        int theDataLength) {
        currentOffset = 0;
        this->dataBuffer = data;
        this->dataLength = theDataLength;
        if (currentState == HANDSHAKING) {
            return _parseHandshake(packetId);
        } else if (currentState == STATUS) {
            return _parseStatus(packetId);
        } else if (currentState == LOGIN) {
            return _parseLogin(packetId);
        } else if (currentState == PLAY) {
            return _parsePlay(packetId);
        }
        throw UnknownPacketException(packetId, UNDEFINED, dataLength);
    }

    PacketInBase *
    PacketParser::_parseHandshake(int packetId) {
        if (packetId == HANDSHAKE) {
            return new PacketInHandshake(this);
        } else if (packetId == LEGACY_PING) {
            return new PacketInLegacyPingRequest();
        }
        throw UnknownPacketException(packetId, HANDSHAKING, dataLength);
    }

    PacketInBase *
    PacketParser::_parseStatus(int packetId) {
        if (packetId == STATUS_REQUEST) {
            return new PacketInStatusRequest(this);
        } else if (packetId == PING) {
            return new PacketInStatusPing(this);
        }
        throw UnknownPacketException(packetId, STATUS, dataLength);
    }

    PacketInBase *
    PacketParser::_parsePlay(int packetId) {
        if (packetId == SERVER_PLUGIN_MESSAGE) {
            return new PacketInPlayPluginMessage(this);
        } else if (packetId == CLIENT_SETTINGS) {
            return new PacketInPlayClientSettings(this);
        } else if (packetId == TELEPORT_CONFIRM) {
            return new PacketInPlayTeleportConfirm(this);
        } else if (packetId == UPDATE_STRUCTURE_BLOCK) {
            return new PacketInPlayUpdateStructureBlock(this);
        }
        throw UnknownPacketException(packetId, PLAY, dataLength);
    }

    PacketInBase *
    PacketParser::_parseLogin(int packetId) {
        if (packetId == LOGIN_START) {
            return new PacketInLoginStart(this);
        } else if (packetId == ENCRYPTION_RESPONSE) {

        } else if (packetId == LOGIN_PLUGIN_RESPONSE) {

        }
        throw UnknownPacketException(packetId, LOGIN, dataLength);
    }

    uint8_t PacketParser::readUnsignedByte() {
        verifyDataLeft(1);
        return dataBuffer[currentOffset++];
    }

    int8_t PacketParser::readByte() {
        return readUnsignedByte();
    }

    int8_t PacketParser::readByte(int socket) {
        uint8_t theByte = 0;
        int readBytes = read(socket, &theByte, 1);
        if (readBytes != 1) {
            throw Exception("Could not read byte from network!");
        }
        return theByte;
    }

    int32_t PacketParser::readVarInt() {
        int32_t finalValue = 0;
        uint8_t read = 0;
        uint8_t current = 0;
        do {
            if (read > 5) {
                throw Exception("Can't read VarInt>5!");
            }
            current = readByte();
            finalValue |= (current & 0b01111111u) << (7u * read++);
        } while ((current & 0b10000000u) != 0);
        return finalValue;
    }


    int32_t PacketParser::readVarInt(int socket, uint8_t *length) {
        int32_t finalValue = 0;
        uint8_t read = 0;
        uint8_t current = 0;
        do {
            if (read > 5) {
                throw Exception("Can't read VarInt>5!");
            }
            current = readByte(socket);
            finalValue |= (current & 0b01111111u) << (7u * read++);
        } while ((current & 0b10000000u) != 0);
        *length += read;
        return finalValue;
    }

    std::string PacketParser::readString() {
        uint32_t length = 0;
        return readString(&length);
    }

    std::string PacketParser::readString(uint32_t *length) {
        *length = readVarInt();
        verifyDataLeft(*length);
        char *array = (char *) readByteArray(*length);
        std::string toReturn = std::string(array);
        free(array);
        return toReturn;
    }

    uint16_t PacketParser::readUnsignedShort() {
        uint16_t shortToReturn =
                readUnsignedByte() << 8u | readUnsignedByte();
        return shortToReturn;
    }


    uint8_t *PacketParser::readByteArray(uint32_t count) {
        auto *actualData = (uint8_t *) malloc(sizeof(uint8_t) * (count + 1));
        bzero(actualData, count);
        for (int i = 0; i < count; i++) {
            actualData[i] = readByte();
        }
        actualData[count] = '\0';
        return actualData;
    }

    int64_t PacketParser::readLong() {
        int64_t value = 0;
        for (int i = 0; i < 8; i++) {
            value |= readByte();
            value <<= 8;
        }
        return value;
    }

    bool PacketParser::verifyDataLeft(uint32_t left) {
        return (currentOffset + left <= dataLength);
    }

    bool PacketParser::readBoolean() {
        return readByte();
    }

    uint8_t *PacketParser::readByteArray(uint32_t *length) {
        *length = dataLength - currentOffset;
        return readByteArray(dataLength - currentOffset);
    }
}
