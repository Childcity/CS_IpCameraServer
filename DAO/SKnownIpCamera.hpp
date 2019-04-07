//
// Created by childcity on 07.04.19.
//

#ifndef CS_IPCAMERASERVER_SKNOWNIPCAMERA_HPP
#define CS_IPCAMERASERVER_SKNOWNIPCAMERA_HPP
#pragma once

#include <string>

struct SKnownIpCamera {
    using string = std::string;

    SKnownIpCamera() = default;
    SKnownIpCamera(const SKnownIpCamera &) = default;
    SKnownIpCamera &operator=(const SKnownIpCamera &) = default;

    string sensorProviderID;
    bool   isActive;

    static const char *CREATE_TABLE_QUERY() {
        return u8"CREATE TABLE IF NOT EXISTS known_ip_cameras(\n"
               // 'id' is not needed for SQLITE, because this db by default create raw_id in each table
               "  sensor_provider_id TEXT NOT NULL UNIQUE,\n"
               "  is_active BOOLEAN DEFAULT 1,\n"
               "  created_at DATETIME DEFAULT (datetime('now', 'localtime'))\n"
               ");";
    }

    static const char *SELECT_ALL_ACTIVE_QUERY() {
        return "SELECT sensor_provider_id FROM known_ip_cameras WHERE is_active=1";
    };
};

#endif //CS_IPCAMERASERVER_SKNOWNIPCAMERA_HPP
