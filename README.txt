Compile scripts, then run server first.
Server will prompt for blackList file.
Enter blacklist file name with extension.

Server will listen for connections. 
Run client.
Client and server will connect.
 
Client will prompt for URL
Enter in URL as usual, for example: www.google.com

Server will now prompt for Cached List
Enter in cached list name with extension.

Server will now attempt to find in cache.
If found, sends cache file contents to client.
If not found, checks blaclist for request
If in blacklist, server rejects and tells client.
If not in blacklist, server requests URL and caches it
Then saves the file.