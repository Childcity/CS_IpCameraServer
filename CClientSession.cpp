#include "CClientSession.h"

boost::recursive_mutex clients_cs;
typedef boost::shared_ptr<CClientSession> client_ptr;
typedef std::vector<client_ptr> cli_ptr_vector;
cli_ptr_vector clients;

CClientSession::CClientSession(io_context &io_context, const size_t maxTimeout,
                               CClientSession::businessLogic_ptr businessLogic)
        : sock_(io_context)
        , started_(false)
        , timer_(io_context)
        , maxTimeout_(maxTimeout)
        , clients_changed_(false)
        , username_("user")
        , io_context_(io_context)
        , write_buffer_({ new char[MAX_WRITE_BUFFER] })
        , read_buffer_({ new char[MAX_READ_BUFFER] })
        , businessLogic_(std::move(businessLogic))
{}

void CClientSession::start()
{
    started_ = true;

    {
        boost::recursive_mutex::scoped_lock lk(clients_cs);
        clients.push_back(shared_from_this());
    }

    db = CSQLiteDB::new_(dbPath, sqlCountOfAttempts, sqlWaitTime);
    db->setWaitFunction([=](size_t ms){
        // Construct a timer without setting an expiry time.
        deadline_timer timer(io_context_);
        // Set an expiry time relative to now.
        timer.expires_from_now(boost::posix_time::millisec(ms));
        // Wait for the timer to expire.
        timer.wait();
    });
    LOG_IF(WARNING, ! db->OpenConnection()) << "ERROR: can't connect to db: " <<db->GetLastError();
    IResult *res = db->ExecuteSelect(string("PRAGMA journal_mode = WAL; PRAGMA encoding = \"UTF-8\"; "
                                                      "PRAGMA foreign_keys = 1; PRAGMA page_size = " + std::to_string(blockOrClusterSize) + "; PRAGMA cache_size = -3000;").c_str());
    res->ReleaseStatement();

    last_ping_ = boost::posix_time::microsec_clock::local_time();

    do_read();
}

CClientSession::ptr CClientSession::new_(io_context& io_context, const size_t maxTimeout, businessLogic_ptr businessLogic)
{
    ptr new_(new CClientSession(io_context, maxTimeout, std::move(businessLogic)));
    return new_;
}

void CClientSession::stop()
{
    {
        boost::recursive_mutex::scoped_lock lk(cs_);

        if( !started_ )
            return;

        VLOG(1) << "DEBUG: stop client: " << username() << std::endl;

        started_ = false;
        sock_.close();
    }

    ptr self = shared_from_this();

    {
        boost::recursive_mutex::scoped_lock lk(clients_cs);
        auto it = std::find(clients.begin(), clients.end(), self);
        if(it != clients.end())
            clients.erase(it);
    }
    update_clients_changed();
}

bool CClientSession::started() const
{
    boost::recursive_mutex::scoped_lock lk(cs_);
    return started_;
}

ip::tcp::socket& CClientSession::sock()
{
    boost::recursive_mutex::scoped_lock lk(cs_);
    return sock_;
}

string CClientSession::username() const
{
    boost::recursive_mutex::scoped_lock lk(cs_);
	string usernameCopy(username_);
    return usernameCopy;
}

void CClientSession::set_clients_changed()
{
    boost::recursive_mutex::scoped_lock lk(cs_);
    clients_changed_ = true;
}

