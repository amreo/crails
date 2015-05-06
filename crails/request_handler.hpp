#ifndef  REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include "http_server.hpp"

class RequestHandler
{
public:
  RequestHandler(const std::string& name) : name(name) {}
  virtual ~RequestHandler() {}

  const std::string& get_name(void) const { return name; }

  virtual bool operator()(const Server::request& request, BuildingResponse& response, Params& params) = 0;
  
private:
  const std::string name;
};

#endif