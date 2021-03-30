from threading import *
from time import sleep

class Hello(Thread):
    def run(self):
        for i in range(50):
            print("hello")
            sleep(.01)


class Hi(Thread):
    def run(Thread):
        for i in range(50):
            print("Hi")
            sleep(.01)

t1 = Hello()
t2 = Hi()

t1.start()


print("Bye")

t2.start()