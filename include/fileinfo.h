/*
 * fileinfo.h
 *
 * File representation on filelist
 *
 *  Created on: 25.05.2017
 *      Author: raqu
 */

#ifndef DATA_FileInfo_H_
#define DATA_FileInfo_H_

#include <json/json.h>
#include <string>

class FileInfo {
private:
	// file name and owner name
	std::string name, owner;
	// size in bytes
	unsigned long size;
	// flags revoked blocked
	short revoked, blocked;
	// file add time since linux epoch in seconds
	double add_time;

public:
	FileInfo() {
		name = "null";
		owner = "null";
		size = 0;
		revoked = 0;
		blocked = 0;
		add_time = 0.0;
	}
	FileInfo(std::string name, std::string owner, unsigned long size) :
			name(name), owner(owner), size(size) {
		revoked = 0;
		blocked = 0;
		add_time = 0.0;
	}
	FileInfo(Json::Value json) {
		name = json[0].asString();
		owner = json[1].asString();
		size = json[2].asUInt64();
		revoked = json[3].asInt();
		blocked = json[4].asInt();
		add_time = json[5].asDouble();
	}
	virtual ~FileInfo() {}

	Json::Value jsonify() {
		Json::Value json;
		json[0] = name.c_str();
		json[1] = owner.c_str();
		json[2] = Json::Value::UInt64(size);
		json[3] = revoked;
		json[4] = blocked;
		json[5] = add_time;
		return json;
	}

	double getAddTime() const {
		return add_time;
	}

	void setAddTime(double addTime) {
		add_time = addTime;
	}

	const std::string getName() const {
		return name;
	}

	const std::string getOwner() const {
		return owner;
	}

	unsigned long getSize() const {
		return size;
	}

	bool isBlocked() const {
		return blocked == 1;
	}

	void lock() {
		this->blocked = 1;
	}

	void unlock() {
		this->blocked = 0;
	}

	bool isRevoked() const {
		return revoked == 1;
	}

	void revoke() {
		this->revoked = 1;
	}
};

#endif /* DATA_FileInfo_H_ */
