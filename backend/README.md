# backend

This is the backend program installed on `https://api.traceart.yoursunny.cn`.  
This directory should be copied to `/home/traceart/backend` on the server.

## Deployment

Compile `bind-reload` program under root privilege so web server can restart `bind9`:

    cd /home/traceart/
    gcc -o bind-reload bind-reload.c
    chown root:root bind-reload
    chmod 4755 bind-reload

Create necessary files and allow web server to overwrite them:

    touch alloc.txt flat.txt db.traceart db.traceart.yoursunny.cn
    chmod 666 alloc.txt flat.txt db.traceart db.traceart.yoursunny.cn

Delegate reverse zone to the server, and set zone file to be `/home/traceart/db.traceart`.

Delegate forward zone `traceart.yoursunny.cn` to the server, and set zone file to be `/home/traceart/db.traceart.yoursunny.cn`.

Configure nginx to run PHP5 in the backend directory.
