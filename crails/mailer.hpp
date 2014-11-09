#ifndef  MAILER_HPP
# define MAILER_HPP

# include "mail_servers.hpp"
# include "controller_base.hpp"

class Mailer
{
public:
  Mailer(ControllerBase& controller, const std::string& configuration);

protected:
  Smtp::Mail   mail;
  SharedVars&  vars;
  Params&      params;

  void render(const std::string& view, const std::string& layout = "");
  void send(void);
  
private:
  ControllerBase& controller;
  std::string    configuration;
  Smtp::Server   smtp_server;
  bool           is_connected;
};

#endif