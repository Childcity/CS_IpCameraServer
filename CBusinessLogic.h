//
// Created by childcity on 03.10.18.
//

#ifndef CS_MINISQLITESERVER_CBUSINESSLOGIC_H
#define CS_MINISQLITESERVER_CBUSINESSLOGIC_H
#pragma once

#include "main.h"
#include "DAO/CKnownIpCameraList.h"
#include "DAO/SKnownIpCamera.hpp"
#include "CJsonParser.h"
#include "CSQLiteDB.h"
#include "glog/logging.h"

#include <memory>
#include <string>
#include <mutex>

class BusinessLogicError: public std::exception {
public:
    explicit BusinessLogicError(string errorMsg) noexcept
            : msg_(std::move(errorMsg))
    {}
    explicit BusinessLogicError(std::string &&errorMsg) noexcept
            : msg_(std::move(errorMsg))
    {}
    explicit BusinessLogicError(const char *errorMsg) noexcept
            : msg_(std::move(std::string(errorMsg)))
    {}
    const char *what() const noexcept override{
        return msg_.c_str();
    }
private:
    std::string msg_;
};

class CBusinessLogic : public boost::enable_shared_from_this<CBusinessLogic> {

public:
    explicit CBusinessLogic();

     virtual ~CBusinessLogic();

    CBusinessLogic(CBusinessLogic const&) = delete;
    CBusinessLogic &operator=(CBusinessLogic const&) = delete;

    bool setLastIpCamEvent(const CSQLiteDB::ptr &dbPtr, const SIpCameraEvent &ipCamEvent);

	SIpCameraEvent getLastIpCamEvent(const string &sensorProviderID = std::string());

    // throws BusinessLogicError
    static void CreateOrUseDb(const std::string &dbPath);

private:

private:
    mutable boost::shared_mutex business_logic_mtx_;

    std::list<SIpCameraEvent> lastIpCamEvents_;
    CKnownIpCameraList knownIpCameraList_;
};

#endif //CS_MINISQLITESERVER_CBUSINESSLOGIC_H
