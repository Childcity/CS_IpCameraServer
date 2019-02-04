//
// Created by childcity on 03.10.18.
//

#ifndef CS_MINISQLITESERVER_CBUSINESSLOGIC_H
#define CS_MINISQLITESERVER_CBUSINESSLOGIC_H
#pragma once

#include "main.h"
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

    void setLastIpCamEvent(const SIpCameraEvent &ipCamEvent){
        //exclusive access to data!
        boost::unique_lock<boost::shared_mutex> lock(business_logic_mtx_);
        lastIpCamEvent_ = ipCamEvent;
    }

	SIpCameraEvent getLastIpCamEvent() const {
        //NOT exclusive access to data! Allows only read, not write!
        boost::shared_lock<boost::shared_mutex> lock(business_logic_mtx_);
        return lastIpCamEvent_;
    }

    // throws BusinessLogicError
    static void CreateOrUseDb(const std::string &dbPath) {

        const auto tmpDb = CSQLiteDB::new_(dbPath);

        // check if tmp db exists
        if(! tmpDb->OpenConnection(SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_READWRITE)){
            LOG(INFO) <<"Can't connect to " <<dbPath <<": " <<tmpDb->GetLastError();
            LOG(INFO) <<"Trying to create new " <<dbPath;

            //create new db. If can't create, return;
            if(! tmpDb->OpenConnection(SQLITE_OPEN_FULLMUTEX|SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE)){
                LOG(WARNING) <<"BUSINESS_LOGIC: can't create or connect to " <<dbPath <<": " <<tmpDb->GetLastError();
                throw BusinessLogicError("Database can't be opened or created. Check permissions and free place on disk");
            }
        }

        //create table for temporary query strings
        int res = tmpDb->Execute(SIpCameraEvent::CREATE_TABLE_QUERY());

        if(res < 0){
            string errMsg("Can't create table for ip_camera events");
            LOG(WARNING) <<"BUSINESS_LOGIC: " <<errMsg;
            throw BusinessLogicError(errMsg);
        }

    }

private:

private:
    mutable boost::shared_mutex business_logic_mtx_;

    SIpCameraEvent lastIpCamEvent_;
};

#endif //CS_MINISQLITESERVER_CBUSINESSLOGIC_H
