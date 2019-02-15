//
// Created by childcity on 30.01.19.
//

#include "CJsonParser.h"

CJsonParser::CJsonParser(std::string rawData)
        : rawData_(std::move(rawData))
{}

CJsonParser::CJsonParser()
        : CJsonParser(string())
{}

pt::ptree CJsonParser::getTree() const {
    return tree_;
}

string CJsonParser::toPrettyJson() const {
    std::ostringstream ostream;
    pt::write_json(ostream, tree_, true); // true to make pretty json
    return ostream.str();
}

void CJsonParser::validateData() {
    std::stringstream strstream(rawData_);
    // Parse the json into the property tree.
    pt::json_parser::read_json(strstream, tree_);
}

string CJsonParser::parseMessage() const {
    return tree_.get<string>("params.message");
}

string CJsonParser::parseCommand() const noexcept {
    return tree_.get("command", string());
}

bool CJsonParser::isIpCameraEvent() const noexcept {
    // if plateText is empty or plateConfidence is empty -> we think, that it isn't event from ip camera
    return (! tree_.get("plateText", string()).empty())
           && (! tree_.get("plateConfidence", string()).empty());
}

SIpCameraEvent CJsonParser::parseIpCameraEvent() const noexcept {
    VLOG_IF(1, tree_.empty()) << "Assert failed: 'tree_' is empty!";
    VLOG_IF(1, rawData_.empty()) << "Assert failed: 'rawData_' is empty!";

    SIpCameraEvent ipCamEvent;

    try {
        ipCamEvent.packetCounter = tree_.get("packetCounter", string());
        ipCamEvent.datetime = tree_.get("datetime", string());
        ipCamEvent.plateText = tree_.get("plateText", string());
        ipCamEvent.plateCountry = tree_.get("plateCountry", string());
        ipCamEvent.plateConfidence = tree_.get("plateConfidence", string());
        ipCamEvent.cameraId = tree_.get("cameraId", string());
        ipCamEvent.carState = tree_.get("carState", string());
        ipCamEvent.geotag_lat = tree_.get("geotag.lat", string());
        ipCamEvent.geotag_lon = tree_.get("geotag.lon", string());
        ipCamEvent.imageType = tree_.get("imageType", string());
        ipCamEvent.plateImageType = tree_.get("plateImageType", string());
        ipCamEvent.plateImageSize = tree_.get("plateImageSize", string());
        ipCamEvent.carMoveDirection = tree_.get("carMoveDirection", string());
        ipCamEvent.timeProcessing = tree_.get("timeProcessing", string());
        ipCamEvent.plateCoordinates = tree_.get("plateCoordinates", string());
        ipCamEvent.carID = tree_.get("carID", string());
        ipCamEvent.GEOtarget = tree_.get("GEOtarget", string());
        ipCamEvent.sensorProviderID = tree_.get("sensorProviderID", string());
        ipCamEvent.rawJson = rawData_;

    } catch (std::exception &ex){
        LOG(WARNING) <<"ERROR (JsonParser): " <<ex.what();
    }

    return ipCamEvent;
}

string CJsonParser::buildAnswer(bool status, const string &command, const string &message,
                                CJsonParser::params_map otherParams) const {
    // Create an empty property tree object.
    pt::ptree serverAnswerTree;

    std::time_t t = std::time(nullptr);   // get time now
    std::tm *now = std::localtime(&t);

    char dateTimeBuffer [80] = {' '};

    strftime (dateTimeBuffer, 80, "%d-%m-%Y %H:%M:%S", now);

    using std::chrono::duration_cast;
    using std::chrono::milliseconds;
    using std::chrono::system_clock;

    long timestamp = duration_cast<milliseconds>(
            system_clock::now().time_since_epoch()
    ).count();

    // Put the simple values into the tree.
    serverAnswerTree.put("command", command);
    if(! message.empty())
        serverAnswerTree.put("params.message", message);
    serverAnswerTree.put("params.status", status ? "ok" : "error");

    for (const auto &param : otherParams) {
        if((! param.first.empty()) && (! param.second.empty()))
            serverAnswerTree.put_child("params."+param.first, param.second);
    }

    serverAnswerTree.put("timestamp", timestamp);
    serverAnswerTree.put("server_datetime", dateTimeBuffer);

    // Write property tree to stream file
    std::stringstream strStream;
    pt::write_json(strStream, serverAnswerTree, true); // true to make pretty json

    return strStream.str();
}

string CJsonParser::BuildAnswer(bool status, const string &command, const string &message, CJsonParser::params_map otherParams) {
    CJsonParser parser;
    return parser.buildAnswer(status, command, message, std::move(otherParams));
}
