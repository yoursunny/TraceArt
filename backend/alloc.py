import sys

def r_w():
    file1 = open(sys.argv[1])
    lastnum = 000000
    for line in file1:
        sys.stdout.write(line)
        if '[' in line:
            anum = int(line[1:-2],16)
            if anum >= lastnum:
                lastnum = anum

    newnum = lastnum + 1
    nnum = "%06X" % newnum
    sys.stdout.write('[' + nnum + ']\n')

    i = 1
    for line in sys.stdin:
        line = line.strip()
        numstr = "%02X" % i
        i += 1
        fullstr = numstr + ' ' + line
        sys.stdout.write(fullstr + '\n')

def main():
    r_w()

if __name__ == '__main__':
    main()
