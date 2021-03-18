#pragma once

class IConnector {

public:
	virtual void send(void*) = 0;
	virtual void* receive() = 0;
};
