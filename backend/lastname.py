import sys

def last():
    for line in sys.stdin:
        if '[' in line:
            str1 = line[1:-2]

    sys.stdout.write(str1 + '.u.traceart.yoursunny.cn')

def main():
    last()

if __name__ == '__main__':
    main()
