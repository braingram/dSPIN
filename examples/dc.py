#!/usr/bin/env python


N = 2
values = [0x00000000, 0x00000000]
commands = [0x00, 0x00]
n_bytes = [0x00, 0x00]


def tx(v):
    print("tx: %r" % hex(v))


def unpack(v, i):
    return ((v >> (8 * i)) & 0xff)


def csh():
    print("cs: high")


def csl():
    print("cs: low")


def send():
    # send commands
    csl()
    for i in xrange(N):
        print "command:",
        tx(commands[i])
    csh()

    mb = max(n_bytes)
    # send/receive values
    for j in xrange(mb):
        csl()
        for i in xrange(N):
            if n_bytes[i]:
                o = n_bytes[i] - 1
                tx(unpack(values[i], o))
                n_bytes[i] = o
        csh()


values = [0x00000000, 0x00000000]
commands = [0x00, 0x00]
n_bytes = [0x00, 0x00]
print("test transfer 1")
send()

values = [0x00000000, 0x00000000]
commands = [0x01, 0x02]
n_bytes = [0x00, 0x00]
print("test transfer 2")
send()


values = [0x000000ef, 0x0000abcd]
commands = [0x01, 0x02]
n_bytes = [0x01, 0x02]
print("test transfer 3")
send()

values = [0x000000ef, 0x0000abcd]
commands = [0x01, 0x02]
n_bytes = [0x03, 0x03]
print("test transfer 4")
send()
