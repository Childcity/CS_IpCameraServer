#ifndef CS_CEVENTS_H
#define CS_CEVENTS_H
#pragma once

#include "main.h"
#include "glog/logging.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <utility>
#include <ctime>
#include <chrono>
#include <map>

namespace pt = boost::property_tree;

struct SIpCameraEvent {
	SIpCameraEvent() = default;
    SIpCameraEvent(const SIpCameraEvent &) = default;
	SIpCameraEvent &operator=(const SIpCameraEvent &) = default;

	string packetCounter;
    string datetime;
    string plateText;
    string plateCountry;
	string plateConfidence;
    string cameraId;
    string carState;
    string geotag_lat;
    string geotag_lon;
    string imageType;
    string plateImageType;
    string plateImageSize;
    string carMoveDirection;
    string timeProcessing;
    string plateCoordinates;
    string carID;
    string GEOtarget;
    string sensorProviderID;
    string rawJson;

	static const char *CREATE_TABLE_QUERY() {
		return "CREATE TABLE IF NOT EXISTS ip_camera_events(\n"
			"  packetCounter INTEGER,\n"
			"  `datetime` TEXT,\n"
			"  plateText BLOB,\n"
			"  plateCountry TEXT,\n"
			"  plateConfidence REAL,\n"
			"  cameraId TEXT,\n"
			"  carState TEXT,\n"
			"  geotag_lat TEXT,\n"
			"  geotag_lon TEXT,\n"
			"  imageType TEXT,\n"
			"  plateImageType TEXT,\n"
			"  plateImageSize TEXT,\n"
			"  carMoveDirection TEXT,\n"
			"  timeProcessing TEXT,\n"
			"  plateCoordinates TEXT,\n"
			"  carID TEXT,\n"
			"  GEOtarget TEXT,\n"
			"  sensorProviderID TEXT,\n"
			"  rawJson TEXT,\n"
			"  wroteToDbAt DATETIME DEFAULT CURRENT_TIMESTAMP\n"
        ");";
	}
	
	static string INSERT_EVENT_QUERY(const SIpCameraEvent &ipCamEvent) {
		static const string constPart("INSERT INTO ip_camera_events (packetCounter, `datetime`, plateText, plateCountry, plateConfidence, cameraId, carState, geotag_lat, geotag_lon, "
		"imageType, plateImageType, plateImageSize, carMoveDirection, timeProcessing, plateCoordinates, carID, GEOtarget, sensorProviderID, rawJson) ");
		
		return string(constPart + "VALUES "
					"('" + ipCamEvent.packetCounter		+ "', '" + ipCamEvent.datetime			+ "', '" + ipCamEvent.plateText 
				+ "', '" + ipCamEvent.plateCountry		+ "', '" + ipCamEvent.plateConfidence
				+ "', '" + ipCamEvent.cameraId			+ "', '" + ipCamEvent.carState			+ "', '" + ipCamEvent.geotag_lat		
				+ "', '" + ipCamEvent.geotag_lon		+ "', '" + ipCamEvent.imageType			+ "', '" + ipCamEvent.plateImageType
				+ "', '" + ipCamEvent.plateImageSize	+ "', '" + ipCamEvent.carMoveDirection	+ "', '" + ipCamEvent.timeProcessing
				+ "', '" + ipCamEvent.plateCoordinates	+ "', '" + ipCamEvent.carID				+ "', '" + ipCamEvent.GEOtarget
				+ "', '" + ipCamEvent.sensorProviderID	+ "', '" + ipCamEvent.rawJson 
				+ "');"
			);
	}
};

class CJsonParser {
    using string = std::string;
	using params_map = std::map<string, pt::ptree>;
public:

    explicit CJsonParser(std::string rawData)
		: rawData_(std::move(rawData))
		{};

	explicit CJsonParser()
		: CJsonParser(string())
	{};

	CJsonParser(const CJsonParser &) = default;
    CJsonParser &operator=(const CJsonParser &) = default;

    pt::ptree getTree() const {
        return tree_;
    }

    string toPrettyJson() const {
        std::ostringstream ostream;
        pt::write_json(ostream, tree_, true); // true to make pretty json
        return ostream.str();
    }

	// throws exceptions!!!
	void validateData() {
		std::stringstream strstream(rawData_);
		// Parse the json into the property tree.
		pt::json_parser::read_json(strstream, tree_);
	}

	// throws exceptions!!!
	string parseMessage() const {
		return tree_.get<string>("params.message");
	}

	// no throw
	string parseCommand() const noexcept {
		return tree_.get("command", string());
	}
	
	// no throw
	bool isIpCameraEvent() const noexcept {
		// if plateText is empty or plateConfidence is empty -> we think, that it isn't event from ip camera
		return (! tree_.get("plateText", string()).empty()) 
					&& (! tree_.get("plateConfidence", string()).empty());
	}

	SIpCameraEvent parseIpCameraEvent() const noexcept {
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

	string buildAnswer(bool status, const string &command, const string &message
					, params_map otherParams = params_map()) const {
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
		serverAnswerTree.put("params.status", status ? "ok":"error");

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

	static string BuildAnswer(bool status, const string &command, const string &message
            , params_map otherParams = params_map()) {
		CJsonParser parser;
		return parser.buildAnswer(status, command, message, std::move(otherParams));
	}


private:
	pt::ptree tree_;
	string rawData_;
};

#endif //CS_CEVENTS_H

/* Example of event:
{
    "packetCounter":"67781",
    "datetime":"20190123 154156000",
    "plateText":"\u004f\u0042\u0032\u0035\u0035\u0038\u0041\u004b",
    "plateCountry":"BGR",
    "plateConfidence":"0.727815",
    "cameraId":"48EA633E7912",
    "carState":"new",
    "geotag":{
        "lat":50.418114,
        "lon":30.476213
    },
    "imageType":"plate",
    "plateImageType":"png",
    "plateImageSize":"0",
    "carMoveDirection":"in",
    "timeProcessing":"0",
    "plateCoordinates":[
        420,
        180,
        356,
        66
    ],
    "carID":"102",
    "GEOtarget":"Camera",
    "sensorProviderID":"Terminal_1"

}
{"packetCounter":"67781","datetime":"20190123 154156000","plateText":"\u004f\u0042\u0032\u0035\u0035\u0038\u0041\u004b","plateCountry":"BGR","plateConfidence":"0.727815","cameraId":"48EA633E7912","carState":"new","geotag":{"lat":50.418114,"lon":30.476213},"imageType":"plate","plateImageType":"png","plateImageSize":"0","carMoveDirection":"in","timeProcessing":"0","plateCoordinates":[420,180,356,66],"carID":"102","GEOtarget":"Camera","sensorProviderID":"Terminal_1"}
{"command": "who", "params": { "message": "250000000" }}
 */

/*
    (?:(?<number>  -?(?=[1-9]|0(?!\d))\d+(\.\d+)?([eE][+-]?\d+)?){0}
    (?<boolean> true | false | null ){0}
    (?<string>  " (?>[^"\\\\]* | \\\\ ["\\\\bfnrt\/] | \\\\ u [0-9a-f]{4} )* " ){0}
    (?<array>   \[ (?> \g<json> (?: , \g<json> )* )? \s* \] ){0}
    (?<pair>    \s* \g<string> \s* : \g<json> ){0}
    (?<object>  \{ (?> \g<pair> (?: , \g<pair> )* )? \s* \} ){0}
    (?<json>    \s* (?> \g<number> | \g<boolean> | \g<string> | \g<array> | \g<object> ) \s* ){0}
    )
    \A \g<json> \Z
*/