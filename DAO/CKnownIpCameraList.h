//
// Created by childcity on 07.04.19.
//

#ifndef CS_IPCAMERASERVER_CKNOWNIPCAMERALIST_H
#define CS_IPCAMERASERVER_CKNOWNIPCAMERALIST_H
#pragma once

#include "SKnownIpCamera.hpp"
#include "../CSQLiteDB.h"
#include <list>
#include <algorithm>

class CKnownIpCameraList {
public:
    CKnownIpCameraList() = default;
    CKnownIpCameraList(const CKnownIpCameraList &) = default;
    CKnownIpCameraList &operator=(const CKnownIpCameraList &) = default;

    void setActive(const CSQLiteDB::ptr &dbPtr);

    bool isActive(const std::string &sensorProviderID) const;

private:
    std::list<SKnownIpCamera> activeCameras_;
};


#endif //CS_IPCAMERASERVER_CKNOWNIPCAMERALIST_H
