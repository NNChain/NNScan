vcl 4.0;

backend rest {
  .host = "172.18.0.20";
  .port = "1317";
}

backend bandsv {
  .host = "172.18.0.16";
  .port = "5000";
}

backend hasura {
  .host = "172.18.0.89";
  .port = "5433";
}

backend faucet {
  .host = "172.18.0.17";
  .port = "5005";
}

backend static {
  .host = "127.0.0.1";
  .port = "80";
}

sub vcl_recv {
  if (req.method == "OPTIONS") {
    return (synth(200));
  }

  if (req.url ~ "^/bandsv/") {
    set req.url = regsub(req.url, "^/bandsv", "/");
    set req.backend_hint = bandsv;
  } else if (req.url ~ "^/rest/") {
    set req.url = regsub(req.url, "^/rest/", "/");
    set req.backend_hint = rest;
    if (req.url ~ "^/auth/accounts/") {
      return (pass);
    }
  } else if (req.url ~ "^/faucet/"){
    set req.url = regsub(req.url, "^/faucet/", "/");
    set req.backend_hint = faucet;
  } else if (req.url ~ "^/v1/graphql") {
    // GraphQL endpoint - route to Hasura
    set req.backend_hint = hasura;
    if (req.http.upgrade ~ "(?i)websocket") {
      return (pipe);
    }
  } else {
    // All other routes (SPA routes like /proposals, /validators, etc.) - serve static files
    set req.backend_hint = static;
    // For SPA routing, serve index.html for all non-file requests
    if (req.url !~ "\.(css|js|png|jpg|jpeg|gif|ico|svg|woff|woff2|ttf|eot|json)$") {
      set req.url = "/index.html";
    }
  }
}

sub vcl_pipe {
  if (req.http.upgrade) {
    set bereq.http.upgrade = req.http.upgrade;
    set bereq.http.connection = req.http.connection;
  }
}

sub vcl_backend_response {
  set beresp.ttl = 3s;
}

sub cors {
  if (req.url ~ "/") {
    set resp.http.Access-Control-Allow-Origin = "*";
    set resp.http.Access-Control-Allow-Methods = "GET, OPTIONS, POST, PATCH, PUT, DELETE";
    set resp.http.Access-Control-Allow-Headers = "Origin, Accept, Content-Type, X-Requested-With, X-CSRF-Token";
  }
}

sub vcl_synth {
  call cors;
}

sub vcl_deliver {
  call cors;
}
