import time
import sys

def read_write():
    timestr = str(int(time.time()))
    sys.stdout.write('$TTL 300\n@ IN SOA vps3.yoursunny.com. traceart-ns.mailinator.com. (\n\t\t ' + timestr +'\n\t\t 604800\n\t\t 86400\n\t\t 2419200\n\t\t 300 );\n\n@ IN NS vps3.yoursunny.com.\n\n')

    for line in sys.stdin:
        line = line.strip()
        alist = line.split()
        alist[0] = reversed(alist[0])
        newstr = ''
        for item in alist[0]:
            newstr += item + '.'
        numstr = newstr[:-1]
        fullstr = numstr + ' IN PTR ' + alist[1] + '.'
        sys.stdout.write(fullstr + '\n')


def main():
    read_write()

if __name__ == '__main__':
    main()
