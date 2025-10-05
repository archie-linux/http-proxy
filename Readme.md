#### Setup Proxy Server

- g++ --std=c++14 -o proxy proxy.cpp
- ./proxy

#### Run script

- python test.py

#### Send HTTP Request

<pre>
Request:  curl -x 127.0.0.1:8080 http://example.com

### Server Logs

Proxy server started on port 8080

Request: 

GET http://example.com/ HTTP/1.1
Host: example.com
User-Agent: curl/7.84.0
Accept: */*
Proxy-Connection: Keep-Alive


Response: 

HTTP/1.1 200 OK
Age: 127042
Cache-Control: max-age=604800
Content-Type: text/html; charset=UTF-8
Date: Thu, 29 Feb 2024 20:51:24 GMT
Etag: "3147526947+ident"
Expires: Thu, 07 Mar 2024 20:51:24 GMT
Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT
Server: ECS (cha/81B9)
Vary: Accept-Encoding
X-Cache: HIT
Content-Length: 1256

<!doctype html>
<html>
<head>
    <title>Example Domain</title>

    <meta charset="utf-8" />
    <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <style type="text/css">
    body {
        background-color: #f0f0f2;
        margin: 0;
        padding: 0;
        font-family: -apple-system, system-ui, BlinkMacSystemFont, "Segoe UI", "Open Sans", "Helvetica Neue", Helvetica, Arial, sans-serif;
        
    }
    div {
        width: 600px;
        margin: 5em auto;
        padding: 2em;
        background-color: #fdfdff;
        border-radius: 0.5em;
        box-shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);
    }
    a:link, a:visited {
        color: #38488f;
        text-decoration: none;
    }
    @media (max-width: 700px) {
        div {
            margin: 0 auto;
            width: auto;
        }
    }
    </style>    
</head>

<body>
<div>
    <h1>Example Domain</h1>
    <p>This domain is for use in illustrative examples in documents. You may use this
    domain in literature without prior coordination or asking for permission.</p>
    <p><a href="https://www.iana.org/domains/example">More information...</a></p>
</div>
</body>
</html>

</pre>

<pre>
Request 1:  curl -x 127.0.0.1:8080 http://httpbin.org/status/200

### Server Logs

Request: 

GET http://httpbin.org/status/200 HTTP/1.1
Host: httpbin.org
User-Agent: curl/7.84.0
Accept: */*
Proxy-Connection: Keep-Alive


Response: 

HTTP/1.1 200 OK
Date: Thu, 29 Feb 2024 20:52:30 GMT
Content-Type: text/html; charset=utf-8
Content-Length: 0
Connection: keep-alive
Server: gunicorn/19.9.0
Access-Control-Allow-Origin: *
Access-Control-Allow-Credentials: true

Request 2:  curl -x 127.0.0.1:8080 http://httpbin.org/status/404

### Server Logs

Request: 

GET http://httpbin.org/status/404 HTTP/1.1
Host: httpbin.org
User-Agent: curl/7.84.0
Accept: */*
Proxy-Connection: Keep-Alive


Response: 

HTTP/1.1 404 NOT FOUND
Date: Thu, 29 Feb 2024 20:52:35 GMT
Content-Type: text/html; charset=utf-8
Content-Length: 0
Connection: keep-alive
Server: gunicorn/19.9.0
Access-Control-Allow-Origin: *
Access-Control-Allow-Credentials: true

</pre>

<pre>
Request: curl -x 127.0.0.1:8080 http://httpbin.org/get

### Server Logs

Request: 

GET http://httpbin.org/get HTTP/1.1
Host: httpbin.org
User-Agent: curl/7.84.0
Accept: */*
Proxy-Connection: Keep-Alive


Response: 

HTTP/1.1 200 OK
Date: Thu, 29 Feb 2024 20:55:00 GMT
Content-Type: application/json
Content-Length: 293
Connection: keep-alive
Server: gunicorn/19.9.0
Access-Control-Allow-Origin: *
Access-Control-Allow-Credentials: true

{
  "args": {}, 
  "headers": {
    "Accept": "*/*", 
    "Host": "httpbin.org", 
    "Proxy-Connection": "Keep-Alive", 
    "User-Agent": "curl/7.84.0", 
    "X-Amzn-Trace-Id": "Root=1-65e0ef24-17fdae242e84d3f40a865ed5"
  }, 
  "origin": "172.93.207.41", 
  "url": "http://httpbin.org/get"
}

</pre>

<pre>
Request: curl -x 127.0.0.1:8080 http://httpbin.org/post -X POST -H "Content-Type: application/json" -d '{"name": "bob", "age": 25}'

### Server Logs

Request: 

POST http://httpbin.org/post HTTP/1.1
Host: httpbin.org
User-Agent: curl/7.84.0
Accept: */*
Proxy-Connection: Keep-Alive
Content-Type: application/json
Content-Length: 26

{"name": "bob", "age": 25}
Response: 

HTTP/1.1 200 OK
Date: Thu, 29 Feb 2024 20:56:27 GMT
Content-Type: application/json
Content-Length: 494
Connection: keep-alive
Server: gunicorn/19.9.0
Access-Control-Allow-Origin: *
Access-Control-Allow-Credentials: true

{
  "args": {}, 
  "data": "{\"name\": \"bob\", \"age\": 25}", 
  "files": {}, 
  "form": {}, 
  "headers": {
    "Accept": "*/*", 
    "Content-Length": "26", 
    "Content-Type": "application/json", 
    "Host": "httpbin.org", 
    "Proxy-Connection": "Keep-Alive", 
    "User-Agent": "curl/7.84.0", 
    "X-Amzn-Trace-Id": "Root=1-65e0ef7b-0e995e430aa153200d6322c9"
  }, 
  "json": {
    "age": 25, 
    "name": "bob"
  }, 
  "origin": "172.93.207.41", 
  "url": "http://httpbin.org/post"
}

</pre>