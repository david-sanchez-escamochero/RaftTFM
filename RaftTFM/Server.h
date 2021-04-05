#pragma once

#include <stdint.h>
#include "RaftDefs.h"
#include "Command.h"
#include "Candidate.h"
#include "Follower.h"
#include "Leader.h"
#include "IConnector.h"
#include "Semaphore.h"
#include <queue>
#include "ManagerLog.h"


#define NONE	-1


typedef struct {
	uint32_t log_index_;
	Command	 log_[MAX_LOG_ENTRIES];
}Log;

class Server
{

public: 
	Server(uint32_t server_id);
	~Server();
	void		send(RPC* rpc, unsigned short port, std::string sender, std::string action, std::string receiver);
	void*		receive();
	void		start();
	IConnector* get_current_shape_sever(StateEnum state);
	uint32_t    get_server_id();
	void		set_new_state(StateEnum state);
	std::mutex	mu_server_;
	void		increment_current_term();
	uint32_t	get_current_term();
	void		set_current_term(uint32_t term);
	uint32_t	get_commit_index();
	uint32_t	get_last_applied();
	int32_t		get_voted_for();
	void		set_voted_for(int32_t vote_for);

protected:

	StateEnum	current_state_;
	StateEnum	new_state_;
	std::thread thread_server_dispatch_;
	std::thread thread_check_new_state_;


	// Persisten state on all servers. 
	uint32_t	current_term_;			// Latest term server has seen (initialized to 0 on first boot, increases monotonically)
	int32_t		voted_for_;				// CandidateId that received vote in current term(or null if none)
	Log			log_;					// Log entries; each entry contains command for state machine, and term when entry was received by leader(first index is 1)

	// Volatile state on all servers. 
	uint32_t	commit_index_;			// Index of highest log entry known to be committed(initialized to 0, increases	monotonically)
	uint32_t	last_applied_;			// Index of highest log entry applied to state	machine(initialized to 0, increases	monotonically)
	
	
private: 
	
	IConnector*		connector_;
	uint32_t		server_id_;
	Communication	communication_;
	bool			have_to_die_;	
	void			dispatch();
	Semaphore		semaphore_dispatch_;
	Semaphore		semaphore_new_state_;
	queue<RPC>		queue_;
	void			check_new_state();
	std::mutex		mu_new_state_;
	ManagerLog		manager_log_;
	std::string		file_log_name_;
};

