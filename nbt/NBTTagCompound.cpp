//
// Created by jona on 2019-06-19.
//

#include "NBTTagCompound.hpp"
#include "NBTTagByte.hpp"
#include "NBTTagShort.hpp"
#include "NBTTagInt.hpp"
#include "NBTTagLong.hpp"
#include "NBTTagFloat.hpp"
#include "NBTTagDouble.hpp"
#include "NBTTagByteArray.hpp"
#include "NBTTagString.hpp"
#include "NBTTagList.hpp"

namespace nbt {
    NBTTagCompound::NBTTagCompound() : NBTTagBase(COMPOUND) {

    }

    NBTTagCompound::~NBTTagCompound() {
        for (auto &nbtTag : elements) {
            delete nbtTag;
        }
    }

    NBTTagBase *NBTTagCompound::get(const std::string &nameToGet) {
        for (auto &nbtTag : elements) {
            if (nbtTag->hasName() && nbtTag->getName() == nameToGet) {
                return nbtTag;
            }
        }
        return nullptr;
    }

    bool NBTTagCompound::remove(const std::string &nameToRemove, bool del) {
        for (auto &nbtTag : elements) {
            if (nbtTag->hasName() && nbtTag->getName() == nameToRemove) {
                elements.remove(nbtTag);
                if (del) {
                    delete nbtTag;
                }
                return true;
            }
        }
        return false;
    }

    void NBTTagCompound::add(NBTTagBase *toAdd) {
        elements.push_back(toAdd);
    }

    bool NBTTagCompound::contains(const std::string &nameToFind) {
        return get(nameToFind) != nullptr;
    }

    void NBTTagCompound::parsePayload(std::FILE *stream, bool named) {
        if (named) {
            parseName(stream);
        }
        elements = std::list<NBTTagBase *>();
        uint8_t typeId;
        do {
            std::fread(&typeId, 1, 1, stream);
            NBTTagBase *value;
            switch (typeId) {
                case END:
                    return;
                case BYTE:
                    value = new NBTTagByte();
                    break;
                case SHORT:
                    value = new NBTTagShort();
                    break;
                case INT:
                    value = new NBTTagInt();
                    break;
                case LONG:
                    value = new NBTTagLong();
                    break;
                case FLOAT:
                    value = new NBTTagFloat();
                    break;
                case DOUBLE:
                    value = new NBTTagDouble();
                    break;
                case BYTE_ARRAY:
                    value = new NBTTagByteArray();
                    break;
                case STRING:
                    value = new NBTTagString();
                    break;
                case LIST:
                    value = new NBTTagList();
                    break;
                case COMPOUND:
                    value = new NBTTagCompound();
                    break;
                case INT_ARRAY:
                    value = new NBTTagByte();
                    break;
                case LONG_ARRAY:
                    value = new NBTTagByte();
                    break;
                default:
                    return;
            }
            value->parsePayload(stream, true);
            elements.push_back(value);
        } while (typeId != END);
    }

    void NBTTagCompound::write(std::FILE *stream, bool named) {

    }
}
