#include "crails/router.hpp"
#include "crails/session_store.hpp"
#include "crails/http.hpp"

using namespace std;

Router::Action Router::get_action(const string& method, const string& uri, Params& query_params) const
{
  auto it  = routes.begin();
  auto end = routes.end();

  // Searching and executing the first matching route
  for (; it != end ; ++it)
  {
    const Item&    item     = *it;
    unsigned short n_params = item.param_names.size();
    regmatch_t*    params   = new regmatch_t[n_params + 1];

    if ((item.method == "" || item.method == method) &&
        !(item.regex.Match(uri, params, n_params + 1)))
    {
      for (unsigned short i = 1 ; i <= n_params ; ++i)
      {
        regmatch_t  match       = params[i];
        std::string param_name  = item.param_names[i -1];
        std::string param_value = uri.substr(match.rm_so, match.rm_eo - match.rm_so);

        query_params[param_name] = param_value;
      }
      delete[] params;
      return item.run;
    }
    delete[] params;
  }
  throw Exception404(); // No route matched, throw 404 Error
}

void Router::Match(const std::string& route, Router::Action callback)
{
  Item item;
  
  ItemInitializeRegex(item, route);
  item.run = callback;
  routes.push_back(item);
}

void Router::Match(const std::string& method, const std::string& route, Router::Action callback)
{
  Item item;
  
  ItemInitializeRegex(item, '^' + route + '$');
  item.method = method;
  item.run    = callback;
  routes.push_back(item);
}

void Router::ItemInitializeRegex(Item& item, std::string route)
{
  Regex       find_params;

  find_params.SetPattern(":[a-zA-Z0-9_-]*");
  {
    std::string regexified_route;
    regmatch_t  match;

    while (!(find_params.Match(route, &match, 1)))
    {
      regexified_route += route.substr(0, match.rm_so);
      regexified_route += "([a-zA-Z0-9_-]*)";
      item.param_names.push_back(route.substr(match.rm_so + 1, match.rm_eo - match.rm_so - 1));
      route             = route.substr(match.rm_eo);
    }
    regexified_route += route;
    std::cout << regexified_route << std::endl;
    item.regex.SetPattern(regexified_route, REG_EXTENDED);
  }
}
