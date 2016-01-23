# backend

This is the backend program installed on `https://api.traceart.yoursunny.cn`.  
This directory should be copied to `/home/traceart/backend` on the server.

## Deployment

Compile `bind-reload` program under root privilege so web server can restart `bind9`:

    gcc -o bind-reload bind-reload.c
    chown root:root /home/traceart/bind-reload
    chmod 4755 /home/traceart/bind-reload

Create necessary files and allow web server to overwrite them:

    touch alloc.txt flat.txt db.traceart
    chmod 666 alloc.txt flat.txt db.traceart

Delegate reverse zone to the server, and set zone file to be `db.traceart`.

Configure nginx to run PHP5 in the backend directory.