void CClientSession::on_read(const error_code &err, size_t bytes)
{
    if( err || (bytes > MAX_READ_BUFFER))
        stop();

    if( !started() )
        return;


    // process the msg

    // we must make copy of read_buffer_, for quick unlock cs_ mutex
    size_t len = strlen(read_buffer_.get());



    string inMsg;
    {
        boost::recursive_mutex::scoped_lock lk(cs_);
        for (size_t i = 0; i < len; ++i) {
            //continue if read_buffer_[i] == one of (\r, \n, NULL)
            if((read_buffer_[i] != char(0)) && (read_buffer_[i] != char(13))  && (read_buffer_[i] != char(10)))
                inMsg += read_buffer_[i];
        }
    }


    VLOG(1) << "DEBUG: received msg !!!" << inMsg << "!!!\nDEBUG: received bytes from user '" <<username() <<"' bytes: " << bytes;
            //<< " delay: " <<(boost::posix_time::microsec_clock::local_time() - last_ping_).total_milliseconds() <<"ms";

	/*
		Structure of command to server:
		{
			"command": "get_last_event",
			"params": {
					"message": ""
				}
		}

		Structure of answer from server:
			If positive:
				{
					"command": "get_last_event",
					"params": {
							"status": "ok"
						}
					"timestamp": 123455234, // in milliseconds!
					"server_datatime": "YYYY-MM-DD HH:MM:SS"
				}
			If negative (error occure):
				{
					"command": "get_last_event",
					"params": {
							"status": "error",
							"message": "Parser error"
						}
					"timestamp": 123455234,
					"server_datetime": ""
				}
	*/

	CJsonParser parser(inMsg);

	try {
		parser.validateData();

		string serverCommand(parser.parseCommand());

		if (serverCommand.empty()) {
			// check if it is ip camera event
			if (parser.isIpCameraEvent()) {
				on_ipcam_event(parser);
			} else {
				throw std::invalid_argument("not an event");
			}
		}else if(serverCommand == "get_last_event"){
            on_get_last_event();

		}else if(serverCommand == "login"){
			on_login(parser.parseMessage());

		}else if(serverCommand == "ping"){
			on_ping();

		}else if(serverCommand == "who"){
			on_clients();
	
		}else if(serverCommand == "fibo"){
			on_fibo(parser.parseMessage());
	
		}else if(serverCommand == "exit"){
			stop();
		}

	} catch (std::exception &ex) {
	    if(! inMsg.empty()){
            string errmsg(string(u8"WARNING: unexpected data: ") + ex.what());
            LOG(WARNING) <<errmsg;
            do_write(parser.buildAnswer(false, inMsg, errmsg));
	    }else{
            LOG(WARNING) <<"WARNING: received empty msg";
            do_read();
	    }
	}

}

void CClientSession::on_login(const string &username)
{
	{
		boost::recursive_mutex::scoped_lock lk(cs_);
		username_ = username;

		VLOG(1) << "DEBUG: logged in: " << username_ << std::endl;
	}

    do_write(CJsonParser::BuildAnswer(true, "login", "login OK"));

    update_clients_changed(); // this caused bug with dead lock when restore or backup db, I didn't tested fixed it or not
}

void CClientSession::on_ping()
{
	bool clients_changed;

	{
		boost::recursive_mutex::scoped_lock lk(cs_);
		clients_changed = clients_changed_;
	}

	string message = clients_changed ? string("client_list_changed") : string("");

    do_write(CJsonParser::BuildAnswer(true, "ping", message));

    // we have notified client, that clients list was changed yet,
    // so clients_changed_ should be false
    clients_changed_ = false;
}

void CClientSession::on_clients()
{
    cli_ptr_vector clients_copy;
    {
        boost::recursive_mutex::scoped_lock lk(clients_cs);
        clients_copy = clients;
    }

	pt::ptree clients;

	for (const auto &it : clients_copy) {
		clients.push_back(pt::ptree::value_type("", it->username()));
	}
	
	std::map<string, pt::ptree> params;
	params["clients"] = clients;

    do_write(CJsonParser::BuildAnswer(true, "who", "", params));
}

void CClientSession::on_check_ping()
{
    boost::recursive_mutex::scoped_lock lk(cs_);
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    time_duration::tick_type delay = (now - last_ping_).total_milliseconds();

    if( delay >= time_duration::tick_type(maxTimeout_ - 1) ){
        VLOG(1) << "DEBUG: stopping: " << username_ << " - no ping in time " <<delay;
        stop();
    }

    last_ping_ = boost::posix_time::microsec_clock::local_time();
}

void CClientSession::post_check_ping()
{
    boost::recursive_mutex::scoped_lock lk(cs_);
    timer_.expires_from_now(boost::posix_time::millisec(maxTimeout_));
    timer_.async_wait(bind(&CClientSession::on_check_ping, shared_from_this()));
}

