#include "crails/server.hpp"
#include "crails/params.hpp"
#include <crails/logger.hpp>
#include <condition_variable>

using namespace std;
using namespace Crails;

#ifdef ASYNC_SERVER
void BodyParser::wait_for_body(const HttpServer::request& request, ServerTraits::Response response, Params& params)
{
  mutex              sem_mutex;
  unique_lock<mutex> sem_lock(sem_mutex);
  condition_variable sem;
  unsigned int       to_read    = params["headers"]["Content-Length"].defaults_to<unsigned int>(0);
  unsigned int       total_read = 0;
  std::string        read_buffer;
  Regex              get_boundary;
  
  callback = [this, &sem, &to_read, &total_read, &read_buffer, &response](boost::iterator_range<char const*> range,
                        boost::system::error_code error_code,
                        size_t size_read,
                        HttpServer::connection_ptr connection_ptr)
  {
    logger << Logger::Info << "Reading buffer..." << Logger::endl;
    for (unsigned int i = 0 ; i < size_read ; ++i)
      read_buffer += range[i];
    total_read += size_read;
    if (total_read == to_read)
    {
      logger << Logger::Info << "Finished reading..." << Logger::endl;
      sem.notify_all();
    }
    else
      response->read(callback);
  };
  response->read(callback);
  sem.wait(sem_lock);
  body_received(request, response, params);
  params.response_parsed.notify_all();
}
#else
void BodyParser::wait_for_body(const HttpServer::request& request, ServerTraits::Response response, Params& params)
{
  body_received(request, response, params);
}
#endif
