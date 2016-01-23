# read input.txt

# write db.txt that looks same as db.he-ipv6
import time

def read_write():
    afile = open('input.txt')
    nfile = open('output.txt','w')
    timestr = str(int(time.time()))
    nfile.write('$TTL 300\n@ IN SOA vps3.yoursunny.com. traceart-ns.mailinator.com. (\n\t\t ' + timestr +'\n\t\t 604800\n\t\t 86400\n\t\t 2419200\n\t\t 300 );\n\n@ IN NS vps3.yoursunny.com.\n\n')

    for line in afile:
        line = line.strip()
        alist = line.split()
        alist[0] = reversed(alist[0])
        newstr = ''
        for item in alist[0]:
            newstr += item + '.'
        numstr = newstr[:-1]
        fullstr = numstr + ' IN PTR ' + alist[1] + '.'
        nfile.write(fullstr + '\n')

    afile.close()
    nfile.close()

def main():
    read_write()

if __name__ == '__main__':
    main()