void CClientSession::do_get_fibo(const size_t &n)
{
    //return n<=2 ? n: get_fibo(n-1) + get_fibo(n-2);
    size_t a = 1, b = 1;
    for( size_t i = 3; i <= n; i++ )
    {
        size_t c = a + b;
        a = b; b = c;
    }

    // next do_write should be without do_read after write. So we put false as second param to do_write
    do_write(CJsonParser::BuildAnswer(true, "fibo", std::to_string(b)), false);
}

void CClientSession::on_fibo(const string &number)
{
    std::istringstream in(number);
    size_t n;
    in >> n;
    VLOG(1) <<"fibo for: "<<n;
    auto self = shared_from_this();
    io_context_.post([self, this, n](){ do_get_fibo(n); });

    do_read();
}



void CClientSession::do_process_ipcam_event(const CJsonParser parser)
{
    if( ! started() ){
        return;
    }

	int effectedData = 0;
	SIpCameraEvent ipcamEvent;

	{
		boost::recursive_mutex::scoped_lock bd_;

		if (!db->isConnected()) {
			if (!db->OpenConnection()) {
				LOG(WARNING) << "ERROR: " + db->GetLastError();
				return;
			}
		}

		// parse ev
		ipcamEvent = parser.parseIpCameraEvent();

		// save to db
		effectedData = db->Execute(SIpCameraEvent::INSERT_EVENT_QUERY(ipcamEvent).c_str());
	}

	businessLogic_->setLastIpCamEvent(ipcamEvent);

    if (effectedData < 0){
        LOG(WARNING) << "ERROR: effected data < 0! : " << db->GetLastError();
    }else{
        VLOG(1) << "DEBUG: seved event: " <<parser.toPrettyJson();
    }

}

void CClientSession::on_ipcam_event(const CJsonParser &parser)
{
    if( !started() )
        return;

    io_context_.post(bind(&CClientSession::do_process_ipcam_event, shared_from_this(), parser));

    do_read();
}

void CClientSession::on_get_last_event() {
    try {
        string lastEvent(businessLogic_->getLastIpCamEvent().rawJson);

        if(lastEvent.empty()){
            throw std::invalid_argument("haven't any events");
        }

        CJsonParser eventParser(lastEvent);
        eventParser.validateData();

        std::map<string, pt::ptree> params;
        params["event"] = eventParser.getTree();

        do_write(eventParser.buildAnswer(true, "get_last_event", "", params));
    } catch (std::exception &ex) {
        string errmsg(string(u8"ERROR: ") + ex.what());
        LOG(WARNING) <<errmsg;
        do_write(CJsonParser::BuildAnswer(false, "get_last_event", errmsg));
    }
}



void CClientSession::do_read()
{
    {
        boost::recursive_mutex::scoped_lock lk(cs_);
        ZeroMemory(read_buffer_.get(), sizeof(char) * MAX_READ_BUFFER);
    }

    post_check_ping();

    async_read(sock_, buffer(read_buffer_.get(), MAX_READ_BUFFER), boost::asio::transfer_at_least(1),
                        bind(&CClientSession::on_read, shared_from_this(), _1, _2));

}

void CClientSession::do_write(const string &msg, bool read_on_write)
{
    if( !started() )
        return;

    {
        boost::recursive_mutex::scoped_lock lk(cs_);
        ZeroMemory(write_buffer_.get(), sizeof(char) * MAX_WRITE_BUFFER);
    }

    std::copy(msg.begin(), msg.end(), write_buffer_.get());

    auto self(shared_from_this());

    async_write(sock_, buffer(write_buffer_.get(), msg.size()),
                [this, self, read_on_write](error_code, size_t){
                    if(read_on_write){
                        do_read();
                    }
                });

}

void update_clients_changed()
{
    cli_ptr_vector clients_copy;

//    // C++17 only
//    if(boost::recursive_mutex::scoped_lock lk(clients_cs); !clients.empty()){
//
//        clients_copy = clients;
//    }

    {
        boost::recursive_mutex::scoped_lock lk(clients_cs);
        clients_copy = clients;
    }

    for(const auto &it : clients_copy )
        it->set_clients_changed();
}
