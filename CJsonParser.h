#ifndef CS_CEVENTS_H
#define CS_CEVENTS_H
#pragma once

#include "main.h"
#include "glog/logging.h"
#include "SIpCameraEvent.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <utility>
#include <ctime>
#include <chrono>
#include <map>

namespace pt = boost::property_tree;

class CJsonParser {
    using string = std::string;
	using params_map = std::map<string, pt::ptree>;
public:

    explicit CJsonParser(std::string rawData);;

	explicit CJsonParser();;

	CJsonParser(const CJsonParser &) = default;
    CJsonParser &operator=(const CJsonParser &) = default;

    pt::ptree getTree() const;

    string toPrettyJson() const;

	// throws exceptions!!!
	void validateData();

	// throws exceptions!!!
	string getMessage() const;

	// no throw
	string getCommand() const noexcept;

	// no throw
	string getCameraSensorProviderID() const noexcept;
	
	// no throw
	bool isIpCameraEvent() const noexcept;

	SIpCameraEvent parseIpCameraEvent() const noexcept;

	string buildAnswer(bool status, const string &command, const string &message
					, params_map otherParams = params_map()) const;

	static string BuildAnswer(bool status, const string &command, const string &message
            , params_map otherParams = params_map());


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