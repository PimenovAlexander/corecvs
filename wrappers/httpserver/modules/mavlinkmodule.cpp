
#include <httpUtils.h>
#include <sstream>
#include "mavlinkmodule.h"
#include "test/gcs/mavlink/common/mavlink.h"
#include "test/gcs/mavlink/message.hpp"

using namespace corecvs;

bool MavLinkModule::shouldProcessURL(const std::string& url) {
    return HelperUtils::startsWith(url, "/");
}

bool MavLinkModule::shouldPollURL(const std::string& url)
{
    // return !HelperUtils::startsWith(url, "/heartbeatMessage");
    return false;
}

bool MavLinkModule::shouldWrapURL(const std::string& url)
{
    return HelperUtils::startsWith(url, "/mavlink");
}

int addToBuffer8(uint8_t data, char *buffer, int len, int j)
{
    for (int i = sizeof(data) - 1; i >= 0 && j < len; i--, j++) {
        buffer[j] = (char) (data >> i);
    }
    return j;
}

int addToBuffer16(uint16_t data, char *buffer, int len, int j)
{
    for (int i = sizeof(data) - 1; i >= 0 && j < len; i--, j++) {
        buffer[j] = (char) (data >> i);
    }
    return j;
}

int addToBuffer32(uint32_t data, char *buffer, int len, int j)
{
    for (int i = sizeof(data) - 1; i >= 0 && j < len; i--, j++) {
        buffer[j] = (char) (data >> i);
    }
    return j;
}

int fillDataBuffer(mavlink_message_t *d, char *buffer, int len)
{
    int i, sizeOfPacket = 0;

    mavlink_message_t message = *d;

    sizeOfPacket = addToBuffer8(message.magic,          buffer, len, sizeOfPacket);
    sizeOfPacket = addToBuffer8(message.len,            buffer, len, sizeOfPacket);
    sizeOfPacket = addToBuffer8(message.seq,            buffer, len, sizeOfPacket);
    sizeOfPacket = addToBuffer8(message.sysid,          buffer, len, sizeOfPacket);
    sizeOfPacket = addToBuffer8(message.compid,         buffer, len, sizeOfPacket);
    sizeOfPacket = addToBuffer32(message.msgid,         buffer, len, sizeOfPacket);
    int payloadSize = sizeof(uint64_t) * ((MAVLINK_MAX_PAYLOAD_LEN+MAVLINK_NUM_CHECKSUM_BYTES+7)/8);
    for (i = payloadSize - 1; i >= 0 && sizeOfPacket < len; i--, sizeOfPacket++) {
        buffer[sizeOfPacket] = (char) (message.payload64[0] >> i);
    }
#ifdef incompatflags
    sizeOfPacket = addToBuffer8(message.incompat_flags, buffer, len, sizeOfPacket);
    sizeOfPacket = addToBuffer8(message.compat_flags,   buffer, len, sizeOfPacket);
    int signatureSize = sizeof(uint8_t) * MAVLINK_SIGNATURE_BLOCK_LEN;
    for (i = signatureSize - 1; i >= 0 && sizeOfPacket < len; i--, sizeOfPacket++) {
        buffer[sizeOfPacket] = (char) (message.signature[0] >> i);
    }
#endif
    int checksumSize = sizeof(uint8_t) * 2;
    for (i = checksumSize - 1; i >= 0 && sizeOfPacket < len; i--, sizeOfPacket++) {
        buffer[sizeOfPacket] = (char) (message.ck[0] >> i);
    }
    sizeOfPacket = addToBuffer16(message.checksum,      buffer, len, sizeOfPacket);

    if (sizeOfPacket > len) {
        /* Error!  The buffer wasn't big enough. */
        return sizeOfPacket;
    } else {
        return 0;
    }
}

std::vector<uint8_t> MavLinkContent::getContent()
{
    std::ostringstream result;

    if (containsValue || containsValues) {
        if (containsValue) {
            result << value;
        }
        if (containsValues) {
            for (auto value : values) {
                result << "&" << value;
            }
        }
    } else {
        // These variables are mocks that have to be replaced with UAV's logic
        int system_id = 1;
        int component_id = 2;
        int target_system = 1;
        int target_component = 1;
        int type = 0;
        int mission_type = 1;
        auto *message = new mavlink_message_t();
        auto mavlink_message_size = mavlink_msg_mission_ack_pack(system_id, component_id, message, target_system, target_component, type, mission_type);

        char data_arr[mavlink_message_size];

        int nonbufferedMessageSize = fillDataBuffer(message, data_arr, mavlink_message_size);

        if (nonbufferedMessageSize == 0) {
            result << message;
        } else {
            result << nonbufferedMessageSize;
        }
    }

    std::string str = result.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::shared_ptr<HttpContent> MavLinkModule::getContentByUrl(const std::string& url)
{
    std::string urlPath(url);

    // Removes poll prefix
    checkAndRewritePollPrefix(urlPath);

    std::vector<std::pair<std::string, std::string> > query = HttpUtils::parseParameters(urlPath);

    std::cout << "MavLink Module : " << urlPath << std::endl;

    if (
            HelperUtils::startsWith(urlPath, "/missionItem") ||
            HelperUtils::startsWith(urlPath, "/clearAll") ||
            HelperUtils::startsWith(urlPath, "/pauseMission") ||
            HelperUtils::startsWith(urlPath, "/heartbeatMessage") ||
            HelperUtils::startsWith(urlPath, "/missionSetCurrent"))
    {
        return std::shared_ptr<HttpContent>(new MavLinkContent());
    }
    if (HelperUtils::startsWith(urlPath, "/currentMissionItem"))
    {
        // Current mission item should be acquired from UAV's data
        int currentMissionItem = 0;
        return std::shared_ptr<HttpContent>(new MavLinkContent(currentMissionItem));
    }
    if (HelperUtils::startsWith(urlPath, "/setParam"))
    {
        std::cout << std::endl << "SET PARAM " << std::endl << query[0].first << std::endl;
    }
    if (HelperUtils::startsWith(urlPath, "/getProperties"))
    {
        int properties[] = { rand() % 100, rand() % 100, rand() % 100 };
        return std::shared_ptr<HttpContent>(new MavLinkContent(properties));
    }

    return std::shared_ptr<HttpContent>();
}

MavLinkModule::MavLinkModule() {

}
