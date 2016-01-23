import sys

def trans():
    for line in sys.stdin:
        if '[' in line:
            numline = line[1:-2]
        else:
            sys.stdout.write(numline + line)

def main():
    trans()

if __name__ == '__main__':
    main()
