import sys
import time

def forward():
    timestr = str(int(time.time()))
    sys.stdout.write('$TTL 300\n@ IN SOA vps3.yoursunny.com. traceart-ns.mailinator.com. (\n\t\t ' + timestr +'\n\t\t 604800\n\t\t 86400\n\t\t 2419200\n\t\t 300 );\n\n@ IN NS vps3.yoursunny.com.\n\n@       IN      A       199.175.53.34\n@       IN      AAAA    2607:8b00:2::72cf:943a\napi     IN      CNAME   vps3.yoursunny.com.\n')

    astr = '.u IN AAAA 2001:0470:1f11:04a0:5441:7274:'

    alist = []

    for line in sys.stdin:
        if '[' in line:
            num1 = line[1:-2]
            alist.append(' ')

        else:
            num2 = line[0:2]
            alist[-1] = num1 + num2

    for item in alist:
            fullstr = item[:-2] + astr + item[:-4] + ':' + item[-4:] + '\n'
            sys.stdout.write(fullstr)

def main():
    forward()

if __name__ == '__main__':
    main()
